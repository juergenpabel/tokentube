#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <syslog.h>
#include <errno.h>
#include <confuse.h>
#include <tokentube.h>
#include "pba.h"


int pba_user_loadkey(tt_library_t* library, const char* user, size_t user_size, const char* pass, size_t pass_size, const char* identifier, char* key, size_t* key_size);


int pba_user(cfg_t* cfg, tt_library_t* library, const char* identifier, char* key, size_t* key_size, tt_status_t* status) {
	char            username[TT_USERNAME_CHAR_MAX+1] = {0};
	size_t          username_size = sizeof(username);
	char            password[TT_PASSWORD_CHAR_MAX+1] = {0};
	size_t          password_size = sizeof(password);
	const char*     prompt_username = NULL;
	const char*     prompt_password = NULL;
	const char*     default_username = NULL;

	prompt_username = cfg_getstr( cfg, "user|prompt-username" );
	prompt_password = cfg_getstr( cfg, "user|prompt-password" );
	default_username = cfg_getstr( cfg, "user|default-username" );
	if( default_username != NULL && default_username[0] != '\0' ) {
		strncpy( username, default_username, sizeof(username)-1 );
		cfg_setstr( cfg, "user|default-username", NULL );
	}
	if( pba_plymouth_user( "message-user", "message-pass", prompt_username, prompt_password, username, &username_size, password, &password_size, status ) != TT_OK ) {
		library->api.runtime.system.log( TT_LOG__ERROR, "pba", "pba_plymouth_user() returned TT_ERR" );
		memset( password, '\0', password_size );
		return TT_ERR;
	}
	if( *status == TT_YES ) {
		if( pba_user_loadkey( library, username, username_size, password, password_size, identifier, key, key_size ) != TT_OK ) {
			library->api.runtime.system.log( TT_LOG__ERROR, "pba", "pba_user_loadkey() failed in %s()", __FUNCTION__ );
			memset( password, '\0', password_size );
			return TT_ERR;
		}
	}
	memset( password, '\0', password_size );
	return TT_OK;
}


int pba_user_loadkey(tt_library_t* library, const char* user, size_t user_size, const char* pass, size_t pass_size, const char* identifier, char* key, size_t* key_size) {
	if( user == NULL || user_size == 0 || pass == NULL || pass_size == 0 || identifier == NULL || key == NULL || key_size == NULL ) {
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

