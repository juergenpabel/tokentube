#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <syslog.h>
#include <errno.h>
#include <confuse.h>

#include <tokentube.h>
#include <tokentube/plugin.h>
#include "pba.h"


int pba_user_loadkey(tt_library_t* library, const char* user, size_t user_len, const char* pass, size_t pass_len, char* key, size_t* key_len) {
	tt_status_t	status = TT_STATUS__UNDEFINED;
	int		result = TT_ERR;

	if( user == NULL || user_len == 0 || pass == NULL || pass_len == 0 || key == NULL || key_len == NULL ) {
		return TT_ERR;
	}
	if( library->api.user.execute_verify( user, pass, &status ) == TT_OK ) {
		if( status == TT_YES ) {
			library->api.runtime.debug( TT_DEBUG__VERBOSITY2, "pba", "tt_user_verify() verified credentials in pba_user_loadkey" );
			if( library->api.user.execute_load( user, pass, key, key_len ) == TT_OK ) {
				result = TT_OK;
			} else {
				library->api.runtime.log( TT_LOG__ERROR, "pba", "tt_user_execute_load() failed in pba_user_loadkey" );
			}
		} else {
			library->api.runtime.log( LOG_INFO, "pba", "tt_user_verify() failed to verify credentials in pba_user_loadkey" );
			result = TT_ERR;
		}
	} else {
		library->api.runtime.log( TT_LOG__ERROR, "pba", "tt_user_verify() failed in pba_user_loadkey" );
		result = TT_ERR;
	}
	if( result == TT_ERR ) {
		memset( key, '\0', *key_len );
	}
	return result;
}

