#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <confuse.h>
#include <gcrypt.h>
#include "libtokentube.h"


__attribute__ ((visibility ("hidden")))
int default__api__otp_create(const char* identifier) {
	tt_status_t	status = TT_STATUS__UNDEFINED;
	char		key[TT_KEY_BITS_MAX/8] = { 0 };
	size_t		hash_size = 0;
	size_t		key_size = 0;
	size_t          i = 0;
	tt_otp_t	otp = {0};

	TT_TRACE( "library/plugin", "%s(identifier='%s')", __FUNCTION__, identifier );
	if( identifier == NULL || identifier[0] == '\0' ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( g_crypto_otp_bits == TT_UNINITIALIZED ) {
		TT_LOG_ERROR( "plugin/default", "otp|bits unconfigured in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( libtokentube_plugin__otp_exists( identifier, &status ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "runtime error: error while checking if otp '%s' exists", identifier );
		return TT_ERR;
	}
	if( status == TT_YES ) {
                TT_LOG_ERROR( "plugin/default", "request error: otp '%s' already exists", identifier );
		return TT_ERR;
	}

	key_size = sizeof( key );
	if( libtokentube_plugin__storage_load( TT_FILE__KEY, identifier, key, &key_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_plugin__luks_load failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	hash_size = gcry_md_get_algo_dlen( gcry_md_map_name( libtokentube_crypto_get_hash() ) );
	if( (size_t)g_crypto_otp_bits/8 > hash_size ) {
		TT_LOG_ERROR( "plugin/default", "crypto|hash='%s' does not have enough bits in %s()", libtokentube_crypto_get_hash(), __FUNCTION__ );
		return TT_ERR;
	}
	if( key_size > hash_size ) {
		TT_LOG_ERROR( "plugin/default", "unsupported key size (%d bytes, max=%d) in %s()", key_size, sizeof(otp.data), __FUNCTION__ );
		return TT_ERR;
	}
	otp.data_size = sizeof(otp.data);
	if( libtokentube_crypto_hash( key, g_crypto_otp_bits/8, otp.data, &otp.data_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_crypto_hash() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	otp.data_size = key_size;
	for( i=otp.data_size; i>0; i-- ) {
		otp.data[i-1] = key[i-1] ^ otp.data[(i-1)%(g_crypto_otp_bits/8)];
	}
	otp.bits = g_crypto_otp_bits;
	strncpy( otp.hash, libtokentube_crypto_get_hash(), sizeof(otp.hash)-1 );

	if( default__impl__otp_save( identifier, &otp ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_otp_save() failed for '%s' in %s()", identifier, __FUNCTION__ );
		return TT_ERR;
	}
	libtokentube_runtime_broadcast( TT_EVENT__OTP_CREATED, identifier );
        return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int default__api__otp_exists(const char* identifier, tt_status_t* status) {
	TT_TRACE( "library/plugin", "%s(identifier='%s',status=%p)", __FUNCTION__, identifier, status );
	if( identifier == NULL || identifier[0] == '\0' || status == NULL ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( libtokentube_plugin__storage_exists( TT_FILE__OTP, identifier, status ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_plugin__storage_exists() failed for '%s' in %s()", identifier, __FUNCTION__ );
		return TT_ERR;
	}
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int default__api__otp_delete(const char* identifier, tt_status_t* status) {
	TT_TRACE( "library/plugin", "%s(identifier='%s',status=%p)", __FUNCTION__, identifier, status );
	if( identifier == NULL || identifier[0] == '\0' || status == NULL ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( libtokentube_plugin__otp_exists( identifier, status ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "runtime error: error while checking if otp '%s' exists", identifier );
		return TT_ERR;
	}
	if( *status == TT_YES ) {
		if( libtokentube_plugin__storage_delete( TT_FILE__OTP, identifier, status ) != TT_OK) {
			TT_LOG_ERROR( "plugin/default", "runtime error: error while deleting otp '%s'", identifier );
			return TT_ERR;
		}
		libtokentube_runtime_broadcast( TT_EVENT__OTP_DELETED, identifier );
	}
	return TT_OK;
}

