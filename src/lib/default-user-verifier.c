#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gcrypt.h>
#include "libtokentube.h"


__attribute__ ((visibility ("hidden")))
int default__impl__user_verifier_set(const char* username, const char* password, tt_user_t* user) {
	char	data[TT_DIGEST_BITS_MAX/8] = {0};
	char	salt[TT_DIGEST_BITS_MAX/8] = {0};
	char	key[TT_DIGEST_BITS_MAX/8] = {0};
	size_t	data_size;

	TT_TRACE( "plugin/default", "%s(username='%s',password='%s',user=%p)", __FUNCTION__, username, password, user );
	if( username == NULL || username[0] == '\0' || password == NULL || password[0] == '\0' || user == NULL ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	data_size = gcry_md_get_algo_dlen( gcry_md_map_name( user->crypto.hash ) );
	if( data_size == 0 ) {
		TT_LOG_ERROR( "plugin/default", "gcry_md_get_algo_dlen() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( libtokentube_crypto_hash( username, strlen(username), salt, &data_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_crypto_hash() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( libtokentube_crypto_hash( password, strlen(password), key, &data_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_crypto_hash() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( libtokentube_crypto_kdf( salt, data_size, key, data_size, data, &data_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_crypto_kdf() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( libtokentube_crypto_hash( data, data_size, data, &data_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_crypto_hash() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	memcpy( user->verifier.data, data, data_size );
	user->verifier.data_len = data_size;
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int default__impl__user_verifier_test(const char* username, const char* password, tt_user_t* user, tt_status_t* status) {
	char	data[TT_DIGEST_BITS_MAX/8] = {0};
	char	salt[TT_DIGEST_BITS_MAX/8] = {0};
	char	key[TT_DIGEST_BITS_MAX/8] = {0};
	size_t	data_size;

	TT_TRACE( "plugin/default", "%s(username='%s',password='%s',user=%p,status=%p)", __FUNCTION__, username, password, user, status );
	if( username == NULL || username[0] == '\0' || password == NULL || password[0] == '\0' || user == NULL || status == NULL ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	data_size = gcry_md_get_algo_dlen( gcry_md_map_name( user->crypto.hash ) );
	if( data_size == 0 ) {
		TT_LOG_ERROR( "plugin/default", "gcry_md_get_algo_dlen() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( libtokentube_crypto_hash_impl( user->crypto.hash, username, strlen(username), salt, &data_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_crypto_hash() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( libtokentube_crypto_hash_impl( user->crypto.hash, password, strlen(password), key, &data_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_crypto_hash() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( libtokentube_crypto_kdf_impl( user->crypto.kdf, user->crypto.kdf_iter, user->crypto.hash, salt, data_size, key, data_size, data, &data_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_crypto_kdf() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( libtokentube_crypto_hash_impl( user->crypto.hash, data, data_size, data, &data_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_crypto_hash() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( data_size != user->verifier.data_len ) {
		TT_LOG_WARN( "plugin/default", "size of calculated verifier does not match size of stored verifier in %s()", __FUNCTION__ );
		*status = TT_NO;
		return TT_OK;
	}
	if( memcmp( data, user->verifier.data, data_size ) == 0 ) {
		TT_DEBUG3( "plugin/default", "%s() returns TT_YES", __FUNCTION__ );
		*status = TT_YES;
	} else {
		TT_DEBUG3( "plugin/default", "%s() returns TT_NO", __FUNCTION__ );
		*status = TT_NO;
	}
	return TT_OK;
}

