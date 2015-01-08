#define _GNU_SOURCE
#define _BSD_SOURCE
#define _POSIX_C_SOURCE 1

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <gcrypt.h>
#include "libtokentube.h"


__attribute__ ((visibility ("hidden")))
char g_crypto_cipher[1024] = { 0 };
__attribute__ ((visibility ("hidden")))
char g_crypto_hash[1024] = { 0 };
__attribute__ ((visibility ("hidden")))
char g_crypto_kdf[1024] = { 0 };
__attribute__ ((visibility ("hidden")))
int  g_crypto_kdf_iter = TT_UNINITIALIZED;
__attribute__ ((visibility ("hidden")))
int  g_crypto_otp_bits = TT_UNINITIALIZED;


__attribute__ ((visibility ("hidden")))
int libtokentube_crypto_initialize() {
	if( gcry_check_version( GCRYPT_VERSION ) == 0 ) {
		TT_LOG_FATAL( "library/crypto", "initialization of libgcrypt failed" );
		return TT_ERR;
	}
	gcry_control( GCRYCTL_DISABLE_SECMEM, 0 );
	gcry_control( GCRYCTL_INITIALIZATION_FINISHED, 0 );
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_crypto_configure() {
	size_t	size;

	size = sizeof(g_crypto_cipher);
	if( libtokentube_conf_read_str( "crypto|cipher", g_crypto_cipher, &size) != TT_OK ) {
		TT_LOG_FATAL( "library/crypto", "error processing configuration value '%s'", "crypto|cipher" );
		return TT_ERR;
	}
	size = sizeof(g_crypto_hash);
	if( libtokentube_conf_read_str( "crypto|hash", g_crypto_hash, &size) != TT_OK ) {
		TT_LOG_FATAL( "library/crypto", "error processing configuration value '%s'", "crypto|hash" );
		return TT_ERR;
	}
	size = sizeof(g_crypto_kdf);
	if( libtokentube_conf_read_str( "crypto|kdf", g_crypto_kdf, &size) != TT_OK ) {
		TT_LOG_FATAL( "library/crypto", "error processing configuration value '%s'", "crypto|kdf" );
		return TT_ERR;
	}
	if( libtokentube_conf_read_int( "crypto|kdf-iterations", &g_crypto_kdf_iter ) != TT_OK ) {
		TT_LOG_FATAL( "library/crypto", "error processing configuration value '%s'", "crypto|kdf-iterations" );
		return TT_ERR;
	}
	if( libtokentube_conf_read_int( "otp|bits", &g_crypto_otp_bits ) != TT_OK ) {
		TT_LOG_FATAL( "library/crypto", "error processing configuration value '%s'", "otp|bits" );
		return TT_ERR;
	}

	TT_DEBUG1( "library/crypto", "crypto|cipher='%s' (%s)", g_crypto_cipher, libtokentube_crypto_get_cipher() );
	TT_DEBUG1( "library/crypto", "crypto|hash='%s' (%s)", g_crypto_hash, libtokentube_crypto_get_hash() );
	TT_DEBUG1( "library/crypto", "crypto|kdf='%s'", g_crypto_kdf );
	TT_DEBUG1( "library/crypto", "crypto|kdf-iterations='%d'", g_crypto_kdf_iter );
	TT_DEBUG1( "library/crypto", "otp|bits='%d'", g_crypto_otp_bits );

	if( gcry_cipher_map_name( libtokentube_crypto_get_cipher() ) <= 0 ) {
		TT_LOG_FATAL( "library/crypto", "unsupported algorithm '%s' configured in crypto|cipher", g_crypto_cipher );
		return TT_ERR;
	}
	if( gcry_md_map_name( libtokentube_crypto_get_hash() ) <= 0 ) {
		TT_LOG_FATAL( "library/crypto", "unsupported algorithm '%s' configured in crypto|hash", g_crypto_hash );
		return TT_ERR;
	}
	if( strncasecmp( g_crypto_kdf, "pbkdf2", 7 ) != 0 && strncasecmp( g_crypto_kdf, "scrypt", 7 ) != 0 ) {
		TT_LOG_FATAL( "library/crypto", "unsupported algorithm '%s' configured in crypto|kdf", g_crypto_kdf );
		return TT_ERR;
	}
	if( g_crypto_kdf_iter == TT_UNINITIALIZED ) {
		TT_LOG_FATAL( "library/crypto", "unsupported value %d configured in crypto|kdf-iterations", g_crypto_kdf_iter );
		return TT_ERR;
	}
	if( g_crypto_otp_bits == TT_UNINITIALIZED ) {
		TT_LOG_FATAL( "library/crypto", "unsupported value %d configured in otp|bits", g_crypto_otp_bits );
		return TT_ERR;
	}
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_crypto_finalize() {
	memset( g_crypto_cipher, '\0', sizeof(g_crypto_cipher) );
	memset( g_crypto_hash, '\0', sizeof(g_crypto_hash) );
	memset( g_crypto_kdf, '\0', sizeof(g_crypto_kdf) );
	g_crypto_kdf_iter = TT_UNINITIALIZED;
	return TT_OK;
}

