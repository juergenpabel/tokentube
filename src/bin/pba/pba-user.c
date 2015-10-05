#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <syslog.h>
#include <errno.h>
#include <confuse.h>
#include <tokentube.h>
#include "pba.h"


int pba_user_loadkey(tt_library_t* library, const char* user, size_t user_size, const char* pass, size_t pass_size, const char* identifier, char* key, size_t* key_size) {
	if( user == NULL || user_size == 0 || pass == NULL || pass_size == 0 || identifier == 0 || identifier[0] == '\0' || key == NULL || key_size == NULL ) {
		library->api.runtime.system.log( TT_LOG__ERROR, "pba", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( library->api.auth.user.loadkey( user, pass, identifier, key, key_size ) != TT_OK ) {
		library->api.runtime.system.log( TT_LOG__ERROR, "pba", "API:auth.user.loadkey() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( *key_size == 0 ) {
		library->api.runtime.system.log( LOG_INFO, "pba", "API:auth.user.loadkey() returned incorrect credentials in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	return TT_OK;
}

