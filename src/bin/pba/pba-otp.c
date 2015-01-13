#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <tokentube.h>

#include "pba.h"


int pba_otp(tt_library_t* library, const char* identifier, char* key, size_t* key_size) {
	char	challenge[TT_OTP_TEXT_MAX+1] = { 0 };
	char	response[TT_OTP_TEXT_MAX+1] = { 0 };
	size_t	challenge_size = sizeof(challenge);

	if( library->api.otp.execute_challenge == NULL ) {
		library->api.runtime.log( TT_LOG__ERROR, "pba/otp", "API:otp.execute_challenge undefined in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( library->api.otp.execute_challenge( identifier, challenge, &challenge_size ) != TT_OK) {
		library->api.runtime.log( TT_LOG__ERROR, "pba/otp", "API:otp.execute_challenge() returned error in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( pba_otp_plymouth( challenge, response ) != TT_OK ) {
		library->api.runtime.log( TT_LOG__ERROR, "pba/otp", "pba_otp_plymouth() returned error in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( library->api.otp.execute_apply( identifier, challenge, response, key, key_size ) != TT_OK ) {
		library->api.runtime.log( TT_LOG__ERROR, "pba/otp", "tt_otp_execute_apply() returned error in %s()", __FUNCTION__ );
		memset( challenge, '\0', sizeof(challenge) );
		memset( response, '\0', sizeof(response) );
		return TT_ERR;
	}
	memset( challenge, '\0', sizeof(challenge) );
	memset( response, '\0', sizeof(response) );
	return TT_OK;
}

