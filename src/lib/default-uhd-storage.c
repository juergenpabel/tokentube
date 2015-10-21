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


static cfg_opt_t opt_uhd_data[] = {
	CFG_STR("sys-id", NULL, CFGF_NODEFAULT),
	CFG_STR("key-id", NULL, CFGF_NODEFAULT),
	CFG_STR("key-data", NULL, CFGF_NODEFAULT),
	CFG_STR("otp-hash", NULL, CFGF_NODEFAULT),
	CFG_INT("otp-iter", 0, CFGF_NODEFAULT),
	CFG_INT("otp-bits", 0, CFGF_NODEFAULT),
	CFG_END()
};

static cfg_opt_t opt_uhd[] = {
	CFG_STR("api", "", CFGF_NONE),
	CFG_SEC("uhd", opt_uhd_data, CFGF_NONE),
	CFG_END()
};


__attribute__ ((visibility ("hidden")))
int  default__impl__uhd_storage_load(const char* identifier, dflt_uhd_t* uhd) {
	char    buffer[DEFAULT__FILESIZE_MAX+1] = {0};
	size_t  buffer_size = sizeof(buffer);
	cfg_t*  cfg = NULL;

	TT_TRACE( "plugin/default", "%s(identifier='%s',uhd=%p)", __FUNCTION__, identifier, uhd );
	if( identifier == NULL || identifier[0] == '\0' || uhd == NULL ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameters in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( libtokentube_plugin__storage_load( TT_FILE__UHD, identifier, buffer, &buffer_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "API:storage_load failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	cfg = cfg_init( opt_uhd, CFGF_NONE );
	if( cfg == NULL ) {
		TT_LOG_ERROR( "plugin/default", "cfg_init() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( cfg_parse_buf( cfg, buffer ) != CFG_SUCCESS ) {
		TT_LOG_ERROR( "plugin/default", "cfg_parse() failed for identifier '%s' in %s()", identifier, __FUNCTION__ );
		cfg_free( cfg );
		return TT_ERR;
	}
	strncpy( uhd->sys_id, cfg_getstr( cfg, "uhd|sys-id" ), sizeof(uhd->sys_id)-1 );
	strncpy( uhd->key_id, cfg_getstr( cfg, "uhd|key-id" ), sizeof(uhd->key_id)-1 );
	strncpy( uhd->key_data, cfg_getstr( cfg, "uhd|key-data" ), sizeof(uhd->key_data)-1 );
	strncpy( uhd->otp_hash, cfg_getstr( cfg, "uhd|otp-hash" ), sizeof(uhd->otp_hash)-1 );
	uhd->otp_bits = cfg_getint( cfg, "uhd|otp-bits" );
	uhd->otp_iter = cfg_getint( cfg, "uhd|otp-iter" );
	cfg_free( cfg );
	if( strncmp( uhd->key_id, identifier, TT_IDENTIFIER_CHAR_MAX ) != 0 ) {
		TT_LOG_ERROR( "plugin/default", "API:storage_load() for identifier '%s' returned non-matching record in %s()", identifier, __FUNCTION__ );
		return TT_ERR;
	}
	if( uhd->otp_hash[0] == '\0' || uhd->otp_iter == 0 || uhd->otp_bits == 0 || uhd->key_data[0] == '\0' ) {
		TT_LOG_ERROR( "plugin/default", "error while parsing uhd file for identifier '%s'", identifier );
		return TT_ERR;
	}
	memcpy( buffer, uhd->key_data, strnlen( uhd->key_data, TT_IDENTIFIER_CHAR_MAX ) );
	buffer_size = strnlen( buffer, TT_IDENTIFIER_CHAR_MAX );
	uhd->key_data_size = sizeof(uhd->key_data);
	if( libtokentube_util_base64_decode( buffer, buffer_size, uhd->key_data, &uhd->key_data_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "base64_decode() failed for key-data for identifier '%s'", identifier );
		return TT_ERR;
	}
	if( uhd->key_data_size < TT_CIPHER_BITS_MIN/8 || uhd->key_data_size > TT_CIPHER_BITS_MAX/8 ) {
		TT_LOG_ERROR( "plugin/default", "data too small or too arge for identifier '%s'", identifier );
		return TT_ERR;
	}
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int  default__impl__uhd_storage_save(const char* identifier, const dflt_uhd_t* uhd) {
	char    buffer[DEFAULT__FILESIZE_MAX+1] = {0};
	size_t  buffer_size = sizeof(buffer);
	cfg_t*  cfg = NULL;

	TT_TRACE( "plugin/default", "%s(identifier='%s',uhd=%p)", __FUNCTION__, identifier, uhd );
	if( identifier == NULL || identifier[0] == '\0' || uhd == NULL || strncmp( identifier, uhd->key_id, TT_IDENTIFIER_CHAR_MAX ) != 0 ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameters in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	cfg = cfg_init( opt_uhd, CFGF_NONE );
	if( cfg == NULL ) {
		TT_LOG_ERROR( "plugin/default", "cfg_init() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	snprintf( buffer, buffer_size, "plugin/default (%d.%d.%d)", TT_VERSION_MAJOR, TT_VERSION_MINOR, TT_VERSION_PATCH );
	cfg_setstr( cfg, "api", buffer );
	cfg_setstr( cfg, "uhd|sys-id", uhd->sys_id );
	cfg_setstr( cfg, "uhd|key-id", uhd->key_id );
	cfg_setstr( cfg, "uhd|otp-hash", uhd->otp_hash );
	cfg_setint( cfg, "uhd|otp-iter", uhd->otp_iter );
	cfg_setint( cfg, "uhd|otp-bits", (int)uhd->otp_bits );
	memset( buffer,'\0', sizeof(buffer) );
	if( libtokentube_util_base64_encode( uhd->key_data, uhd->key_data_size, buffer, &buffer_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "API:base64_encode failed for identifier '%s' in %s()", identifier, __FUNCTION__ );
		cfg_free( cfg );
		return TT_ERR;
	}
	cfg_setstr( cfg, "uhd|key-data", buffer );
	buffer_size = sizeof(buffer);
	memset( buffer,'\0', buffer_size );
	if( libtokentube_runtime_conf__serialize( cfg, buffer, &buffer_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_runtime_conf__serialize() failed in %s()", __FUNCTION__ );
		cfg_free( cfg );
		return TT_ERR;
	}
	if( libtokentube_plugin__storage_save( TT_FILE__UHD, identifier, buffer, buffer_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "API:storage_save failed in %s()", __FUNCTION__ );
		cfg_free( cfg );
		return TT_ERR;
	}
	cfg_free( cfg );
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int  default__impl__uhd_storage_exists(const char* identifier, tt_status_t* status) {
	TT_TRACE( "plugin/default", "%s(identifier='%s',status=%p)", __FUNCTION__, identifier, status );
	if( identifier == NULL || identifier[0] == '\0' || status == NULL ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameters in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int  default__impl__uhd_storage_delete(const char* identifier, tt_status_t* status) {
	TT_TRACE( "plugin/default", "%s(identifier='%s',status=%p)", __FUNCTION__, identifier, status );
	if( identifier == NULL || identifier[0] == '\0' || status == NULL ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameters in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	return TT_OK;
}

