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
#include <syslog.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
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
        { "dry-run",     no_argument, 0, 'd' },
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
	CFG_STR("default_username", "", CFGF_NONE),
	CFG_END()
};


static cfg_opt_t opt_sso[] = {
	CFG_BOOL("sso", cfg_false, CFGF_NONE),
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
	int	verbose = 0;
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
				username = strndup(optarg, TT_USERNAME_CHAR_MAX+1);
				username_size = strnlen(username, TT_USERNAME_CHAR_MAX);
                        break;
 			case 'p':
				password = strndup(optarg, TT_PASSWORD_CHAR_MAX+1);
				password_size = strnlen(password, TT_PASSWORD_CHAR_MAX);
                        break;
 			case 'c':
				configuration = strndup( optarg, FILENAME_MAX+1 );
                        break;
 			case 'b':
				strncpy( bootdevice, optarg, sizeof(bootdevice)-1 );
                        break;
 			case 'd':
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
			username_size = strlen(username);
		}
	}
	cfg_free( cfg );

	if( username == NULL || password == NULL ) {
		if( pba_plymouth(conf_userprompt, conf_passprompt, &username, &username_size, &password, &password_size) != TT_OK ) {
			g_library.api.runtime.log( TT_LOG__ERROR, "pba", "pba_plymouth() returned TT_ERR" );
			tt_finalize();
			exit(-1);
		}
	}

	if( ( username == NULL || username[0] == '\0' ) && password != NULL ) {
		g_library.api.runtime.debug( TT_DEBUG__VERBOSITY1, "pba", "no username given, returning password in %s()", __FUNCTION__ );
		write( STDOUT_FILENO, password, password_size );
	}

	if( username == NULL && password == NULL ) {
		if( pba_otp( &g_library, username, key, &key_size ) != TT_OK ) {
			g_library.api.runtime.log( TT_LOG__ERROR, "pba", "pba_otp() returned error in %s()", __FUNCTION__ );
			tt_finalize();
			exit(-1);
		}
		write( STDOUT_FILENO, key, key_size );
	}

	if( username != NULL && password != NULL ) {
		if( pba_user_loadkey( &g_library, username, username_size, password, password_size, key, &key_size ) != TT_OK ) {
			g_library.api.runtime.log( TT_LOG__ERROR, "pba", "pba_user_loadkey() failed in %s()", __FUNCTION__ );
			memset( password, '\0', password_size );
			free( password );
			free( username );
			tt_finalize();
			exit(-1);
		}
		if( g_library.api.storage.file_load( TT_FILE__CONFIG_PBA, "[boot]/tokentube/pba/sso.conf", buffer, &buffer_size ) == TT_OK ) {
			cfg = cfg_init( opt_sso, CFGF_NONE );
			if( cfg != NULL ) {
				if( cfg_parse_buf( cfg, buffer ) == 0 ) {
					if( conf_ssod == TT_YES ) {
						if( cfg_getbool( cfg, "sso" ) > 0 ) {
							if( pba_ssod_start() == TT_OK ) {
								pba_ssod_credentials( username, password );
							}
						}
					}
				}
			}
			cfg_free( cfg );
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

