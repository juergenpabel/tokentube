#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <libgen.h>
#include <getopt.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <confuse.h>
#include <tokentube.h>
#include <tokentube/plugin.h>
#include "plugin.h"
#include "pba.h"


static tt_library_t	g_library;
static char* 		g_conf_user_userprompt = NULL;
static char*		g_conf_user_passprompt = NULL;
static int   		g_conf_sso = TT_NO;
static char*		g_conf_sso_exec = NULL;
static char*		g_conf_sso_socket = NULL;
static char*		g_conf_otp_identifier = NULL;


static const struct option long_options[] = {
	{ "fallback",    required_argument, 0, 'x' },
	{ "boot-device", required_argument, 0, 'b' },
	{ "config",      required_argument, 0, 'c' },
	{ "sso",         required_argument, 0, 's' },
	{ "username",    required_argument, 0, 'u' },
	{ "password",    required_argument, 0, 'p' },
	{ "verbose",     no_argument,       0, 'v' },
	{ 0, 0, 0, 0 }
};


static cfg_opt_t opt_pba_splash[] = {
	CFG_STR("background-image", NULL, CFGF_NONE),
	CFG_STR("message", NULL, CFGF_NONE),
	CFG_INT("duration", 0, CFGF_NONE),
	CFG_BOOL("confirm", 0, CFGF_NONE),
	CFG_END()
};

static cfg_opt_t opt_pba_sso[] = {
	CFG_STR("executable", NULL, CFGF_NONE),
	CFG_STR("socket", NULL, CFGF_NONE),
	CFG_END()
};

static cfg_opt_t opt_pba_user[] = {
	CFG_STR("title", NULL, CFGF_NONE),
	CFG_STR("background-image", NULL, CFGF_NONE),
	CFG_STR("prompt-username", "Username:", CFGF_NONE),
	CFG_STR("prompt-password", "Password:", CFGF_NONE),
	CFG_STR("default-username", NULL, CFGF_NONE),
	CFG_END()
};

static cfg_opt_t opt_pba_otp[] = {
	CFG_STR("title", NULL, CFGF_NONE),
	CFG_STR("background-image", NULL, CFGF_NONE),
	CFG_STR("prompt-identifier", "Identifier:", CFGF_NONE),
	CFG_STR("prompt-challenge", "Challenge:", CFGF_NONE),
	CFG_STR("prompt-response", "Response:", CFGF_NONE),
	CFG_STR("default-identifier", NULL, CFGF_NONE),
	CFG_END()
};

static cfg_opt_t opt_pba[] = {
	CFG_FUNC("include-dir", pba_cfg_include),
	CFG_SEC("splash", opt_pba_splash, CFGF_NONE),
	CFG_SEC("user", opt_pba_user, CFGF_NONE),
	CFG_SEC("sso", opt_pba_sso, CFGF_NONE),
	CFG_SEC("otp", opt_pba_otp, CFGF_NONE),
	CFG_END()
};


int pba_cfg_include(cfg_t* cfg, cfg_opt_t* opt, int argc, const char** argv) {
	char		buffer[TT_CONFIG_MAX+1] = { 0 };
	size_t		buffer_size = sizeof(buffer);
	size_t		buffer_pos = 0;
	char		listing[FILENAME_MAX+1] = { 0 };
	size_t		listing_size = sizeof(listing);
	char*		filename = NULL;
	char*		eol = NULL;

	if( cfg == NULL || opt == NULL || argc <= 0 || argv == NULL ) {
		return 1;
	}
	if( g_library.api.storage.file_load( TT_FILE__CONFIG_PBA, argv[0], listing, &listing_size ) != TT_OK ) {
		return 1;
	}
	filename = listing;
	do {
		eol = strchr( filename, '\n' );
		if( eol != NULL ) {
			*eol = '\0';
			eol++;
		}
		buffer_pos = strnlen( buffer, sizeof(buffer) );
		buffer_size = sizeof(buffer) - buffer_pos;
		if( g_library.api.storage.file_load( TT_FILE__CONFIG_PBA, filename, buffer+buffer_pos, &buffer_size ) != TT_OK ) {
			return 1;
		}
		filename = eol;
	} while( filename != NULL && filename[0] != '\0' );
	if( cfg_parse_buf( cfg, buffer ) != CFG_SUCCESS ) {
		return 1;
	}
	return CFG_SUCCESS;
}


int main (int argc, char *argv[]) {
	char		bootdevice[FILENAME_MAX+1] = { 0 };
	char		fallback[FILENAME_MAX+1] = { 0 };
	char		buffer[TT_CONFIG_MAX+1] = { 0 };
	size_t		buffer_size = sizeof(buffer);
	char		key[TT_KEY_BITS_MAX/8] = { 0 };
	size_t		key_size = sizeof(key);
	char		configuration[FILENAME_MAX+1] = { 0 };
	char		username[TT_USERNAME_CHAR_MAX+1] = {0};
	size_t		username_size = sizeof(username);
	char		password[TT_PASSWORD_CHAR_MAX+1] = {0};
	size_t		password_size = sizeof(password);
	cfg_t*		cfg = NULL;
	int		fd, verbose = 0;
	char		c;

	opterr = 0;
	while((c = getopt_long(argc, argv, "f:b:d:c:u:p:vh", long_options, NULL)) != -1) {
		switch(c) {
 			case 'h':
				printf("Usage: pba <parameter>\n");
				printf("\n");
				printf("       -c|--config      CONFIGURATION_FILENAME\n");
				printf("       -b|--bootdevice  BOOT_DEVICE\n");
				printf("       -x|--fallback    FALLBACK_EXECUTABLE\n");
				printf("\n");
				printf("Parameter:\n");
				printf("       -u|--user        USERNAME\n");
				printf("       -p|--pass        PASSWORD\n");
				printf("       -s|--sso         [yes|no]\n");
				printf("\n");
  				exit(0);
 			case 'x':
				strncpy( fallback, optarg, sizeof(fallback)-1 );
				break;
 			case 'u':
				strncpy( username, optarg, sizeof(username)-1 );
				username_size = strnlen( username, sizeof(username) );
				break;
 			case 'p':
				strncpy( password, optarg, sizeof(password)-1 );
				password_size = strnlen( password, sizeof(password) );
				break;
 			case 'c':
				strncpy( configuration, optarg, sizeof(configuration)-1 );
				break;
 			case 'b':
				strncpy( bootdevice, optarg, sizeof(bootdevice)-1 );
				break;
 			case 'v':
				verbose++;
				break;
 			case 's':
				if( strncmp( "yes", optarg, 4 ) == 0) {
					g_conf_sso = TT_YES;
				}
				break;
			default:
				break;
  		}
	}
	if( verbose ) {
		if( verbose >= 3 ) {
			setenv( "TT_LOG_LEVEL", "debug", 0 );
		} else if( verbose == 2 ) {
			setenv( "TT_LOG_LEVEL", "warn", 0 );
		} else if( verbose == 1 ) {
			setenv( "TT_LOG_LEVEL", "error", 0 );
		}
		setenv( "TT_LOG_TARGET", "FILE", 0 );
		setenv( "TT_LOG_TARGET_FILE", "/proc/self/fd/2", 0 );
	}
	if( fallback[0] == '\0' ) {
		if( readlink( "/proc/self/exe", fallback, sizeof(fallback)-1 ) > 0 ) {
			strncat( fallback, ".fallback", sizeof(fallback)-strlen( fallback )-1 );
			if( access( fallback, X_OK ) < 0 ) {
				fallback[0] = '\0';
			}
		}
	}
	if( pba_initialize( &g_library, bootdevice, configuration ) != TT_OK ) {
		fprintf( stderr, "TokenTube[pba]: initialization failed\n");
		if( fallback[0] != '\0' ) {
			fprintf( stderr, "TokenTube[pba]: executing fallback ('%s')\n", fallback );
			argv[0] = fallback;
			execv( fallback, argv );
		}
		fprintf( stderr, "TokenTube[pba]: terminating\n");
		exit(-1);
	}
	if( g_library.api.storage.luks_load != NULL ) {
		if( g_library.api.storage.luks_load( key, &key_size ) == TT_OK ) {
			write( STDOUT_FILENO, key, key_size );
			memset( key, '\0', key_size );
			tt_finalize();
			exit(0);
		}
	}
	if( g_library.api.storage.file_load( TT_FILE__CONFIG_PBA, "/boot/tokentube/pba.conf", buffer, &buffer_size ) != TT_OK ) {
		g_library.api.runtime.log( TT_LOG__ERROR, "pba", "API:storage.file_load() failed for [boot]/tokentube/pba.conf" );
		if( fallback[0] != '\0' ) {
			fprintf( stderr, "TokenTube[pba]: executing fallback ('%s')\n", fallback );
			argv[0] = fallback;
			execv( fallback, argv );
		}
		fprintf( stderr, "TokenTube[pba]: terminating\n");
		exit(-1);

	}
	cfg = cfg_init( opt_pba, CFGF_NONE );
	if( cfg == NULL ) {
		g_library.api.runtime.log( TT_LOG__ERROR, "pba", "cfg_init() failed in %s()", __FUNCTION__ );
		tt_finalize();
		if( fallback[0] != '\0' ) {
			fprintf( stderr, "TokenTube[pba]: executing fallback ('%s')\n", fallback );
			argv[0] = fallback;
			execv( fallback, argv );
		}
		fprintf( stderr, "TokenTube[pba]: terminating\n");
		exit(-1);
	}
	if( cfg_parse_buf( cfg, buffer ) != 0 ) {
		g_library.api.runtime.log( TT_LOG__ERROR, "pba", "cfg_parse_buf() failed for [boot]/tokentube/pba.conf" );
		cfg_free( cfg );
		tt_finalize();
		if( fallback[0] != '\0' ) {
			fprintf( stderr, "TokenTube[pba]: executing fallback ('%s')\n", fallback );
			argv[0] = fallback;
			execv( fallback, argv );
		}
		fprintf( stderr, "TokenTube[pba]: terminating\n");
		exit(-1);
	}
	g_conf_user_userprompt = cfg_getstr( cfg, "user|prompt-username" );
	g_conf_user_passprompt = cfg_getstr( cfg, "user|prompt-password" );
	g_conf_otp_identifier  = cfg_getstr( cfg, "otp|default-identifier" );
	g_conf_sso_exec = cfg_getstr( cfg, "sso|executable" );
	g_conf_sso_socket = cfg_getstr( cfg, "sso|socket" );
	if( g_conf_sso_socket == NULL ) {
		g_conf_sso_socket = "/dev/.initramfs/tokentube/pba/sso.socket";
	}
	strncpy( buffer, g_conf_sso_socket, sizeof(buffer)-1 );
	if( access( dirname( buffer ), F_OK ) == -1 && errno == ENOENT ) {
		if( g_library.api.util.posix_mkdir( buffer ) < 0 ) {
			g_library.api.runtime.log( TT_LOG__ERROR, "pba", "API:util.posix_mkdir() failed for '%s'", configuration );
			cfg_free( cfg );
			tt_finalize();
			exit(-1);
		}
		chmod( buffer, S_IWUSR|S_IRUSR|S_IXUSR|S_IXGRP|S_IXOTH );
		if( username[0] == '\0' ) {
			if( cfg_getstr( cfg, "user|default-username" ) != NULL ) {
				strncpy( username, cfg_getstr( cfg, "user|default-username" ), sizeof(username)-1 );
				username_size = strnlen( username, sizeof(username) );
			}
		}
	}
	if( username[0] == '\0' || password[0] == '\0' ) {
		if( pba_plymouth( g_conf_user_userprompt, g_conf_user_passprompt, username, &username_size, password, &password_size ) != TT_OK ) {
			g_library.api.runtime.log( TT_LOG__ERROR, "pba", "pba_plymouth() returned TT_ERR" );
			tt_finalize();
			exit(-1);
		}
	}
	if( username[0] == '\0' && password[0] == '\0' ) {
		key_size = sizeof(key);
		if( pba_otp( &g_library, g_conf_otp_identifier, key, &key_size ) != TT_OK ) {
			g_library.api.runtime.log( TT_LOG__ERROR, "pba", "pba_otp() returned error in %s()", __FUNCTION__ );
			tt_finalize();
			exit(-1);
		}
		write( STDOUT_FILENO, key, key_size );
	}
	if( username[0] == '\0' && password[0] != '\0' ) {
		g_library.api.runtime.debug( TT_DEBUG__VERBOSITY1, "pba", "no username given, returning password in %s()", __FUNCTION__ );
		write( STDOUT_FILENO, password, password_size );
	}
	if( username[0] != '\0' && password[0] != '\0' ) {
		key_size = sizeof(key);
		if( pba_user_loadkey( &g_library, username, username_size, password, password_size, key, &key_size ) != TT_OK ) {
			g_library.api.runtime.log( TT_LOG__ERROR, "pba", "pba_user_loadkey() failed in %s()", __FUNCTION__ );
			memset( password, '\0', password_size );
			tt_finalize();
			exit(-1);
		}
		if( g_conf_sso == TT_YES ) {
			buffer_size = 0;
			buffer_size += snprintf( buffer+buffer_size, sizeof(buffer)-buffer_size, "ssod {\n" );
			buffer_size += snprintf( buffer+buffer_size, sizeof(buffer)-buffer_size, "  executable = '%s'\n", g_conf_sso_exec );
			buffer_size += snprintf( buffer+buffer_size, sizeof(buffer)-buffer_size, "  socket = '%s'\n", g_conf_sso_socket );
			buffer_size += snprintf( buffer+buffer_size, sizeof(buffer)-buffer_size, "}\n" );
			memset( configuration, '\0', sizeof(configuration) );
			strncpy( configuration, "/tmp/sso.conf-XXXXXX", sizeof(configuration)-1 );
			if( configuration[0] != '\0' ) {
				umask( 077 );
				fd = mkstemp( configuration );
				if( fd >= 0 ) {
					if( write( fd, buffer, buffer_size ) != (ssize_t)buffer_size ) {
						g_library.api.runtime.log( TT_LOG__ERROR, "pba", "write() failed in %s()", __FUNCTION__ );
						unlink( configuration );
					}
					close( fd );
					if( pba_sso_start( g_conf_sso_exec, configuration, g_conf_sso_socket ) == TT_OK ) {
						pba_sso_credentials( g_conf_sso_socket, username, password );
					} else {
						g_library.api.runtime.log( TT_LOG__ERROR, "pba", "failed to start sso-daemon" );
					}
					unlink( configuration );
				}
			}
		}
		write( STDOUT_FILENO, key, key_size );
	}
	memset( key, '\0', sizeof(key) );
	memset( password, '\0', sizeof(password) );
	cfg_free( cfg );
	tt_finalize();
	return 0;
}

