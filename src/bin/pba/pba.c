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

static cfg_opt_t opt_pba_sso[] = {
	CFG_BOOL("sso", cfg_false, CFGF_NONE),
	CFG_END()
};


int main (int argc, char *argv[]) {
	char*  configuration = NULL;
	char   bootdevice[1024] = { 0 };
	char   cfg_pba[4096] = { 0 };
	size_t cfg_pba_size = sizeof(cfg_pba);
	cfg_t* cfg = NULL;
	char*  username = NULL;
	size_t username_len = 0;
	char*  password = NULL;
	size_t password_len = 0;
	char   key[TT_KEY_BITS_MAX/8] = { 0 };
	size_t key_len = sizeof(key);
	int result = -1;
	int verbose = 0;
	char	c;

        opterr = 0;
        while((c = getopt_long(argc, argv, "b:d:c:u:p:v", long_options, NULL)) != -1) {
                switch(c) {
 			case 'u':
				username = strndup(optarg, TT_USERNAME_CHAR_MAX+1);
				username_len = strnlen(username, TT_USERNAME_CHAR_MAX);
                        break;
 			case 'p':
				password = strndup(optarg, TT_PASSWORD_CHAR_MAX+1);
				password_len = strnlen(password, TT_PASSWORD_CHAR_MAX);
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
		exit(-1);
	}

	if( g_library.api.storage.luks_load != NULL ) {
		if( g_library.api.storage.luks_load( key, &key_len ) == TT_OK ) {
			write( STDOUT_FILENO, key, key_len );
			memset( key, '\0', key_len );
			tt_finalize();
			exit(0);
		}
	}

	if( g_library.api.storage.file_load( TT_FILE__CONFIG_PBA, "/boot/tokentube/pba.conf", cfg_pba, &cfg_pba_size ) == TT_OK ) {
		cfg = cfg_init( opt_pba, CFGF_NONE );
		if( cfg_parse_buf( cfg, cfg_pba ) == 0 ) {
			conf_userprompt = cfg_getstr( cfg, "prompt_username" );
			conf_passprompt = cfg_getstr( cfg, "prompt_password" );

			if( access( TT_FILENAME__SSOD_INITRAMFS_DIR "/" TT_FILENAME__SSOD_TOKENTUBE_DIR , F_OK ) == -1 && errno == ENOENT ) {
				mkdir( TT_FILENAME__SSOD_INITRAMFS_DIR "/" TT_FILENAME__SSOD_TOKENTUBE_DIR, S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH );
				if( username == NULL ) {
					username = cfg_getstr( cfg, "default_username" );
					username_len = strlen(username);
				}
			}
		} else {
			g_library.api.runtime.log( TT_LOG__ERROR, "pba", "cfg_parse_buf() failed for [boot]/tokentube/pba.conf" );
			exit(-1);
		}
	} else {
		g_library.api.runtime.log( TT_LOG__ERROR, "pba", "g_library.api.storage.file_load() failed for [boot]/tokentube/pba.conf" );
		exit(-1);
	}
	if( username == NULL || password == NULL ) {
		if( pba_plymouth(conf_userprompt, conf_passprompt, &username, &username_len, &password, &password_len) != TT_OK ) {
			g_library.api.runtime.log( TT_LOG__ERROR, "pba", "pba_plymouth() returned TT_ERR" );
			exit(-1);
		}
	}
	if( username != NULL && password != NULL ) {
		username[username_len] = '\0';
		password[password_len] = '\0';
		if( username_len > 0 ) {
			if( password_len > 0 ) {
				if( pba_user_loadkey( &g_library, username, username_len, password, password_len, key, &key_len ) == TT_OK ) {
					char   cfg_pba_sso[4096] = { 0 };
					size_t cfg_pba_sso_size = sizeof(cfg_pba_sso);
					cfg_t* cfg = NULL;

					if( g_library.api.storage.file_load( TT_FILE__CONFIG_PBA, "/boot/tokentube/pba/sso.conf", cfg_pba_sso, &cfg_pba_sso_size ) == TT_OK ) {
						cfg = cfg_init( opt_pba_sso, CFGF_NONE );
						if( cfg_parse_buf(cfg, cfg_pba_sso) == 0 ) {
							if( conf_ssod == TT_YES && cfg_getbool( cfg, "sso" ) != 0 ) {
								if( pba_ssod_start() == TT_OK ) {
									pba_ssod_credentials( username, password );
								} else {
									g_library.api.runtime.log( TT_LOG__ERROR, "pba", "pba_ssod_start() returned TT_ERR" );
								}
							}
						} else {
							g_library.api.runtime.log( TT_LOG__ERROR, "pba", "cfg_parse_buf() failed for tokentube/pba/sso.conf" );
						}
					}
					write( STDOUT_FILENO, key, key_len );
					memset( key, '\0', key_len );
				} else {
					write( STDOUT_FILENO, password, password_len );
				}
				result = 0;
			} else {
				if( pba_otp( &g_library, "helpdesk", key, &key_len ) == TT_OK ) {
					write( STDOUT_FILENO, key, key_len );
				} else {
					g_library.api.runtime.log( TT_LOG__ERROR, "pba", "pba_otp() returned TT_ERR" );
				}
				result = 0;
			}
		} else {
			write( STDOUT_FILENO, password, password_len );
			result = 0;
		}
		memset( username, '\0', username_len );
		memset( password, '\0', password_len );
		free( username );
		free( password );
	}
	tt_finalize();
	return result;
}

