#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gcrypt.h>
#include "libtokentube.h"


__attribute__ ((visibility ("hidden")))
int default__impl__user_hmac_set(const char* username, const char* password, tt_user_t* user) {
	tt_user_t  copy;
	size_t     i, size;

	TT_TRACE( "plugin/default", "%s(username='%s',password='%s',user=%p)", __FUNCTION__, username, password, user );
	if( username == NULL || username[0] == '\0' || password == NULL || password[0] == '\0' || user == NULL ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( gcry_md_get_algo_dlen( gcry_md_map_name( user->crypto.hash ) ) == 0 ) {
		TT_LOG_ERROR( "plugin/default", "gcry_md_get_algo_dlen() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	memcpy( &copy, user, sizeof(tt_user_t) );
	copy.crypto.kdf_iter = htonl( copy.crypto.kdf_iter );
	for( i=0; i<DEFAULT__KEY_MAX; i++ ) {
		copy.key[i].uuid_len = htonl( copy.key[i].uuid_len );
		copy.key[i].data_len = htonl( copy.key[i].data_len );
	}
	memset( &copy.hmac, '\0', sizeof(copy.hmac) );

	size = sizeof(copy.hmac.data);
	if( libtokentube_crypto_hmac_impl( copy.crypto.hash, &copy, sizeof(tt_user_t), password, strlen(password), user->hmac.data, &size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_crypto_hmac() failed in %s()", __FUNCTION__ );
		user->hmac.data_len = 0;
		return TT_ERR;
	}
	user->hmac.data_len = size;
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int default__impl__user_hmac_test(const char* username, const char* password, tt_user_t* user, tt_status_t* status) {
	tt_user_t  copy;
	size_t     i, size;

	TT_TRACE( "plugin/default", "%s(username='%s',password='%s',user=%p,status=%p)", __FUNCTION__, username, password, user, status );
	if( username == NULL || username[0] == '\0' || password == NULL || password[0] == '\0' || user == NULL || status == NULL ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( gcry_md_get_algo_dlen( gcry_md_map_name( user->crypto.hash ) ) == 0 ) {
		TT_LOG_ERROR( "plugin/default", "gcry_md_get_algo_dlen() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	memcpy( &copy, user, sizeof(tt_user_t) );
	copy.crypto.kdf_iter = htonl( copy.crypto.kdf_iter );
	for( i=0; i<DEFAULT__KEY_MAX; i++ ) {
		copy.key[i].uuid_len = htonl( copy.key[i].uuid_len );
		copy.key[i].data_len = htonl( copy.key[i].data_len );
	}
	memset( &copy.hmac, '\0', sizeof(copy.hmac) );

	size = sizeof(copy.hmac.data);
	if( libtokentube_crypto_hmac_impl( copy.crypto.hash, &copy, sizeof(tt_user_t), password, strlen(password), copy.hmac.data, &size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_crypto_hmac() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	copy.hmac.data_len = size;

	if( copy.hmac.data_len != user->hmac.data_len ) {
		TT_LOG_WARN( "plugin/default", "size of calculated hmac does not match size of stored hmac in %s()", __FUNCTION__ );
		*status = TT_NO;
		return TT_OK;
	}
	if( memcmp( copy.hmac.data, user->hmac.data, user->hmac.data_len ) == 0 ) {
		TT_DEBUG3( "plugin/default", "%s() returns TT_YES", __FUNCTION__ );
		*status = TT_YES;
	} else {
		TT_DEBUG3( "plugin/default", "%s() returns TT_NO", __FUNCTION__ );
		*status = TT_NO;
	}
	return TT_OK;
}

