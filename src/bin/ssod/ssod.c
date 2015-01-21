#define _GNU_SOURCE 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <syslog.h>
#include <sys/ptrace.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/mman.h>
#include <sys/prctl.h>
#include <fcntl.h>
#include <dirent.h>
#include <libgen.h>
#include <limits.h>
#include <confuse.h>
#include <tokentube.h>


#define SSOD_SOCKET		TT_FILENAME__SSOD_INITRAMFS_DIR "/" TT_FILENAME__SSOD_TOKENTUBE_DIR "/" TT_FILENAME__SSOD_SOCKET


typedef enum {
        PEER_UNKNOWN = 0,
        PEER_PBA = 1,
        PEER_GREETER = 2
} ssod_peer_t;


static cfg_opt_t opt_sso_greeters[] = {
	CFG_STR("listing", NULL, CFGF_NONE),
	CFG_INT("uid-min", 0, CFGF_NONE),
	CFG_INT("uid-max", INT_MAX, CFGF_NONE),
	CFG_END()
};

static cfg_opt_t opt_sso_ssod[] = {
	CFG_STR("executable", NULL, CFGF_NONE),
	CFG_STR("socket", SSOD_SOCKET, CFGF_NONE),
	CFG_END()
};

static cfg_opt_t opt_sso[] = {
	CFG_FUNC("include", cfg_include),
	CFG_SEC("ssod", opt_sso_ssod, CFGF_NONE),
	CFG_SEC("greeters", opt_sso_greeters, CFGF_NONE),
	CFG_END()
};


static char	g_username[TT_USERNAME_CHAR_MAX+1] = {0};
static char	g_password[TT_PASSWORD_CHAR_MAX+1] = {0};
static cfg_t*	g_cfg = NULL;


static void ssod_punt(int code) {
	memset( g_username, '\0', sizeof(g_username) );
	memset( g_password, '\0', sizeof(g_password) );
	if( g_cfg != NULL ) {
		cfg_free( g_cfg );
	}
	closelog();
	if( code > 0 && code != SIGCHLD ) {
		syslog( LOG_ERR, "SSOD: exiting from ssod.c:%d\n", code );
		exit( -1 );
	}
	exit( 0 );
}


static int ssod_verifypeer(int sock) {
	char		filename[FILENAME_MAX+1] = {0};
	char		greeter[FILENAME_MAX+1] = {0};
	char		peer[FILENAME_MAX+1] = {0};
	struct ucred	usercred = {0};
	socklen_t	uclen = sizeof(usercred);
	char*		path = NULL;
	DIR*		dir = NULL;
	struct dirent*	entry = NULL;

	if( getsockopt( sock, SOL_SOCKET, SO_PEERCRED, &usercred, &uclen ) < 0 ) {
		return TT_ERR;
	}
	if( usercred.uid < cfg_getint( g_cfg, "greeters|uid-min" ) || usercred.uid > cfg_getint( g_cfg, "greeters|uid-max" ) ) {
		return TT_NO;
	}
	snprintf( filename, sizeof(filename), "/proc/%d/exe", (int)usercred.pid );
	if( realpath( filename, peer ) == NULL ) {
		return TT_ERR;
	}
	path = cfg_getstr( g_cfg, "greeters|listing" );
	if( path == NULL ) {
		return TT_YES;
	}
	dir = opendir( path );
	if( dir == NULL ) {
		return TT_ERR;
	}
	entry = readdir( dir );
	while( entry != NULL ) {
		snprintf( filename, sizeof(filename), "%s/%s", path, entry->d_name );
		if( realpath( filename, greeter ) != NULL ) {
			if( strncmp( peer, greeter, FILENAME_MAX ) == 0 ) {
				closedir( dir );
				return TT_YES;
			}
		}
		entry = readdir( dir );
	}
	closedir( dir );
	return TT_NO;
}


int main(int argc, char* argv[]) {
	ssod_peer_t		state = PEER_UNKNOWN;
	int			i, fd[2], sock_local, sock_remote;
	struct sockaddr_un	sa = {0};
	struct stat		st1, st2;
	pid_t			other;

	if( argc != 2 ) {
		fprintf( stderr, "usage: ssod <CONFIG_FILE>\n" );
		exit( -1 );
	}
	openlog( NULL, LOG_PID|LOG_CONS, LOG_USER );
	if( getuid() == 0 && geteuid() == 0 ) {
		for( i=getdtablesize(); i>0; i-- ) {
			close( i-1 );
		}
		(void)open( "/dev/null", O_RDWR );
		(void)dup2( 0, 1 );
		(void)dup2( 0, 2 );
		umask( 077 );
		prctl( PR_SET_DUMPABLE, 0 );
		mlockall( MCL_CURRENT|MCL_FUTURE );
		setsid();
	}
	signal( SIGCHLD, ssod_punt );
	if( pipe( fd ) < 0 ) {
		ssod_punt( __LINE__ );
	}
	other = fork();
	switch( other ) {
		case -1:
			ssod_punt( __LINE__ );
		case 0:
			other = getppid();
			close( fd[1] );
			while( read( fd[0], &fd[1], sizeof(fd[1]) ) != 0 ) { sleep(1); };
			close( fd[0] );
			break;
		default:
			close( fd[0] );
			if( prctl(PR_SET_PTRACER, other, 0, 0, 0 ) < 0 ) {
				ssod_punt( __LINE__ );
			}
			close( fd[1] );
	}
	if( ptrace(PTRACE_SEIZE, other, NULL, NULL ) < 0 ) {
		ssod_punt( __LINE__ );
	}
	if( other != getppid() ) {
		waitpid( other, &i, 0 );
		ssod_punt( 0 );
	}
	g_cfg = cfg_init( opt_sso, CFGF_NONE );
	if( g_cfg == NULL ) {
		ssod_punt( __LINE__ );
	}
	if( cfg_parse( g_cfg, argv[1] ) != 0 ) {
		ssod_punt( __LINE__ );
	}
	if( access( cfg_getstr( g_cfg, "ssod|socket" ), F_OK ) == 0 ) {
		ssod_punt( __LINE__ );
	}
	sa.sun_family = AF_UNIX;
	strncpy( sa.sun_path, cfg_getstr( g_cfg, "ssod|socket" ), sizeof(sa.sun_path)-1 );
	sock_local = socket( AF_UNIX, SOCK_STREAM, 0 );
	if( sock_local < 0 ) {
		ssod_punt( __LINE__ );
	}
	if( bind( sock_local, (struct sockaddr*)&sa, sizeof(sa.sun_family) + strnlen(sa.sun_path, sizeof(sa.sun_path) ) ) < 0 ) {
		ssod_punt( __LINE__ );
	}
	if( chmod( cfg_getstr( g_cfg, "ssod|socket" ), S_IWUSR|S_IRUSR|S_IWGRP|S_IRGRP|S_IWOTH|S_IROTH ) < 0 ) {
		ssod_punt( __LINE__ );
	}
	if( listen( sock_local, 1 ) < 0 ) {
		ssod_punt( __LINE__ );
	}
	if( chdir( dirname( cfg_getstr( g_cfg, "ssod|socket" ) ) ) < 0 ) {
		ssod_punt( __LINE__ );
	}
	state = PEER_PBA;
	for(;;) {
		sock_remote = accept( sock_local, NULL, NULL );
		if( sock_remote < 0 ) {
			ssod_punt( __LINE__ );
		}
		switch( state ) {
			case PEER_PBA:
				memset( g_username, '\0', sizeof(g_username) );
				for( i=0; i<(int)sizeof(g_username)-1; i++ ) {
					if( read( sock_remote, &g_username[i], 1) == 1) {
						if( g_username[i] == '\n' ) {
							g_username[i] = '\0';
							break;
						}
					}
				}
				if( strnlen( g_username, sizeof(g_username) ) == 0 ) {
					break;
				}
				memset( g_password, '\0', sizeof(g_password) );
				for( i=0; i<(int)sizeof(g_password)-1; i++ ) {
					if( read( sock_remote, &g_password[i], 1) == 1) {
						if( g_password[i] == '\n' ) {
							g_password[i] = '\0';
							break;
						}
					}
				}
				state = PEER_GREETER;
				break;
			case PEER_GREETER:
				if( getuid() == 0 && geteuid() == 0 ) {
					do {
						if( stat( ".", &st1 ) < 0 ) {
							ssod_punt( __LINE__ );
						}
						chdir( ".." );
						if( stat( ".", &st2 ) < 0 ) {
							ssod_punt( __LINE__ );
						}
					} while( st1.st_dev != st2.st_dev || st1.st_ino != st2.st_ino );
					chroot( "." );
					if( chdir( dirname( cfg_getstr( g_cfg, "ssod|socket" ) ) ) < 0 ) {
						ssod_punt( __LINE__ );
					}
					cfg_free( g_cfg );
					g_cfg = cfg_init( opt_sso, CFGF_NONE );
					if( g_cfg == NULL ) {
						ssod_punt( __LINE__ );
					}
					if( cfg_parse( g_cfg, "/etc/tokentube/sso.conf" ) != 0 ) {
						ssod_punt( __LINE__ );
					}
				}
				if( ssod_verifypeer( sock_remote ) == TT_YES ) {
					write( sock_remote, g_username, strnlen( g_username, sizeof(g_username) ) );
					write( sock_remote, "\n", 1 );
					write( sock_remote, g_password, strnlen( g_password, sizeof(g_password) ) );
					write( sock_remote, "\n", 1 );
				}
				close( sock_remote );
				close( sock_local );
				ssod_punt( 0 );
				break;
			default:
				close( sock_remote );
				close( sock_local );
				unlink( cfg_getstr( g_cfg, "ssod|socket" ) );
				ssod_punt( __LINE__ );
		}
		close( sock_remote );
	}
	ssod_punt( __LINE__ );
	exit( -1 );
}

