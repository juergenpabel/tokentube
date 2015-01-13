#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <syslog.h>
#include <errno.h>
#include <confuse.h>
#include <tokentube.h>
#include "pba.h"


int pba_user_loadkey(tt_library_t* library, const char* user, size_t user_size, const char* pass, size_t pass_size, char* key, size_t* key_size) {
	tt_status_t	status = TT_STATUS__UNDEFINED;

	if( user == NULL || user_size == 0 || pass == NULL || pass_size == 0 || key == NULL || key_size == NULL ) {
		library->api.runtime.log( TT_LOG__ERROR, "pba", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( library->api.user.execute_verify == NULL ) {
		library->api.runtime.log( TT_LOG__ERROR, "pba", "API:user.verify() undefined in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( library->api.user.execute_verify( user, pass, &status ) != TT_OK ) {
		library->api.runtime.log( TT_LOG__ERROR, "pba", "API:user.verify() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	switch( status ) {
		case TT_YES:
			library->api.runtime.debug( TT_DEBUG__VERBOSITY2, "pba", "API:user.verify() verified credentials in %s()", __FUNCTION__ );
			if( library->api.user.execute_load( user, pass, key, key_size ) != TT_OK ) {
				library->api.runtime.log( TT_LOG__ERROR, "pba", "API:user.execute_load() failed in %s()", __FUNCTION__ );
				return TT_ERR;
			}
			break;
		case TT_NO:
			library->api.runtime.log( LOG_INFO, "pba", "API:user.verify() failed to verify credentials in %s()", __FUNCTION__ );
			return TT_ERR;
		default:
			library->api.runtime.log( LOG_INFO, "pba", "API:user.verify() returned error in %s()", __FUNCTION__ );
			return TT_ERR;
	}
	return TT_OK;
}

