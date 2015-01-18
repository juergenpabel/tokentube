#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <confuse.h>
#include <tokentube.h>
#include <tokentube/plugin.h>
#include "plugin.h"
#include "pba.h"


static tt_library_t	g_library;
static char* 		conf_userprompt = "Username:";
static char*		conf_passprompt = "Password:";
static int   		conf_ssod = TT_YES;


static const struct option long_options[] = {
        { "boot-device", required_argument, 0, 'b' },
        { "config",      required_argument, 0, 'c' },
        { "ssod",        required_argument, 0, 's' },
        { "username",    required_argument, 0, 'u' },
        { "password",    required_argument, 0, 'p' },
        { "verbose",     no_argument,       0, 'v' },
        { 0, 0, 0, 0 }
};


static cfg_opt_t opt_pba[] = {
	CFG_STR("prompt_username", "Username:", CFGF_NONE),
	CFG_STR("prompt_password", "Password:", CFGF_NONE),
	CFG_STR("default_username", NULL, CFGF_NONE),
	CFG_END()
};

static cfg_opt_t opt_sso_ssod[] = {
	CFG_STR("executable", SSOD_EXECUTABLE, CFGF_NONE),
	CFG_STR("socket", SSOD_SOCKET, CFGF_NONE),
	CFG_END()
};

static cfg_opt_t opt_sso[] = {
	CFG_SEC("ssod", opt_sso_ssod, CFGF_NONE),
	CFG_END()
};


int main (int argc, char *argv[]) {
	char	bootdevice[FILENAME_MAX+1] = { 0 };
	char	buffer[TT_CONFIG_MAX+1] = { 0 };
	size_t	buffer_size = sizeof(buffer);
	char	key[TT_KEY_BITS_MAX/8] = { 0 };
	size_t	key_size = sizeof(key);
	char*	configuration = NULL;
	char*	username = NULL;
	size_t	username_size = 0;
	char*	password = NULL;
	size_t	password_size = 0;
	cfg_t*	cfg = NULL;
	int	fd, verbose = 0;
	char	c;

        opterr = 0;
        while((c = getopt_long(argc, argv, "b:d:c:u:p:vh", long_options, NULL)) != -1) {
                switch(c) {
 			case 'h':
				printf("Usage: pba <parameter>\n");
				printf("\n");
				printf("       -c|--config      CONFIGURATION_FILENAME\n");
				printf("       -b|--bootdevice  BOOT_DEVICE\n");
				printf("\n");
				printf("Parameter:\n");
				printf("       -u|--user        USERNAME\n");
				printf("       -p|--pass        PASSWORD\n");
				printf("       -s|--ssod        [yes|no]\n");
				printf("\n");
  				exit(0);
 			case 'u':
				username = strndup( optarg, TT_USERNAME_CHAR_MAX+1 );
				username_size = strnlen( username, TT_USERNAME_CHAR_MAX );
                        break;
 			case 'p':
				password = strndup( optarg, TT_PASSWORD_CHAR_MAX+1 );
				password_size = strnlen( password, TT_PASSWORD_CHAR_MAX );
                        break;
 			case 'c':
				configuration = strndup( optarg, FILENAME_MAX+1 );
                        break;
 			case 'b':
				strncpy( bootdevice, optarg, sizeof(bootdevice)-1 );
                        break;
 			case 'v':
				verbose++;
                        break;
 			case 's':
				if( strncmp( "no", optarg, 3) == 0) {
					conf_ssod = TT_NO;
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
 
	if( pba_initialize( &g_library, bootdevice, configuration ) != TT_OK ) {
		fprintf( stderr, "TokenTube[pba]: pba_initialize() failed in %s()", __FUNCTION__ );
		exit(-1);
	}
	if( configuration != NULL ) {
		free( configuration );
		configuration = NULL;
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
		exit(-1);

	}
	cfg = cfg_init( opt_pba, CFGF_NONE );
	if( cfg == NULL ) {
		g_library.api.runtime.log( TT_LOG__ERROR, "pba", "cfg_init() failed in %s()", __FUNCTION__ );
		tt_finalize();
		exit(-1);
	}
	if( cfg_parse_buf( cfg, buffer ) != 0 ) {
		g_library.api.runtime.log( TT_LOG__ERROR, "pba", "cfg_parse_buf() failed for [boot]/tokentube/pba.conf" );
		cfg_free( cfg );
		tt_finalize();
		exit(-1);
	}
	conf_userprompt = cfg_getstr( cfg, "prompt_username" );
	conf_passprompt = cfg_getstr( cfg, "prompt_password" );

	if( access( TT_FILENAME__SSOD_INITRAMFS_DIR "/" TT_FILENAME__SSOD_TOKENTUBE_DIR , F_OK ) == -1 && errno == ENOENT ) {
		mkdir( TT_FILENAME__SSOD_INITRAMFS_DIR "/" TT_FILENAME__SSOD_TOKENTUBE_DIR, S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH );
		if( username == NULL ) {
			username = cfg_getstr( cfg, "default_username" );
			if( username != NULL ) {
				username = strndup( username, TT_USERNAME_CHAR_MAX+1 );
				username_size = strnlen( username, TT_USERNAME_CHAR_MAX );
			}
		}
	}

	if( username == NULL || password == NULL ) {
		if( pba_plymouth(conf_userprompt, conf_passprompt, &username, &username_size, &password, &password_size) != TT_OK ) {
			g_library.api.runtime.log( TT_LOG__ERROR, "pba", "pba_plymouth() returned TT_ERR" );
			tt_finalize();
			exit(-1);
		}
	}

	if( username == NULL && password == NULL ) {
		if( pba_otp( &g_library, "helpdesk", key, &key_size ) != TT_OK ) {
			g_library.api.runtime.log( TT_LOG__ERROR, "pba", "pba_otp() returned error in %s()", __FUNCTION__ );
			tt_finalize();
			exit(-1);
		}
		write( STDOUT_FILENO, key, key_size );
	}

	if( ( username == NULL || username[0] == '\0' ) && password != NULL ) {
		g_library.api.runtime.debug( TT_DEBUG__VERBOSITY1, "pba", "no username given, returning password in %s()", __FUNCTION__ );
		write( STDOUT_FILENO, password, password_size );
	}

	if( ( username != NULL && username[0] != '\0' ) && password != NULL ) {
		if( pba_user_loadkey( &g_library, username, username_size, password, password_size, key, &key_size ) != TT_OK ) {
			g_library.api.runtime.log( TT_LOG__ERROR, "pba", "pba_user_loadkey() failed in %s()", __FUNCTION__ );
			memset( password, '\0', password_size );
			free( password );
			free( username );
			tt_finalize();
			exit(-1);
		}
		if( conf_ssod == TT_YES ) {
			buffer_size = sizeof(buffer);
			if( g_library.api.storage.file_load( TT_FILE__CONFIG_PBA, "/boot/tokentube/sso.conf", buffer, &buffer_size ) == TT_OK ) {
				cfg_free( cfg );
				cfg = cfg_init( opt_sso, CFGF_NONE );
				if( cfg == NULL ) {
					g_library.api.runtime.log( TT_LOG__ERROR, "pba", "cfg_init() failed in %s()", __FUNCTION__ );
				}
				if( cfg != NULL && cfg_parse_buf( cfg, buffer ) != 0 ) {
					g_library.api.runtime.log( TT_LOG__ERROR, "pba", "cfg_parse_buf() failed in %s()", __FUNCTION__ );
					cfg_free( cfg );
					cfg = NULL;
				}
				configuration = strdup( "/tmp/boot_tokentube_sso.conf.XXXXXX" );
				if( cfg != NULL && configuration != NULL ) {
					fd = mkstemp( configuration );
					if( fd >= 0 ) {
						if( write( fd, buffer, buffer_size ) != (ssize_t)buffer_size ) {
							g_library.api.runtime.log( TT_LOG__ERROR, "pba", "write() failed in %s()", __FUNCTION__ );
							unlink( configuration );
						}
						close( fd );
						if( pba_ssod_start( cfg_getstr( cfg, "ssod|executable" ), configuration, cfg_getstr( cfg, "ssod|socket" ) ) == TT_OK ) {
							pba_ssod_credentials( cfg_getstr( cfg, "ssod|socket" ), username, password );
						} else {
							g_library.api.runtime.log( TT_LOG__ERROR, "pba", "pba_ssod_start() failed in %s()", __FUNCTION__ );
						}
						unlink( configuration );
					} else {
						g_library.api.runtime.log( TT_LOG__ERROR, "pba", "mkstemp() failed in %s()", __FUNCTION__ );
					}
					free( configuration );
					configuration = NULL;
					cfg_free( cfg );
					cfg = NULL;
				}
			}
		}
		write( STDOUT_FILENO, key, key_size );
	}

	memset( key, '\0', sizeof(key) );
	if( username != NULL ) {
		memset( username, '\0', username_size );
		free( username );
	}
	if( password != NULL ) {
		memset( password, '\0', password_size );
		free( password );
	}
	tt_finalize();
	return 0;
}

