#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>
#include <confuse.h>
#include <tokentube.h>

#include "pba.h"


int pba_initialize(tt_library_t* api, const char* bootdevice, const char* configuration) {
	if( bootdevice != NULL && bootdevice[0] != '\0' ) {
		setenv( "TT_RUNTIME_BOOTDEVICE", bootdevice, 1 );
	} else {
		signal( SIGCHLD, SIG_IGN );
		signal( SIGPIPE, SIG_IGN );
		mkdir( "/boot",                     S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH );
		mkdir( "/boot/tokentube",           S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH );
		mkdir( "/etc",                      S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH );
		mkdir( "/etc/tokentube",            S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH );
		mkdir( "/dev",                      S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH );
		mkdir( "/dev/.initramfs",           S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH );
		mkdir( "/dev/.initramfs/tokentube", S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH );
		mkdir( "/tmp",                      S_IRWXU|S_IRWXG|S_IRWXO|S_ISVTX );
	}
	if( tt_initialize( TT_VERSION ) != TT_OK ) {
		return( TT_ERR );
	}
	if( tt_configure( configuration ) != TT_OK ) {
		return( TT_ERR );
	}
	if( tt_discover( api ) != TT_OK ) {
		return( TT_ERR );
	}
	return TT_OK;
}

