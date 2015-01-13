#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <gcrypt.h>
#include <tokentube.h>

#define OPT_USER_CREATE  20
#define OPT_USER_UPDATE  21
#define OPT_USER_VERIFY  22
#define OPT_USER_DELETE  23
#define OPT_OTP_CREATE 24
#define OPT_OTP_CHALLENGE 25
#define OPT_OTP_RESPONSE 26
#define OPT_OTP_APPLY  27
#define OPT_OTP_DELETE  28
#define OPT_PBA_INSTALL  29


static const struct option long_options[] = {
	{ "config",          required_argument, 0, 'c' },
	{ "pba-install",     no_argument, 0, OPT_PBA_INSTALL },
	/* pba install actions */
	{ "pba-type",     required_argument, 0, 'T' },
	{ "pba-path",     required_argument, 0, 'P' },
	/* user management actions */
	{ "user-create",     no_argument, 0, OPT_USER_CREATE },
	{ "user-update",     no_argument, 0, OPT_USER_UPDATE },
	{ "user-verify",     no_argument, 0, OPT_USER_VERIFY },
	{ "user-delete",     no_argument, 0, OPT_USER_DELETE },
	/* one-time-password management actions */
	{ "otp-create",      no_argument, 0, OPT_OTP_CREATE },
	{ "otp-challenge",   no_argument, 0, OPT_OTP_CHALLENGE },
	{ "otp-response",    no_argument, 0, OPT_OTP_RESPONSE },
	{ "otp-apply",       no_argument, 0, OPT_OTP_APPLY },
	{ "otp-delete",      no_argument, 0, OPT_OTP_DELETE },
	/* action-specifc parameters */
	{ "username",        required_argument, 0, 'u' },
	{ "password",        required_argument, 0, 'p' },
	{ "new-password",    required_argument, 0, 'n' },
	{ "identifier",      required_argument, 0, 'i' },
	{ "challenge",       required_argument, 0, 'x' },
	{ "response",        required_argument, 0, 'y' },
	/* general parameters */
	{ "boot-device",     no_argument,       0, 'b' },
	{ "verbose",         no_argument,       0, 'v' },
	{ "debug",           no_argument,       0, 'd' },
	{ "quiet",           no_argument,       0, 'q' },
	{ "help",            no_argument,       0, 'h' },
	{ 0, 0, 0, 0 }
};



int main(int argc, char* argv[]) {
	tt_library_t	library;
	char* filename_bootdevice = NULL;
	char* filename_config = NULL;
	char* type_install = NULL;
	char* path_install = NULL;
	char* identifier = NULL;
	char key[TT_KEY_BITS_MAX/8] = {0};
	size_t key_len = 0;
	char cur_user_password[TT_PASSWORD_CHAR_MAX+1] = {0};
	char new_user_password[TT_PASSWORD_CHAR_MAX+1] = {0};
	char challenge[TT_OTP_TEXT_MAX+1] = {0};
	char response[TT_OTP_TEXT_MAX+1] = {0};
	char username[TT_USERNAME_CHAR_MAX+1] = {0};
	size_t cr_len = 0;
	int  action = 0;
	unsigned int quiet = 0;
	unsigned int verbose = 0;
	unsigned int debug = 0;
	char debuglevel[] = "0";
	int  i=0, c=0, err=0;
	tt_status_t status = TT_STATUS__UNDEFINED;

	opterr = 0;
	while((c = getopt_long(argc, argv, "T:P:c:u:p:n:i:x:y:b:qdvh", long_options, NULL)) != -1) {
		switch(c) {
			case 'h':
				printf("Usage: tokentube <action> <parameter>\n");
				printf("\n");
				printf("       -c|--config      CONFIGURATION_FILENAME\n");
				printf("\n");
				printf("System actions:\n");
				printf("       --pba-install    <-T> [initrd|initramfs]\n");
				printf("                        <-P> INSTALL_PATH\n");
				printf("\n");
				printf("User actions:\n");
				printf("       --user-create      <-u|--username> USERNAME\n");
				printf("                         [<-p|--password> USER_PASSWORD]\n");
				printf("       --user-update      <-u|--username> USERNAME\n");
				printf("                         [<-p|--password> USER_PASSWORD]\n");
				printf("                         [<-n|--new-password> NEW_PASSWORD]\n");
				printf("       --user-verify      <-u|--username> USERNAME\n");
				printf("                         [<-p|--password> USER_PASSWORD]\n");
				printf("       --user-delete      <-u|--username> USERNAME\n");
				printf("\n");
				printf("One-time-password actions:\n");
				printf("       --otp-create       <-i|--identifier> OTP_NAME\n");
				printf("       --otp-challenge    <-i|--identifier> OTP_NAME\n");
				printf("       --otp-response     <-i|--identifier> OTP_NAME\n");
				printf("                         [<-x|--challenge> CHALLENGE_STRING\n");
				printf("       --otp-apply        <-i|--identifier> OTP_NAME\n");
				printf("                         [<-x|--challenge> CHALLENGE_STRING\n");
				printf("                         [<-y|--response> RESPONSE_STRING\n");
				printf("       --otp-delete       <-i|--identifier> OTP_NAME\n");
				printf("\n");
				printf("Additional parameters:\n");
				printf("       -h|--help         Show help\n");
				printf("       -v|--verbose      Be verbose (can be used multiple times), showing log messages on stderr\n");
				printf("       -d|--debug        Be debuggy (can be used multiple times), showing debug messages on stderr\n");
				printf("       -q|--quiet        Be quiet, only errors are send to stderr (status in return code)\n");
				exit(0);
			break;
			case 'T':
				type_install = strndup( optarg, 64 );
			break;
			case 'P':
				path_install = strndup( optarg, FILENAME_MAX+1 );
			break;
			case 'c':
				filename_config = strndup( optarg, FILENAME_MAX+1 );
			break;
			case 'b':
				filename_bootdevice = strndup( optarg, FILENAME_MAX+1 );
			break;
			case 'v':
				verbose++;
			break;
			case 'd':
				debug++;
			break;
			case 'q':
				quiet = 1;
			break;
			case 'u':
				strncpy( username, optarg, sizeof(username)-1 );
			break;
			case 'p':
				strncpy( cur_user_password, optarg, sizeof(cur_user_password)-1 );
			break;
			case 'n':
				strncpy( new_user_password, optarg, sizeof(new_user_password)-1 );
			break;
			case 'i':
				identifier = optarg;
			break;
			case 'x':
				strncpy( challenge, optarg, sizeof(challenge)-1 );
			break;
			case 'y':
				strncpy( response, optarg, sizeof(response)-1 );
			break;
			default:
				if( action == 0 ) {
					action = c;
				} else {
					fprintf( stderr, "ERROR: multiple actions not allowed (%d, %d), see help details\n", action, c );
					action = -1;
					err = -1;
				}
		}
	}
	if( filename_bootdevice != NULL ) {
		setenv( "TT_RUNTIME_BOOTDEVICE", filename_bootdevice, 1 );
	}
	if( verbose ) {
		if( verbose >= 3 ) {
			setenv( "TT_LOG_LEVEL", "info", 0 );
		} else if( verbose == 2 ) {
			setenv( "TT_LOG_LEVEL", "warn", 0 );
		} else if( verbose == 1 ) {
			setenv( "TT_LOG_LEVEL", "error", 0 );
		}

		setenv( "TT_LOG_TARGET", "FILE", 0 );
		setenv( "TT_LOG_TARGET_FILE", "/proc/self/fd/2", 0 );
	}
	if( debug ) {
		if( debug > 9 ) {
			debug = 9;
		}
		debuglevel[0] += debug;
		setenv( "TT_DEBUG_LEVEL", debuglevel, 1 );
	}
	if( tt_initialize( TT_VERSION ) != TT_OK ) {
		fprintf(stderr, "Exiting...\n");
		exit( -1 );
	}
	if( tt_configure( filename_config ) != TT_OK ) {
		fprintf(stderr, "Exiting...\n");
		exit( -1 );
	}
	if( tt_discover( &library ) != TT_OK ) {
		fprintf(stderr, "Exiting...\n");
		exit( -1 );
	}
	if( path_install != NULL && type_install != NULL ) {
		if( library.api.runtime.install_pba( type_install, path_install ) != TT_OK ) {
			fprintf(stderr, "Exiting...\n");
			exit( -1 );
		}
		tt_finalize();
		exit( 0 );
	}
	mlockall( MCL_CURRENT|MCL_FUTURE );
	gcry_control( GCRYCTL_INIT_SECMEM, 16384, 0 );
	gcry_control( GCRYCTL_INITIALIZATION_FINISHED, 0 );
	switch( action ) {
		case -1:
			/* fallthrough dummy */
		break;
		case 0:
			fprintf( stderr, "ERROR: no action specified, call with '--help' for usage information\n" );
		break;
		case OPT_USER_CREATE:
			if( username[0] == '\0' ) {
				fprintf( stderr, "ERROR: username required for \"user-create\"\n" );
				err = -1;
				break;
			}
			if( library.api.user.exists(username, &status) == TT_OK && status == TT_YES ) {
				fprintf(stderr, "FAILURE: user '%s' already exists\n", username);
				err = -1;
				break;
			}
			while( cur_user_password[0] == '\0' ) {
				strncpy(cur_user_password, getpass("Please provide a password for the to-be-created user: "), sizeof(cur_user_password)-1);
			}
			if( library.api.user.create( username, cur_user_password ) == TT_OK ) {
				if( !quiet ) {
					printf( "SUCCESS: user '%s' has been created\n", username );
				}
			} else {
				err = -1;
				fprintf( stderr, "FAILURE: user '%s' could not be created (failed to create the user's file)\n", username );
			}
		break;
		case OPT_USER_UPDATE:
			if( username[0] == '\0' ) {
				fprintf(stderr, "ERROR: username required for \"change-password\"\n");
				err = -1;
				break;
			}
			while( cur_user_password[0] == '\0' ) {
				strncpy( cur_user_password, getpass("Please provide the user's current password: "), sizeof(cur_user_password)-1 );
			}
			if( library.api.user.execute_verify(username, cur_user_password, &status) == TT_OK && status == TT_NO ) {
				if(!quiet) {
					fprintf(stderr, "ERROR: password not correct for user '%s'\n", username);
				}
				err = 1;
				break;
			}
			while( new_user_password[0] == '\0' ) {
				strncpy( new_user_password, getpass("New Password: "), sizeof(new_user_password)-1 );
			}
			if( library.api.user.update(username, cur_user_password, new_user_password, &status) == TT_OK ) {
				if( !quiet ) {
					if( status == TT_YES ) {
						printf( "SUCCESS: password changed for user '%s'\n", username );
					} else {
						printf( "FAILRE: password incorrect for user '%s'\n", username );
					}
				}
			} else {
				err = 1;
				fprintf( stderr, "ERROR: password not changed for user '%s'\n", username );
			}
		break;
		case OPT_USER_VERIFY:
			if( username[0] == '\0' ) {
				fprintf( stderr, "ERROR: username required for \"user-verify\"\n" );
				err = -1;
				break;
			}
			while( cur_user_password[0] == '\0' ) {
				strncpy( cur_user_password, getpass("Please provide the user's current password: "), sizeof(cur_user_password)-1 );
			}
			if( library.api.user.execute_verify( username, cur_user_password, &status ) == TT_OK ) {
				if( status == TT_YES ) {
					if( !quiet ) {
						printf( "SUCCESS: password verified for user '%s'\n", username );
					}
				} else {
					err = -1;
					fprintf( stderr, "FAILURE: password not verified for user '%s'\n", username );
				}
			} else {
				err = 1;
				fprintf( stderr, "ERROR: password not verified for user '%s'\n", username );
			}
		break;
		case OPT_USER_DELETE:
			if( username[0] == '\0' ) {
				fprintf( stderr, "ERROR: username required for \"user-delete\"\n" );
				err = -1;
				break;
			}
			if( library.api.user.exists(username, &status) == TT_OK && status == TT_YES ) {
				if( library.api.user.delete( username, &status ) == TT_OK ) {
					if( !quiet ) {
						printf( "SUCCESS: user '%s' has been deleted (removed from TokenTube database)\n", username );
					}
				} else {
					err = -1;
					fprintf( stderr, "FAILURE: user '%s' could not be deleted (not removed from TokenTube database)\n", username );
				}
			} else {
				if( status == TT_NO ) {
					if( !quiet ) {
						printf( "WARNING: user '%s' not found in database\n", username );
					}
				} else {
					err = -1;
					fprintf( stderr, "ERROR: tt_user_exists returned an error (probably a bug in tokentube)\n" );
				}
			}
		break;
		case OPT_OTP_CREATE:
			if( identifier == NULL ) {
				fprintf( stderr, "ERROR: identifier required for \"otp-create\"\n" );
				err = -1;
				break;
			}
			if( library.api.otp.create( identifier ) == TT_OK ) {
				if( !quiet ) {
					printf( "SUCCESS: otp '%s' has been created\n", identifier );
				}
			} else {
				fprintf( stderr, "ERROR: tt_otp_create() failed\n" );
				err = -1;
				break;
			}
		break;
		case OPT_OTP_CHALLENGE:
			if( identifier == NULL ) {
				fprintf( stderr, "ERROR: identifier required for \"otp-challenge\"\n" );
				err = -1;
				break;
			}
			cr_len = sizeof( challenge );
			if( library.api.otp.execute_challenge( identifier, challenge, &cr_len ) == TT_OK ) {
				printf( "SUCCESS: challenge='%s'\n", challenge );
			} else {
				err = -1;
				fprintf( stderr, "FAILURE: tt_otp_execute_challenge() failed\n" );
			}
		break;
		case OPT_OTP_RESPONSE:
			if( identifier == NULL ) {
				fprintf( stderr, "ERROR: identifier required for \"otp-challenge\"\n" );
				err = -1;
				break;
			}
			if( challenge[0] == '\0' ) {
				fprintf( stderr, "ERROR: challenge required for \"otp-response\"\n" );
				err = -1;
				break;
			}
			cr_len = sizeof( response );
			if( library.api.otp.execute_response( identifier, challenge, response, &cr_len ) == TT_OK ) {
				printf( "SUCCESS: response='%s'\n", response );
			} else {
				err = -1;
				fprintf( stderr, "FAILURE: tt_otp_execute_response() failed\n" );
			}
		break;
		case OPT_OTP_APPLY:
			if( identifier == NULL ) {
				fprintf( stderr, "ERROR: identifier required for \"otp-apply\"\n" );
				err = -1;
				break;
			}
			if( challenge[0] == '\0' ) {
				strncpy( challenge, getpass("Please provide the challenge: "), sizeof(challenge)-1 );
			}
			if( response[0] == '\0' ) {
				strncpy( response, getpass("Please provide the response: "), sizeof(response)-1 );
			}
			key_len = sizeof(key);
			if( library.api.otp.execute_apply( identifier, challenge, response, key, &key_len ) == TT_OK ) {
				for( i=0; i<(int)key_len; i++ ) {
					printf( "%hhX ", key[i] );
				}
				printf( "\n" );

			} else {
				fprintf( stderr, "ERROR: tt_otp_execute_apply() failed\n" );
			}

		break;
		case OPT_OTP_DELETE:
			if( identifier == NULL ) {
				fprintf( stderr, "ERROR: identifier required for \"otp-delete\"\n" );
				err = -1;
				break;
			}
			if( library.api.otp.delete( identifier, &status ) == TT_OK ) {
				if( !quiet ) {
					printf( "SUCCESS: otp '%s' has been deleted\n", identifier );
				}
			} else {
				fprintf( stderr, "ERROR: tt_otp_delete() failed\n" );
				err = -1;
				break;
			}
		break;
		default:
			printf( "ERROR: unknown action invoked (probably a bug in tokentube)\n" );
			err = -1;
	}
	memset( key, '\0', sizeof(key) );
	memset( cur_user_password, '\0', sizeof(cur_user_password) );
	memset( new_user_password, '\0', sizeof(new_user_password) );
	free( type_install );
	free( path_install );
	free( filename_config );
	tt_finalize();
	return err;
}

