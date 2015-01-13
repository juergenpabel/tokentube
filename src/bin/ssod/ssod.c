#define _GNU_SOURCE 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <syslog.h>
#include <tokentube/defines.h>

#define LEN_USERNAME_MAX   32
#define LEN_PASSWORD_MAX  256
#define LEN_PASSWORD_BUF 4096

#define PEER_UNKNOWN 0
#define PEER_PBA     1
#define PEER_GREETER 2

static char username[LEN_USERNAME_MAX] = {0};
static char* password = NULL;


static void ssod_punt(int code) {
	memset( username, '\0', sizeof(username) );
	if( password != NULL ) {
		memset( password, '\0', LEN_PASSWORD_BUF );
		free( password );
	}
	unlink( TT_FILENAME__SSOD_SOCKET );
	syslog( LOG_CRIT, "ssod:exiting with code=%d", code );
	exit(code);
}


static int ssod_identifypeer(int sock) {
	struct ucred usercred;
	socklen_t uclen = sizeof(usercred);

	if( getsockopt(sock, SOL_SOCKET, SO_PEERCRED, &usercred, &uclen) < 0 ) {
		return TT_ERR;
	}
	syslog( LOG_DEBUG, "ssod: connection accepted by PID=%d", usercred.pid );
	if( usercred.uid == 0 ) {
		return PEER_PBA;
	}
	if( usercred.uid > 0 && usercred.uid < 1000 ) {
		return PEER_GREETER;
	}
	return PEER_UNKNOWN;
}


int main(int argc __attribute__((unused)), char* argv[] __attribute__((unused))) {
	int i, result = 0;
	int password_offset = 0;
	int password_length = 0;
	int sock_local, sock_remote;
	struct sockaddr_un sa_local;

	setsid();
	umask(022);
	if( chdir(TT_FILENAME__SSOD_INITRAMFS_DIR) < 0 ) {
		ssod_punt(-1);
	}
	if( chroot(TT_FILENAME__SSOD_INITRAMFS_DIR) < 0 ) {
		ssod_punt(-2);
	}
	if( chdir(TT_FILENAME__SSOD_TOKENTUBE_DIR) < 0 ) {
		ssod_punt(-3);
	}

	close(2);
	close(1);
	close(0);
	if( open( "/dev/null", O_RDWR ) < 0 ) {
		ssod_punt(-4);
	}
	if( open( "/dev/null", O_RDWR ) < 0 ) {
		ssod_punt(-4);
	}
	if( open( "/dev/null", O_RDWR ) < 0 ) {
		ssod_punt(-4);
	}

	mlockall( MCL_CURRENT|MCL_FUTURE );

	srand( time(NULL) );
	password = malloc( LEN_PASSWORD_BUF );
	if(password == NULL) {
		ssod_punt(-4);
	}
	password_offset = rand() % (LEN_PASSWORD_BUF-LEN_PASSWORD_MAX);

	if( access(TT_FILENAME__SSOD_SOCKET, F_OK) == 0 ) {
		ssod_punt(-99);
	}
	sock_local = socket( AF_UNIX, SOCK_STREAM, 0 );
	if( sock_local < 0 ) {
		ssod_punt(-1);
	}
	sa_local.sun_family = AF_UNIX;
	strncpy( sa_local.sun_path, TT_FILENAME__SSOD_SOCKET, sizeof(sa_local.sun_path)-1 );
	if( bind( sock_local, (struct sockaddr*)&sa_local, sizeof(sa_local.sun_family)+strlen(sa_local.sun_path) ) < 0 ) {
		ssod_punt(-2);
	}
	if( chmod( TT_FILENAME__SSOD_SOCKET, S_IWUSR|S_IRUSR|S_IWGRP|S_IRGRP|S_IWOTH|S_IROTH ) < 0 ) {
		ssod_punt(-1);
	}
	if( listen(sock_local, 1) < 0 ) {
		ssod_punt(-5);
	}

	syslog(LOG_DEBUG, "ssod: entering connection handler loop");
	for(;;) {
		sock_remote = accept( sock_local, NULL, NULL );
		if( sock_remote >= 0 ) {
			switch( ssod_identifypeer(sock_remote) ) {
				case PEER_PBA:
					syslog( LOG_INFO, "ssod: accepted connection from PBA executable" );
					memset( username, '\0', LEN_USERNAME_MAX );
					for(i=0; i<LEN_USERNAME_MAX-1; i++) {
						if( read( sock_remote, &username[i], 1 ) == 1 ) {
							if(username[i] == '\n') {
								username[i] = '\0';
								break;
							}
						} else {
							break;
						}
					}
					if( i == 0 ) {
						syslog( LOG_INFO, "ssod: terminating due to request from PBA executable" );
						ssod_punt(0);
					}
					for(i=0; i<LEN_PASSWORD_BUF; i++) {
						password[i] = 32 + (rand() % 96);
					}
					for(password_length=0; password_length<LEN_PASSWORD_MAX; password_length++) {
						if( read(sock_remote, &password[password_offset+password_length], 1) == 1) {
							if( password[password_offset+password_length] == '\n' ) {
								break;
							}
						}
					}
					password[password_offset+password_length] = 32 + (rand() % 96);
				break;
				case PEER_GREETER:
					syslog( LOG_INFO, "ssod: accepted connection from GREETER executable");
					unlink( TT_FILENAME__SSOD_SOCKET );
					result |= write( sock_remote, username, strlen(username) );
					result |= write( sock_remote, "\n", 1 );
					result |= write( sock_remote, &password[password_offset], password_length );
					result |= write( sock_remote, "\n", 1 );
					close( sock_remote );
					close( sock_local );
					syslog( LOG_INFO, "ssod: sent user credentials to greeter" );
					ssod_punt( result );
				break;
				case PEER_UNKNOWN:
					syslog( LOG_CRIT, "ssod: rejected unknown connection, terminating" );
					unlink( TT_FILENAME__SSOD_SOCKET );
					close( sock_remote );
					close( sock_local );
					ssod_punt(-6);
				break;
				default:
					syslog( LOG_CRIT, "ssod: error while accepting connection, skipping" );
			}
			close( sock_remote );
		} else {
			syslog( LOG_INFO, "ssod: accept() failed" );
			unlink( TT_FILENAME__SSOD_SOCKET );
		}
	}
	return TT_ERR;
}

