#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <confuse.h>
#include <tokentube.h>

#include "pba.h"


int pba_otp(cfg_t* cfg, tt_library_t* library, const char* identifier, char* key, size_t* key_size, tt_status_t* status) {
	char         challenge[TT_OTP_TEXT_MAX+1] = { 0 };
	size_t       challenge_size = sizeof(challenge);
	char         response[TT_OTP_TEXT_MAX+1] = { 0 };
	char         message[TT_CONFIG_MAX+1] = { 0 };
	size_t       message_size = sizeof(message);
	const char*  filename = NULL;

	filename = cfg_getstr( cfg, "otp|message-filename" );
	if( filename != NULL ) {
		if( library->api.storage.load( TT_FILE__CONFIG_PBA, filename, message, &message_size ) != TT_OK ) {
			library->api.runtime.system.log( TT_LOG__ERROR, "pba/otp", "API:storage.load() returned error in %s()", __FUNCTION__ );
			return TT_ERR;
		}
	}
	if( library->api.auth.otp.challenge( identifier, challenge, &challenge_size ) != TT_OK) {
		library->api.runtime.system.log( TT_LOG__ERROR, "pba/otp", "API:auth.otp.challenge() returned error in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( pba_plymouth_otp( message, challenge, response, sizeof(response), status ) != TT_OK ) {
		library->api.runtime.system.log( TT_LOG__ERROR, "pba/otp", "pba_otp_plymouth() returned error in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( *status == TT_YES ) {
		if( library->api.auth.otp.loadkey( identifier, challenge, response, key, key_size ) != TT_OK ) {
			library->api.runtime.system.log( TT_LOG__ERROR, "pba/otp", "tt_auth.otp.loadkey() returned error in %s()", __FUNCTION__ );
			memset( challenge, '\0', sizeof(challenge) );
			memset( response, '\0', sizeof(response) );
			return TT_ERR;
		}
	}
	memset( challenge, '\0', sizeof(challenge) );
	memset( response, '\0', sizeof(response) );
	return TT_OK;
}

