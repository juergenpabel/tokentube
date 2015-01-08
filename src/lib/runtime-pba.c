#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <syslog.h>
#include "libtokentube.h"


__attribute__ ((visibility ("hidden")))
int libtokentube_runtime_install_pba(const char* type, const char* data) {
	if( type == NULL || data == NULL ) {
		TT_LOG_ERROR( "library/pba", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}

	TT_DEBUG1( "library/pba", "installing pba... (type='%s', data='%s')", type, data );
	if( libtokentube_plugin__pba_install_pre( type, data ) != TT_OK ) {
		TT_LOG_ERROR( "library/pba", "libtokentube_plugin__pba_install_pre() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( libtokentube_plugin__pba_install( type, data ) != TT_OK ) {
		TT_LOG_ERROR( "library/pba", "libtokentube_plugin__pba_install() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( libtokentube_plugin__pba_install_post( type, data ) != TT_OK ) {
		TT_LOG_ERROR( "library/pba", "libtokentube_plugin__pba_install_post() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	TT_DEBUG1( "library/pba", "...installing pba complete" );
	return TT_OK;
}

