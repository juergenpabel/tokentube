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


static cfg_opt_t opt_helpdesk_data[] = {
	CFG_STR("host-id", NULL, CFGF_NODEFAULT),
	CFG_STR("otp-hash", NULL, CFGF_NODEFAULT),
	CFG_INT("otp-iterations", 0, CFGF_NODEFAULT),
	CFG_INT("otp-bits", 0, CFGF_NODEFAULT),
	CFG_STR("key-id", NULL, CFGF_NODEFAULT),
	CFG_STR("key-data", NULL, CFGF_NODEFAULT),
	CFG_END()
};

static cfg_opt_t opt_helpdesk[] = {
	CFG_STR("api", "", CFGF_NONE),
	CFG_SEC("helpdesk", opt_helpdesk_data, CFGF_NONE),
	CFG_END()
};


__attribute__ ((visibility ("hidden")))
void default__event__otp_created(const char* identifier) {
	char            buffer[DEFAULT__FILESIZE_MAX+1] = { 0 };
	size_t          buffer_size = sizeof(buffer);
	char            key[TT_KEY_BITS_MAX/8] = { 0 };
	size_t          key_size = sizeof(key);
	char            sysid[64] = { 0 };
	size_t          sysid_size = sizeof(sysid);
	tt_otp_t	otp = { 0 };
	cfg_t*		cfg = NULL;

	TT_TRACE( "plugin/default", "%s(identifier='%s')", __FUNCTION__, identifier );
	if( identifier == NULL ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameters in %s()", __FUNCTION__ );
		return;
	}
	if( default__impl__otp_load( identifier, &otp ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "default__impl__otp_load() failed for identifier '%s' in %s()", identifier, __FUNCTION__ );
		return;
	}
	if( libtokentube_runtime_get_sysid( sysid, &sysid_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "API:get_sysid() failed for identifier '%s' in %s()", identifier, __FUNCTION__ );
		return;
	}
	if( libtokentube_plugin__storage_load( TT_FILE__KEY, identifier, key, &key_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_plugin__storage_load() failed for identifier '%s' in %s()", identifier, __FUNCTION__ );
		return;
	}
	cfg = cfg_init( opt_helpdesk, CFGF_NONE );
	if( cfg == NULL ) {
		TT_LOG_ERROR( "plugin/default", "cfg_init() failed in %s()", __FUNCTION__ );
		return;
	}
	snprintf( buffer, buffer_size, "plugin/default (%d.%d.%d)", TT_VERSION_MAJOR, TT_VERSION_MINOR, TT_VERSION_PATCH );
	cfg_setstr( cfg, "api", buffer );
	cfg_setstr( cfg, "helpdesk|host-id", sysid );
	cfg_setstr( cfg, "helpdesk|otp-hash", otp.hash );
	cfg_setint( cfg, "helpdesk|otp-iterations", 1 );
	cfg_setint( cfg, "helpdesk|otp-bits", (int)otp.bits );
	cfg_setstr( cfg, "helpdesk|key-id", identifier );
	memset( buffer,'\0', sizeof(buffer) );
	if( libtokentube_util_base64_encode( key, key_size, buffer, &buffer_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "API:base64_encode failed for identifier '%s' in %s()", identifier, __FUNCTION__ );
		cfg_free( cfg );
		return;
	}
	cfg_setstr( cfg, "helpdesk|key-data", buffer );
	buffer_size = sizeof(buffer);
	if( libtokentube_runtime_conf__serialize( cfg, buffer, &buffer_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_cfg_serialize() failed in %s()", __FUNCTION__ );
		cfg_free( cfg );
		return;
	}
	cfg_free( cfg );
	if( libtokentube_plugin__storage_save( TT_FILE__UHD, identifier, buffer, buffer_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "API:storage_save failed in %s()", __FUNCTION__ );
	}
}


__attribute__ ((visibility ("hidden")))
void default__event__otp_deleted(const char* identifier) {
	tt_status_t     status = TT_STATUS__UNDEFINED;

	TT_TRACE( "plugin/default", "%s(identifier='%s')", __FUNCTION__, identifier );
	if( identifier == NULL ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameters in %s()", __FUNCTION__ );
		return;
	}
	if( libtokentube_plugin__storage_delete( TT_FILE__UHD, identifier, &status ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "API:storage_delete failed in %s()", __FUNCTION__ );
	}
}


__attribute__ ((visibility ("hidden")))
int default__api__otp_execute_challenge( const char* identifier, char* challenge, size_t* challenge_size) {
	char		bytes[2+TT_OTP_BITS_MAX/8] = { 0 };
	tt_otp_t	otp = {0};
	unsigned short	crc = 0;
	size_t		i = 0;

	TT_TRACE( "plugin/default", "%s(identifier='%s')", __FUNCTION__, identifier );
	if( identifier == NULL || challenge == NULL || challenge_size == NULL || *challenge_size == 0 ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameters in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	srandom( time(NULL) );
	if( default__impl__otp_load( identifier, &otp ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "default__impl__otp_load() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( 2+otp.bits/8 > sizeof(bytes) ) {
		TT_LOG_ERROR( "plugin/default", "otp.bits/8=%d > sizeof(bytes)=%d in %s()", otp.bits/8, sizeof(bytes), __FUNCTION__ );
		return TT_ERR;
	}
	if( libtokentube_util_crc16( otp.data, otp.bits/8, &crc ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "crc16() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	bytes[0] = (crc >> 0) & 0xff;
	bytes[1] = (crc >> 8) & 0xff;
	for( i=0; i<otp.bits/8; i++ ) {
		bytes[2+i] = random() & 0xff;
	}
	if( libtokentube_util_base32_encode( bytes, 2+otp.bits/8, challenge, challenge_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "API:base32_encode() failed for identifier '%s' in %s()", identifier,  __FUNCTION__ );
		return TT_ERR;
	}
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int default__api__otp_execute_response(const char* identifier, const char* challenge, char* response, size_t* response_size) {
	char		buffer[DEFAULT__FILESIZE_MAX+1] = {0};
	size_t		buffer_size = sizeof(buffer);
	char		challenge_raw[2+TT_OTP_BITS_MAX/8] = {0};
	size_t		challenge_raw_size = sizeof(challenge_raw);
	char		response_raw[2+TT_OTP_BITS_MAX/8] = {0};
	char		key[TT_KEY_BITS_MAX/8] = {0};
	size_t		key_size = sizeof(key);
	char		helpdesk_luks_key[TT_KEY_BITS_MAX/8/6*8+3+1] = {0};
	size_t		helpdesk_otp_iter = 0;
	int		challenge_size = 0;
	cfg_t*		cfg = NULL;
	size_t		i = 0;
	unsigned short	crc;
	tt_otp_t	otp = {0};


	TT_TRACE( "plugin/default", "%s(identifier='%s',challenge='%s')", __FUNCTION__, identifier, challenge );
	if( identifier == NULL || challenge == NULL || response == NULL || response_size == NULL || *response_size == 0 ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameters in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( libtokentube_plugin__storage_load( TT_FILE__UHD, identifier, buffer, &buffer_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "API:storage_load() failed for identifier='%s' in %s()", identifier, __FUNCTION__ );
		return TT_ERR;
	}
	cfg = cfg_init( opt_helpdesk, CFGF_NONE );
	if( cfg == NULL ) {
		TT_LOG_ERROR( "plugin/default", "cfg_init() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( cfg_parse_buf( cfg, buffer ) != CFG_SUCCESS ) {
		TT_LOG_ERROR( "plugin/default", "cfg_parse() failed for identifier '%s' in %s()", identifier, __FUNCTION__ );
		cfg_free( cfg );
		return TT_ERR;
	}
	if( strncmp( cfg_getstr( cfg, "helpdesk|key-id" ), identifier, TT_IDENTIFIER_CHAR_MAX ) != 0 ) {
		TT_LOG_ERROR( "plugin/default", "API:storage_load() for identifier '%s' returned non-matching record in %s()", identifier, __FUNCTION__ );
		cfg_free( cfg );
		return TT_ERR;
	}
	strncpy( otp.hash, cfg_getstr( cfg, "helpdesk|otp-hash" ), sizeof(otp.hash)-1 );
	otp.bits = cfg_getint( cfg, "helpdesk|otp-bits" );
	helpdesk_otp_iter = cfg_getint( cfg, "helpdesk|otp-iterations" );
	strncpy( helpdesk_luks_key, cfg_getstr( cfg, "helpdesk|key-data" ), sizeof(helpdesk_luks_key)-1 );
	if( otp.hash[0] == '\0' || helpdesk_otp_iter == 0 || otp.bits == 0 || helpdesk_luks_key[0] == '\0' ) {
		TT_LOG_ERROR( "plugin/default", "error while parsing helpdesk file for identifier '%s'", identifier );
		cfg_free( cfg );
		return TT_ERR;
	}
	if( libtokentube_util_base64_decode( helpdesk_luks_key, strlen(helpdesk_luks_key), key, &key_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "base64_decode() failed for key-data for identifier '%s'", identifier );
		cfg_free( cfg );
		return TT_ERR;
	}
	if( key_size < TT_CIPHER_BITS_MIN/8 || key_size > TT_CIPHER_BITS_MAX/8 ) {
		TT_LOG_ERROR( "plugin/default", "data too small or too arge for identifier '%s'", identifier );
		cfg_free( cfg );
		return TT_ERR;
	}
	memcpy( otp.data, key, key_size );
	for( i=0; i<helpdesk_otp_iter; i++ ) {
		otp.data_size = sizeof(otp.data);
		if( libtokentube_crypto_hash_impl( otp.hash, otp.data, otp.bits/8, otp.data, &otp.data_size ) != TT_OK ) {
			TT_LOG_ERROR( "plugin/default", "libtokentube_crypto_hash_impl() failed in %s()", __FUNCTION__ );
			cfg_free( cfg );
			return TT_ERR;
		}
	}
	for( i=key_size; i>0; i-- ) {
		otp.data[i-1] = key[i-1] ^ otp.data[(i-1)%(g_crypto_otp_bits/8)];
	}
	if( libtokentube_util_crc16( otp.data, otp.bits/8, &crc ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "crc16() failed in %s()", __FUNCTION__ );
		cfg_free( cfg );
		return TT_ERR;
	}
	challenge_size = strlen(challenge);
	if( libtokentube_util_base32_decode( challenge, challenge_size, challenge_raw, &challenge_raw_size ) != TT_OK) {
		TT_LOG_ERROR( "plugin/default", "base32_decode() failed in %s()", __FUNCTION__ );
		cfg_free( cfg );
		return TT_ERR;
	}
	if( challenge_raw_size != (2+otp.bits/8) ) {
		TT_LOG_ERROR( "plugin/default", "challenge and reponse are of different length" );
		cfg_free( cfg );
		return TT_ERR;
	}
	if( (char)((crc>>0) & 0xff) != challenge_raw[0] || (char)((crc>>8) & 0xff) != challenge_raw[1] ) {
		TT_LOG_ERROR( "plugin/default", "CRC of challenge incorrect (%u %u, instead of %u %u)", (unsigned char)challenge_raw[0], (unsigned char)challenge_raw[1], ((crc>>0)&0xff), ((crc>>8)&0xff) );
		cfg_free( cfg );
		return TT_ERR;
	}
	for( i=0; i<otp.bits/8; i++ ) {
		response_raw[2+i] = challenge_raw[2+i] ^ key[i] ^ otp.data[i];
	}
	for( i=0; i<key_size; i++ ) {
		otp.data[i] = otp.data[i] ^ key[i];
	}
	otp.data_size = sizeof(otp.data);
	if( libtokentube_crypto_hash_impl( otp.hash, otp.data, otp.bits/8, otp.data, &otp.data_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_crypto_hash_impl() failed in %s()", __FUNCTION__ );
		cfg_free( cfg );
		return TT_ERR;
	}
	for( i=key_size; i>0; i-- ) {
		otp.data[i-1] = key[i-1] ^ otp.data[(i-1)%(g_crypto_otp_bits/8)];
	}
	if( libtokentube_util_crc16( otp.data, otp.bits/8, &crc ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "API:crc16() failed in %s()", __FUNCTION__ );
		cfg_free( cfg );
		return TT_ERR;
	}
	response_raw[0] = (crc>>0) & 0xff;
	response_raw[1] = (crc>>8) & 0xff;
	if( libtokentube_util_base32_encode(response_raw, 2+otp.bits/8, response, response_size) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "API:base32_encode() failed in %s()", __FUNCTION__ );
		cfg_free( cfg );
		return TT_ERR;
	}
	snprintf( buffer, sizeof(buffer)-1, "plugin/default (%d.%d.%d)", TT_VERSION_MAJOR, TT_VERSION_MINOR, TT_VERSION_PATCH );
	cfg_setstr( cfg, "api", buffer );
	cfg_setint( cfg, "helpdesk|otp-iterations", helpdesk_otp_iter+1 );
	buffer_size = sizeof(buffer);
	if( libtokentube_runtime_conf__serialize( cfg, buffer, &buffer_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_cfg_serialize() failed in %s()", __FUNCTION__ );
		cfg_free( cfg );
		return TT_ERR;
	}
	cfg_free( cfg );
	if( libtokentube_plugin__storage_save( TT_FILE__UHD, identifier, buffer, strnlen( buffer, sizeof(buffer) ) ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "API:storage_save() failed for identifier='%s' in %s()", identifier, __FUNCTION__ );
		return TT_ERR;
	}
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int default__api__otp_execute_apply(const char* identifier, const char* challenge, const char* response, char* key, size_t* key_size) {
	char		challenge_raw[TT_OTP_TEXT_MAX+1] = {0};
	size_t		challenge_raw_size = sizeof(challenge_raw);
	char		response_raw[TT_OTP_TEXT_MAX+1] = {0};
	size_t		response_raw_size = sizeof(response_raw);
	size_t		i = 0;
	unsigned short	crc = 0;
	tt_otp_t	otp = {0};

	TT_TRACE( "plugin/default", "%s(identifier='%s',challenge='%s',response='%s')", __FUNCTION__, identifier, challenge, response );
	if( identifier == NULL || challenge == NULL || response == NULL || key == NULL || key_size == NULL || *key_size == 0 ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameters in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	memset( key, '\0', *key_size );
	if( libtokentube_util_base32_decode( challenge, strnlen(challenge, TT_OTP_TEXT_MAX), challenge_raw, &challenge_raw_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "base32_decode() failed for identifier '%s' in %s()", identifier, __FUNCTION__ );
		return TT_ERR;
	}
	if( libtokentube_util_base32_decode( response, strnlen(response, TT_OTP_TEXT_MAX), response_raw, &response_raw_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "base32_decode() failed for identifier '%s' in %s()", identifier, __FUNCTION__ );
		return TT_ERR;
	}

	if( default__impl__otp_load( identifier, &otp ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "default__impl__otp_load() failed for '%s' in %s()", identifier, __FUNCTION__ );
		return TT_ERR;
	}
	if( otp.data_size == 0 || otp.data_size > sizeof(otp.data) || otp.data_size > *key_size ) {
		TT_LOG_ERROR( "plugin/default", "invalid otp.data_size for identifier '%s' in %s()", identifier, __FUNCTION__ );
		return TT_ERR;
	}
	if( otp.bits/8 == 0 || otp.bits/8 > sizeof(otp.data) || otp.bits/8 > *key_size ) {
		TT_LOG_ERROR( "plugin/default", "invalid otp.bits for identifier '%s' in %s()", identifier, __FUNCTION__ );
		return TT_ERR;
	}
	if( otp.bits/8 != (challenge_raw_size-2) || otp.bits/8 != (response_raw_size-2) ) {
		TT_LOG_ERROR( "plugin/default", "otp.bits=%d, challenge_raw_size=%d, response_raw_size=%d", otp.bits, challenge_raw_size-2, response_raw_size-2 );
		return TT_ERR;
	}
	*key_size = otp.data_size;
	for( i=0; i<*key_size; i++ ) {
		key[i] = challenge_raw[2+i%(otp.bits/8)] ^ response_raw[2+i%(otp.bits/8)] ^ otp.data[i];
	}
	for( i=0; i<*key_size; i++ ) {
		otp.data[i] = otp.data[i] ^ key[i];
	}
	otp.data_size = sizeof(otp.data);
	if( libtokentube_crypto_hash_impl( otp.hash, otp.data, otp.bits/8, otp.data, &otp.data_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_crypto_hash_impl() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	for( i=*key_size; i>0; i-- ) {
		otp.data[i-1] = otp.data[(i-1)%(otp.bits/8)] ^ key[i-1];
	}

	if( libtokentube_util_crc16( otp.data, otp.bits/8, &crc ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "API:crc16() failed for identifier '%s' in %s()", identifier, __FUNCTION__ );
		return TT_ERR;
	}
	if( (response_raw[0] & 0xff) != ((crc >> 0) & 0xff) || (response_raw[1] & 0xff) != ((crc >> 8) & 0xff) ) {
		TT_LOG_ERROR( "plugin/default", "crc(response)[%x%x] != hash(otp.data)[%x%x] in %s()", ((crc >> 0) & 0xff), ((crc >> 8) & 0xff), response_raw[0] & 0xff, response_raw[1] & 0xff, __FUNCTION__ );
		return TT_ERR;
	}
	otp.data_size = *key_size;
	if( default__impl__otp_save( identifier, &otp ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "default__impl__otp_save() failed for '%s' in %s()", identifier, __FUNCTION__ );
		return TT_ERR;
	}
	TT_DEBUG3( "plugin/default", "default__impl__otp_save() successful for '%s' in %s()", identifier, __FUNCTION__ );
	libtokentube_runtime_broadcast( TT_EVENT__OTP_APPLIED, identifier );
        return TT_OK;
}

