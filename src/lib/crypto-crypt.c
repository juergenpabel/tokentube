#include <stdio.h>
#include <gcrypt.h>
#include <libscrypt.h>
#include "libtokentube.h"


__attribute__ ((visibility ("hidden")))
int libtokentube_crypto_crypt_impl(tt_cryptmode_t mode, const char* oid, void* data, const size_t data_size, const void* key, const size_t key_size, const void* iv, const size_t iv_size) {
	int			cipher_id = 0;
	gcry_cipher_hd_t	cipher;

	if( oid == NULL || data == NULL || data_size == 0 || key == NULL || key_size == 0 || iv == NULL || iv_size == 0 ) {
		TT_LOG_ERROR( "library/crypto", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	cipher_id = gcry_cipher_map_name( oid );
	if( gcry_cipher_open( &cipher, cipher_id, gcry_cipher_mode_from_oid( oid ), 0 ) != GPG_ERR_NO_ERROR ) {
		TT_LOG_ERROR( "library/crypto", "gcry_cipher_open() failed for '%s' in %s()", oid, __FUNCTION__ );
		return TT_ERR;
	}
	if( gcry_cipher_setkey( cipher, key, key_size ) != GPG_ERR_NO_ERROR ) {
		TT_LOG_ERROR( "library/crypto", "gcry_cipher_setkey() failed for '%s' in %s()", oid, __FUNCTION__ );
		gcry_cipher_close( cipher );
		return TT_ERR;
	}
	if( gcry_cipher_setiv( cipher, iv, iv_size ) != GPG_ERR_NO_ERROR ) {
		TT_LOG_ERROR( "library/crypto", "gcry_cipher_setiv() failed for '%s' in %s()", oid, __FUNCTION__ );
		gcry_cipher_close( cipher );
		return TT_ERR;
	}
	switch( mode ) {
		case CRYPT_ENCRYPT:
			if( gcry_cipher_encrypt( cipher, data, data_size, NULL, 0 ) != GPG_ERR_NO_ERROR ) {
				TT_LOG_ERROR( "library/crypto", "gcry_cipher_encrypt() failed in %s()", __FUNCTION__ );
				gcry_cipher_close( cipher );
				return TT_ERR;
			}
			break;
		case CRYPT_DECRYPT:
			if( gcry_cipher_decrypt( cipher, data, data_size, NULL, 0 ) != GPG_ERR_NO_ERROR ) {
				TT_LOG_ERROR( "library/crypto", "gcry_cipher_decrypt() failed in %s()", __FUNCTION__ );
				gcry_cipher_close( cipher );
				return TT_ERR;
			}
			break;
		default:
			TT_LOG_ERROR( "library/crypto", "bug in %s()", __FUNCTION__ );
			return TT_ERR;
	}
	gcry_cipher_close( cipher );
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_crypto_hash_impl(const char* oid, const void* in, size_t in_size, void* out, size_t* out_size) {
        gcry_md_hd_t	digest = NULL;
        void*		digest_result;
	int		hash_id = 0;

	if( in == NULL || in_size == 0 || out == NULL || out_size == NULL || *out_size == 0 ) {
		TT_LOG_ERROR( "library/crypto", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	hash_id = gcry_md_map_name( oid );
	if( hash_id < 0 ) {
		TT_LOG_ERROR( "library/crypto", "invalid hash='%s' in %s()", oid, __FUNCTION__ );
		return TT_ERR;
	}
	if( *out_size >= gcry_md_get_algo_dlen(hash_id) ) {
		*out_size = gcry_md_get_algo_dlen(hash_id);
	} else {
		TT_LOG_WARN( "library/crypto", "truncating to %d bytes due to insufficient buffer in %s()", *out_size, __FUNCTION__ );
	}
        if( gcry_md_open( &digest, hash_id, 0 ) != GPG_ERR_NO_ERROR ) {
		TT_LOG_ERROR( "library/crypto", "gcry_md_open() failed for '%s' in %s()", oid, __FUNCTION__ );
		return TT_ERR;
	}
	gcry_md_write( digest, in, in_size );
	digest_result = gcry_md_read( digest, 0 );
	memcpy( out, digest_result, *out_size );
	gcry_md_close( digest );
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_crypto_kdf_impl(const char* kdf, size_t kdf_iter, const char* oid, const void* salt, size_t salt_size, const void* in, size_t in_size, void* out, size_t* out_size) {
	int		hash_id;

	if( kdf == NULL || kdf_iter == 0 || oid == NULL || salt == NULL || salt_size == 0 || in == NULL || in_size == 0 || out == NULL || out_size == NULL || *out_size == 0 ) {
		TT_LOG_ERROR( "library/crypto", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	hash_id = gcry_md_map_name( oid );
	if( hash_id == 0 ) {
		TT_LOG_ERROR( "library/crypto", "gcry_md_map_name() failed for '%s' in %s()", oid, __FUNCTION__ );
		return TT_ERR;
	}
	if( strncasecmp( kdf, "s2k", 3 ) == 0 ) {
		if( salt_size != 8 ) {
			TT_LOG_WARN( "library/crypto", "adjusting salt_size to '8' because of S2K-specific requirement in %s()", __FUNCTION__ );
			salt_size = 8;
		}
		if( gcry_kdf_derive( in, in_size, GCRY_KDF_ITERSALTED_S2K, hash_id, salt, salt_size, kdf_iter, *out_size, out ) != GPG_ERR_NO_ERROR ) {
			TT_LOG_ERROR( "library/crypto", "gcry_kdf_derive() failed for 's2k' in %s()", __FUNCTION__ );
			return TT_ERR;
		}
		return TT_OK;
	}
	if( strncasecmp( kdf, "pbkdf2", 6 ) == 0 ) {
		if( gcry_kdf_derive( in, in_size, GCRY_KDF_PBKDF2, hash_id, salt, salt_size, kdf_iter, *out_size, out ) != GPG_ERR_NO_ERROR ) {
			TT_LOG_ERROR( "library/crypto", "gcry_kdf_derive() failed for 'pbkdf2' in %s()", __FUNCTION__ );
			return TT_ERR;
		}
		return TT_OK;
	}
	if( strncasecmp( kdf, "scrypt", 6 ) == 0 ) {
		if( libscrypt_scrypt( in, in_size, salt, salt_size, kdf_iter, 8, 1, out, *out_size ) != 0 ) {
			TT_LOG_ERROR( "library/crypto", "gcry_kdf_derive() failed for 'scrypt' in %s()", __FUNCTION__ );
			return TT_ERR;
		}
		return TT_OK;
	}

	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_crypto_encrypt(void* data, const size_t data_size, const void* key, const size_t key_size, const void* iv, const size_t iv_size) {
	const char*	oid = NULL;
	int		cipher_id = 0;
	size_t		cipher_key_size = 0;
	size_t		cipher_iv_size = 0;

	oid = libtokentube_crypto_get_cipher();
	cipher_id = gcry_cipher_map_name( oid );
	if( gcry_cipher_algo_info( cipher_id, GCRYCTL_GET_KEYLEN, NULL, &cipher_key_size ) != GPG_ERR_NO_ERROR ) {
		TT_LOG_ERROR( "library/crypto", "gcry_cipher_algo_info() failed for '%s' in %s()", oid, __FUNCTION__ );
		return TT_ERR;
	}
	if( cipher_key_size > key_size ) {
		TT_LOG_ERROR( "library/crypto", "not enough key bits for '%s' in %s()", oid, __FUNCTION__ );
		return TT_ERR;
	}
	if( gcry_cipher_algo_info( cipher_id, GCRYCTL_GET_BLKLEN, NULL, &cipher_iv_size ) != GPG_ERR_NO_ERROR ) {
		TT_LOG_ERROR( "library/crypto", "gcry_cipher_algo_info() failed for '%s' in %s()", oid, __FUNCTION__ );
		return TT_ERR;
	}
	if( cipher_iv_size > iv_size ) {
		TT_LOG_ERROR( "library/crypto", "not enough iv bits for '%s' in %s()", oid, __FUNCTION__ );
		return TT_ERR;
	}
	if( libtokentube_crypto_crypt_impl( CRYPT_ENCRYPT, oid, data, data_size, key, cipher_key_size, iv, cipher_iv_size ) != TT_OK) {
		TT_LOG_ERROR( "library/crypto", "libtokentube_crypto_crypt_impl() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_crypto_decrypt(void* data, const size_t data_size, const void* key, const size_t key_size, const void* iv, const size_t iv_size) {
	const char*	oid = NULL;
	int		cipher_id = 0;
	size_t		cipher_key_size = 0;
	size_t		cipher_iv_size = 0;

	oid = libtokentube_crypto_get_cipher();
	cipher_id = gcry_cipher_map_name( oid );
	if( gcry_cipher_algo_info( cipher_id, GCRYCTL_GET_KEYLEN, NULL, &cipher_key_size ) != GPG_ERR_NO_ERROR ) {
		TT_LOG_ERROR( "library/crypto", "gcry_cipher_algo_info() failed for '%s' in %s()", oid, __FUNCTION__ );
		return TT_ERR;
	}
	if( cipher_key_size > key_size ) {
		TT_LOG_ERROR( "library/crypto", "not enough key bits for '%s' in %s()", oid, __FUNCTION__ );
		return TT_ERR;
	}
	if( gcry_cipher_algo_info( cipher_id, GCRYCTL_GET_BLKLEN, NULL, &cipher_iv_size ) != GPG_ERR_NO_ERROR ) {
		TT_LOG_ERROR( "library/crypto", "gcry_cipher_algo_info() failed for '%s' in %s()", oid, __FUNCTION__ );
		return TT_ERR;
	}
	if( cipher_iv_size > iv_size ) {
		TT_LOG_ERROR( "library/crypto", "not enough iv bits for '%s' in %s()", oid, __FUNCTION__ );
		return TT_ERR;
	}
	if( libtokentube_crypto_crypt_impl( CRYPT_DECRYPT, oid, data, data_size, key, cipher_key_size, iv, cipher_iv_size ) != TT_OK) {
		TT_LOG_ERROR( "library/crypto", "libtokentube_crypto_crypt_impl() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_crypto_hash(const void* in, size_t in_size, void* out, size_t* out_size) {
	return libtokentube_crypto_hash_impl( libtokentube_crypto_get_hash(), in, in_size, out, out_size );
}


__attribute__ ((visibility ("hidden")))
int libtokentube_crypto_kdf(const void* salt, size_t salt_size, const void* in, size_t in_size, void* out, size_t* out_size) {
	const char*	hash = NULL;
	const char*	kdf = NULL;
	size_t		kdf_iter = 0;

	if( salt == NULL || salt_size == 0 || in == NULL || in_size == 0 || out == NULL || out_size == NULL || *out_size == 0 ) {
		TT_LOG_ERROR( "library/crypto", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	hash = libtokentube_crypto_get_hash();
	kdf = g_crypto_kdf;
	kdf_iter = g_crypto_kdf_iter;
	if( libtokentube_crypto_kdf_impl( kdf, kdf_iter, hash, salt, salt_size, in, in_size, out, out_size) != TT_OK ) {
		TT_LOG_ERROR( "library/crypto", "libtokentube_crypto_kdf_impl() failed for '%s' in %s()", kdf, __FUNCTION__ );
		return TT_ERR;
	}
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_crypto_random(const void* data, size_t data_size) {
	if( data == NULL || data_size == 0 ) {
		TT_LOG_ERROR( "library/crypto", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	gcry_randomize( (unsigned char*)data, data_size, GCRY_VERY_STRONG_RANDOM );
	return TT_OK;
}

