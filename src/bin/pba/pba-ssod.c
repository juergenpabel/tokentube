#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <confuse.h>
#include <tokentube.h>
#include "pba.h"


int pba_sso_start(const char* executable, const char* config, const char* sockname) {
	struct timespec	delay = {0};

	if( executable == NULL || config == NULL || sockname == NULL ) {
		return TT_ERR;
	}
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
			delay.tv_sec = 0;
			delay.tv_nsec = 000050000;
			while( delay.tv_sec < 3 && access( sockname, F_OK ) < 0 ) {
				nanosleep( &delay, NULL );
				delay.tv_sec++;
			}
	}
	return TT_OK;
}


int pba_sso_stop(const char* sockname) {
	struct sockaddr_un	sa = {0};
	int			sock;

	if( sockname == NULL ) {
		return TT_ERR;
	}
	if( access( sockname, F_OK ) < 0 ) {
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


int pba_sso_credentials(const char* sockname, const char* username, const char* password) {
	struct sockaddr_un	sa = {0};
	int			sock;
	struct ucred		usercred = {0};
	socklen_t		uclen = sizeof(usercred);

	if( sockname == NULL || username == NULL || password == NULL ) {
		return TT_ERR;
	}
	sock = socket( AF_UNIX, SOCK_STREAM, 0 );
	if( sock < 0 ) {
		fprintf( stderr, "pba: socket() failed in %s()\n", __FUNCTION__ );
		return TT_ERR;
	}
	sa.sun_family = AF_UNIX;
	strncpy( sa.sun_path, sockname, sizeof(sa.sun_path)-1 );
	if( connect(sock, (struct sockaddr*)&sa, sizeof(sa.sun_family) + strnlen( sa.sun_path, sizeof(sa.sun_path) ) ) != 0 ) {
		fprintf( stderr, "pba: connect() failed for '%s' in %s()\n", sa.sun_path, __FUNCTION__ );
		close( sock );
		return TT_ERR;
	}
	if( getsockopt( sock, SOL_SOCKET, SO_PEERCRED, &usercred, &uclen ) < 0 ) {
		fprintf( stderr, "pba: getsockopt() failed for '%s' in %s()\n", sa.sun_path, __FUNCTION__ );
		close( sock );
		return TT_ERR;
	}
	if( usercred.uid != getuid() ) {
		fprintf( stderr, "pba: connection from another uid (%ld instead of %ld) in %s()\n", (long)usercred.uid, (long)getuid(), __FUNCTION__ );
		close( sock );
		return TT_NO;
	}
	write( sock, username, strnlen( username, TT_USERNAME_CHAR_MAX ) );
	write( sock, "\n", 1 );
	write( sock, password, strnlen( password, TT_PASSWORD_CHAR_MAX ) );
	write( sock, "\n", 1 );
	close( sock );
	return TT_OK;
}

