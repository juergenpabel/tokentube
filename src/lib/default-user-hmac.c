#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gcrypt.h>
#include "libtokentube.h"


__attribute__ ((visibility ("hidden")))
int default__impl__user_hmac_set(const char* username, const char* password, tt_user_t* user) {
	tt_user_hmac_t orig;

	TT_TRACE( "plugin/default", "%s(username='%s',password='%s',user=%p)", __FUNCTION__, username, password, user );
	if( username == NULL || username[0] == '\0' || password == NULL || password[0] == '\0' || user == NULL ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( gcry_md_get_algo_dlen( gcry_md_map_name( user->crypto.hash ) ) == 0 ) {
		TT_LOG_ERROR( "plugin/default", "gcry_md_get_algo_dlen() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	orig.data_len = sizeof(orig.data);
	memset( &user->hmac, '\0', sizeof(user->hmac) );
	if( libtokentube_crypto_hmac_impl( user->crypto.hash, user, sizeof(tt_user_t), password, strlen(password), orig.data, &orig.data_len ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_crypto_hmac() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	memcpy( &user->hmac, &orig, sizeof(tt_user_hmac_t) );
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int default__impl__user_hmac_test(const char* username, const char* password, tt_user_t* user, tt_status_t* status) {
	tt_user_hmac_t orig;
	tt_user_hmac_t test;

	TT_TRACE( "plugin/default", "%s(username='%s',password='%s',user=%p,status=%p)", __FUNCTION__, username, password, user, status );
	if( username == NULL || username[0] == '\0' || password == NULL || password[0] == '\0' || user == NULL || status == NULL ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( gcry_md_get_algo_dlen( gcry_md_map_name( user->crypto.hash ) ) == 0 ) {
		TT_LOG_ERROR( "plugin/default", "gcry_md_get_algo_dlen() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	memcpy( &orig, &user->hmac, sizeof(tt_user_hmac_t) );
	test.data_len = sizeof(test.data);
	memset( &user->hmac, '\0', sizeof(tt_user_hmac_t) );
	if( libtokentube_crypto_hmac_impl( user->crypto.hash, user, sizeof(tt_user_t), password, strlen(password), test.data, &test.data_len ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_crypto_hmac() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( test.data_len != orig.data_len ) {
		TT_LOG_WARN( "plugin/default", "size of calculated hmac does not match size of stored hmac in %s()", __FUNCTION__ );
		*status = TT_NO;
		return TT_OK;
	}
	if( memcmp( test.data, orig.data, orig.data_len ) == 0 ) {
		TT_DEBUG3( "plugin/default", "%s() returns TT_YES", __FUNCTION__ );
		*status = TT_YES;
	} else {
		TT_DEBUG3( "plugin/default", "%s() returns TT_NO", __FUNCTION__ );
		*status = TT_NO;
	}
	return TT_OK;
}

