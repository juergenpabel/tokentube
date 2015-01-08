#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <tokentube.h>
#include <tokentube/plugin.h>

#include "pba.h"


int pba_otp_plymouth(const char* challenge_text, char* response_text);


int pba_otp(tt_library_t* library, const char* identifier, char* key, size_t* key_len) {
	char	challenge[TT_OTP_TEXT_MAX+1] = { 0 };
	char	response[TT_OTP_TEXT_MAX+1] = { 0 };
	size_t	challenge_len = sizeof(challenge);
	int	result = TT_ERR;

	memset( challenge, '\0', sizeof(challenge) );
	if( library->api.otp.execute_challenge( identifier, challenge, &challenge_len ) != TT_OK) {
		library->api.runtime.log( TT_LOG__ERROR, "pba/otp", "tt_otp_execute_challenge() returned error" );
		return TT_ERR;
	}
	result = pba_otp_plymouth( challenge, response );
	if( result != TT_OK ) {
		library->api.runtime.log( TT_LOG__ERROR, "pba/otp", "pba_otp_plymouth() returned error" );
		return TT_ERR;
	}
	if( library->api.otp.execute_apply( identifier, challenge, response, key, key_len ) == TT_OK ) {
		result = TT_OK;
	} else {
		library->api.runtime.log( TT_LOG__ERROR, "pba/otp", "tt_otp_execute_apply() returned error" );
		memset( key, '\0', *key_len );
	}
	memset( challenge, '\0', sizeof(challenge) );
	memset( response, '\0', sizeof(response) );
	return result;
}

