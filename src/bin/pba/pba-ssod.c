#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <confuse.h>
#include <tokentube.h>
#include "pba.h"


int pba_ssod_start(const char* executable, const char* config, const char* sockname) {
	if( access( sockname, F_OK ) == 0 ) {
		return TT_OK;
	}
	if( errno != ENOENT ) {
		return TT_ERR;
	}
	switch( fork() ) {
		case -1:
			fprintf( stderr, "pba: fork() failed in %s()\n", __FUNCTION__ );
			return TT_ERR;
		case 0:
			execl( executable, executable, config, NULL );
			fprintf( stderr, "pba: execl() failed for '%s' in %s()\n", executable, __FUNCTION__ );
			exit(-1);
		default:
			sleep( 1 );
	}
	return TT_OK;
}


int pba_ssod_stop(const char* sockname) {
	struct sockaddr_un	sa = {0};
	int			sock;

	if( access( TT_FILENAME__SSOD_INITRAMFS_DIR "/" TT_FILENAME__SSOD_TOKENTUBE_DIR "/" TT_FILENAME__SSOD_SOCKET, F_OK ) < 0 ) {
		return TT_OK;
	}
	sa.sun_family = AF_UNIX;
	strncpy( sa.sun_path, sockname, sizeof(sa.sun_path)-1 );
	sock = socket( AF_UNIX, SOCK_STREAM, 0 );
	if( sock < 0 ) {
		fprintf( stderr, "pba: socket() failed in %s()\n", __FUNCTION__ );
		return TT_ERR;
	}
	if( connect( sock, (struct sockaddr*)&sa, sizeof(sa.sun_family) + strnlen( sa.sun_path, sizeof(sa.sun_path) ) ) != 0 ) {
		fprintf( stderr, "pba: connect() failed in %s()\n", __FUNCTION__ );
		close( sock );
		return TT_ERR;
	}
	close( sock );
	return TT_OK;
}


int pba_ssod_credentials(const char* sockname, const char* username, const char* password) {
	struct sockaddr_un	sa = {0};
	int			sock;

	sa.sun_family = AF_UNIX;
	strncpy( sa.sun_path, sockname, sizeof(sa.sun_path)-1 );
	sock = socket( AF_UNIX, SOCK_STREAM, 0 );
	if( sock < 0 ) {
		fprintf( stderr, "pba: socket() failed in %s()\n", __FUNCTION__ );
		return TT_ERR;
	}
	if( connect(sock, (struct sockaddr*)&sa, sizeof(sa.sun_family) + strnlen( sa.sun_path, sizeof(sa.sun_path) ) ) != 0 ) {
		fprintf( stderr, "pba: connect() failed in %s()\n", __FUNCTION__ );
		close( sock );
		return TT_ERR;
	}
	write( sock, username, strnlen( username, TT_USERNAME_CHAR_MAX ) );
	write( sock, "\n", 1 );
	write( sock, password, strnlen( password, TT_PASSWORD_CHAR_MAX ) );
	write( sock, "\n", 1 );
	close( sock );
	return TT_OK;
}

