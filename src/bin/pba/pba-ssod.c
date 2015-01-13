#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <confuse.h>
#include <tokentube.h>
#include "pba.h"


int pba_ssod_start() {
	if( access( TT_FILENAME__SSOD_INITRAMFS_DIR "/" TT_FILENAME__SSOD_TOKENTUBE_DIR "/" TT_FILENAME__SSOD_SOCKET, F_OK ) == -1 ) {
		if( errno == ENOENT ) {
			switch( fork() ) {
				case -1:
					fprintf( stderr, "pba: fork() for tokentube's ssod failed\n" );
					return TT_ERR;
				case 0:
					execl( "/lib/tokentube/ssod", "ssod", NULL );
					exit(-1);
				default:
					sleep( 1 );
			}
		}
	}
	return TT_OK;
}


int pba_ssod_stop() {
	struct sockaddr_un	sa;
	int			sock;

	if( access( TT_FILENAME__SSOD_INITRAMFS_DIR "/" TT_FILENAME__SSOD_TOKENTUBE_DIR "/" TT_FILENAME__SSOD_SOCKET, F_OK ) < 0 ) {
		return TT_OK;
	}
	sa.sun_family = AF_UNIX;
	strncpy( sa.sun_path, TT_FILENAME__SSOD_INITRAMFS_DIR "/" TT_FILENAME__SSOD_TOKENTUBE_DIR "/" TT_FILENAME__SSOD_SOCKET, sizeof(sa.sun_path)-1 );
	sock = socket( AF_UNIX, SOCK_STREAM, 0 );
	if( sock < 0 ) {
		return TT_ERR;
	}
	if( connect(sock, (struct sockaddr*)&sa, sizeof(sa.sun_family)+strlen(sa.sun_path)) != 0 ) {
		close( sock );
		return TT_ERR;
	}
	close( sock );
	return TT_OK;
}


int pba_ssod_credentials(const char* username, const char* password) {
	struct sockaddr_un	sa;
	int			sock;

	sa.sun_family = AF_UNIX;
	strncpy( sa.sun_path, TT_FILENAME__SSOD_INITRAMFS_DIR "/" TT_FILENAME__SSOD_TOKENTUBE_DIR "/" TT_FILENAME__SSOD_SOCKET, sizeof(sa.sun_path)-1 );
	sock = socket( AF_UNIX, SOCK_STREAM, 0 );
	if( sock < 0 ) {
		return TT_ERR;
	}
	if( connect(sock, (struct sockaddr*)&sa, sizeof(sa.sun_family)+strlen(sa.sun_path)) != 0 ) {
		close( sock );
		return TT_ERR;
	}
	write( sock, username, strlen(username) );
	write( sock, "\n", 1 );
	write( sock, password, strlen(password) );
	write( sock, "\n", 1 );
	close( sock );
	return TT_OK;
}

