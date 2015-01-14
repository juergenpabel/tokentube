#define _GNU_SOURCE 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/mman.h>
#include <sys/prctl.h>
#include <fcntl.h>
#include <dirent.h>
#include <limits.h>
#include <confuse.h>
#include <tokentube.h>


#define SSOD_SOCKET TT_FILENAME__SSOD_INITRAMFS_DIR "/" TT_FILENAME__SSOD_TOKENTUBE_DIR "/" TT_FILENAME__SSOD_SOCKET


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
		unlink( cfg_getstr( g_cfg, "ssod|socket" ) );
		cfg_free( g_cfg );
	}
	exit( code );
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
	path = cfg_getstr( g_cfg, "greeters|listing" );
	if( path == NULL ) {
		return TT_YES;
	}
	snprintf( filename, sizeof(filename), "/proc/%d/exe", (int)usercred.pid );
	if( realpath( filename, peer ) == NULL ) {
		return TT_ERR;
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
	int			i, sock_local, sock_remote;
	struct sockaddr_un	sa_local = {0};

	if( argc != 2 ) {
		fprintf( stderr, "usage: ssod <CONFIG_FILE>\n" );
		ssod_punt( -1 );
	}
	g_cfg = cfg_init( opt_sso, CFGF_NONE );
	if( g_cfg == NULL ) {
		ssod_punt( -1 );
	}
	if( cfg_parse( g_cfg, argv[1] ) != 0 ) {
		ssod_punt( -1 );
	}
	if( access( cfg_getstr( g_cfg, "ssod|socket" ), F_OK ) == 0 ) {
		ssod_punt( -1 );
	}
	if( getuid() == 0 && geteuid() == 0 ) {
		setsid();
		umask( 077 );
		if( chdir( TT_FILENAME__SSOD_INITRAMFS_DIR ) < 0 ) {
			ssod_punt(-1);
		}
		if( chdir( TT_FILENAME__SSOD_TOKENTUBE_DIR ) < 0 ) {
			ssod_punt(-3);
		}
		prctl( PR_SET_DUMPABLE, 0 );
		mlockall( MCL_CURRENT|MCL_FUTURE );
	}
	sock_local = socket( AF_UNIX, SOCK_STREAM, 0 );
	if( sock_local < 0 ) {
		ssod_punt( -1 );
	}
	sa_local.sun_family = AF_UNIX;
	strncpy( sa_local.sun_path, cfg_getstr( g_cfg, "ssod|socket" ), sizeof(sa_local.sun_path)-1 );
	if( bind( sock_local, (struct sockaddr*)&sa_local, sizeof(sa_local.sun_family) + strnlen(sa_local.sun_path, sizeof(sa_local.sun_path) ) ) < 0 ) {
		ssod_punt(-1 );
	}
	if( chmod( cfg_getstr( g_cfg, "ssod|socket" ), S_IWUSR|S_IRUSR|S_IWGRP|S_IRGRP|S_IWOTH|S_IROTH ) < 0 ) {
		ssod_punt(-1 );
	}
	if( listen( sock_local, 1 ) < 0 ) {
		ssod_punt( -1 );
	}
	state = PEER_PBA;
	kill( getppid(), SIGCONT );
	for(;;) {
		sock_remote = accept( sock_local, NULL, NULL );
		if( sock_remote < 0 ) {
			fprintf( stderr, "SSOD: accept() failed\n" );
			ssod_punt( -1 );
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
					ssod_punt( 0 );
				}
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
				cfg_free( g_cfg );
				g_cfg = cfg_init( opt_sso, CFGF_NONE );
				if( g_cfg == NULL ) {
					ssod_punt( -1 );
				}
				if( cfg_parse( g_cfg, argv[1] ) != 0 ) {
					ssod_punt( -1 );
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
				fprintf( stderr, "SSOD: internal error\n" );
				close( sock_remote );
				close( sock_local );
				ssod_punt( -1 );
		}
		close( sock_remote );
	}
	fprintf( stderr, "SSOD: internal error\n" );
	ssod_punt( -1 );
}

