#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gcrypt.h>
#include "libtokentube.h"


__attribute__ ((visibility ("hidden")))
int  default__impl__user_key_crypt_impl(tt_cryptmode_t mode, const char* username, const char* password, tt_user_t* user) {
	int     cipher_id = 0;
	char    key[TT_KEY_BITS_MAX/8] = {0};
	size_t  key_size = sizeof(key);
	char    iv[TT_CIPHER_BITS_MAX/8] = {0};
	size_t  iv_size = sizeof(iv);

	TT_TRACE( "plugin/default", "%s(username='%s',password='%s',user=%p)", __FUNCTION__, username, password, user );
	if( username == NULL || username[0] == '\0' || password == NULL || password[0] == '\0' || user == NULL ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	cipher_id = gcry_cipher_map_name( user->cipher );
	if( gcry_cipher_algo_info( cipher_id, GCRYCTL_GET_KEYLEN, NULL, &key_size ) != GPG_ERR_NO_ERROR ) {
		TT_LOG_ERROR( "library/crypto", "gcry_cipher_algo_info() failed for '%s' in %s()", user->cipher, __FUNCTION__ );
		return TT_ERR;
	}
	if( key_size > TT_KEY_BITS_MAX/8 ) {
		TT_LOG_ERROR( "library/crypto", "gcry_cipher_algo_info() return key size too big for '%s' in %s()", user->cipher, __FUNCTION__ );
		return TT_ERR;
	}
	if( gcry_cipher_algo_info( cipher_id, GCRYCTL_GET_BLKLEN, NULL, &iv_size ) != GPG_ERR_NO_ERROR ) {
		TT_LOG_ERROR( "library/crypto", "gcry_cipher_algo_info() failed for '%s' in %s()", user->cipher, __FUNCTION__ );
		return TT_ERR;
	}
	if( iv_size > TT_CIPHER_BITS_MAX/8 ) {
		TT_LOG_ERROR( "library/crypto", "gcry_cipher_algo_info() return iv size too big for '%s' in %s()", user->cipher, __FUNCTION__ );
		return TT_ERR;
	}
	if( libtokentube_crypto_kdf_impl( user->kdf, user->kdf_iter, user->hash, username, strlen(username), password, strlen(password), key, &key_size ) != TT_OK ) {
		TT_LOG_ERROR( "library/crypto", "libtokentube_crypto_kdf_impl() failed in %s()", __FUNCTION__ );
		memset( key, '\0', sizeof(key) );
		memset( iv, '\0', sizeof(iv) );
		return TT_ERR;
	}
	if( libtokentube_crypto_kdf_impl( user->kdf, user->kdf_iter, user->hash, password, strlen(password), username, strlen(username), iv, &iv_size ) != TT_OK ) {
		TT_LOG_ERROR( "library/crypto", "libtokentube_crypto_kdf_impl() failed in %s()", __FUNCTION__ );
		memset( key, '\0', sizeof(key) );
		memset( iv, '\0', sizeof(iv) );
		return TT_ERR;
	}
	if( libtokentube_crypto_crypt_impl( mode, user->cipher, user->luks_data, user->luks_data_len, key, key_size, iv, iv_size ) != TT_OK ) {
		TT_LOG_ERROR( "library/crypto", "libtokentube_crypto_crypt_impl() failed in %s()", __FUNCTION__ );
		memset( key, '\0', sizeof(key) );
		memset( iv, '\0', sizeof(iv) );
		return TT_ERR;
	}
	memset( key, '\0', sizeof(key) );
	memset( iv, '\0', sizeof(iv) );
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int  default__impl__user_key_encrypt(const char* username, const char* password, tt_user_t* user) {
	return default__impl__user_key_crypt_impl( CRYPT_ENCRYPT, username, password, user );
}



__attribute__ ((visibility ("hidden")))
int  default__impl__user_key_decrypt(const char* username, const char* password, tt_user_t* user) {
	return default__impl__user_key_crypt_impl( CRYPT_DECRYPT, username, password, user );
}

