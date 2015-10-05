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
	CFG_STR("sys-id", NULL, CFGF_NODEFAULT),
	CFG_STR("key-id", NULL, CFGF_NODEFAULT),
	CFG_STR("key-data", NULL, CFGF_NODEFAULT),
	CFG_STR("otp-hash", NULL, CFGF_NODEFAULT),
	CFG_INT("otp-iter", 0, CFGF_NODEFAULT),
	CFG_INT("otp-bits", 0, CFGF_NODEFAULT),
	CFG_END()
};

static cfg_opt_t opt_helpdesk[] = {
	CFG_STR("api", "", CFGF_NONE),
	CFG_SEC("helpdesk", opt_helpdesk_data, CFGF_NONE),
	CFG_END()
};


int  default__impl__helpdesk_storage_load(const char* identifier, dflt_helpdesk_t* helpdesk) {
	char    buffer[DEFAULT__FILESIZE_MAX+1] = {0};
	size_t  buffer_size = sizeof(buffer);
	cfg_t*  cfg = NULL;

	TT_TRACE( "plugin/default", "%s(identifier='%s',helpdesk=%p)", __FUNCTION__, identifier, helpdesk );
	if( identifier == NULL || identifier[0] == '\0' || helpdesk == NULL ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameters in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( libtokentube_plugin__storage_load( TT_FILE__UHD, identifier, buffer, &buffer_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "API:storage_load failed in %s()", __FUNCTION__ );
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
	strncpy( helpdesk->sys_id, cfg_getstr( cfg, "helpdesk|sys-id" ), sizeof(helpdesk->sys_id)-1 );
	strncpy( helpdesk->key_id, cfg_getstr( cfg, "helpdesk|key-id" ), sizeof(helpdesk->key_id)-1 );
	strncpy( helpdesk->key_data, cfg_getstr( cfg, "helpdesk|key-data" ), sizeof(helpdesk->key_data)-1 );
	strncpy( helpdesk->otp_hash, cfg_getstr( cfg, "helpdesk|otp-hash" ), sizeof(helpdesk->otp_hash)-1 );
	helpdesk->otp_bits = cfg_getint( cfg, "helpdesk|otp-bits" );
	helpdesk->otp_iter = cfg_getint( cfg, "helpdesk|otp-iter" );
	cfg_free( cfg );
	if( strncmp( helpdesk->key_id, identifier, TT_IDENTIFIER_CHAR_MAX ) != 0 ) {
		TT_LOG_ERROR( "plugin/default", "API:storage_load() for identifier '%s' returned non-matching record in %s()", identifier, __FUNCTION__ );
		return TT_ERR;
	}
	if( helpdesk->otp_hash[0] == '\0' || helpdesk->otp_iter == 0 || helpdesk->otp_bits == 0 || helpdesk->key_data[0] == '\0' ) {
		TT_LOG_ERROR( "plugin/default", "error while parsing helpdesk file for identifier '%s'", identifier );
		return TT_ERR;
	}
	memcpy( buffer, helpdesk->key_data, strnlen( helpdesk->key_data, TT_IDENTIFIER_CHAR_MAX ) );
	buffer_size = strnlen( buffer, TT_IDENTIFIER_CHAR_MAX );
	helpdesk->key_data_size = sizeof(helpdesk->key_data);
	if( libtokentube_util_base64_decode( buffer, buffer_size, helpdesk->key_data, &helpdesk->key_data_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "base64_decode() failed for key-data for identifier '%s'", identifier );
		return TT_ERR;
	}
	if( helpdesk->key_data_size < TT_CIPHER_BITS_MIN/8 || helpdesk->key_data_size > TT_CIPHER_BITS_MAX/8 ) {
		TT_LOG_ERROR( "plugin/default", "data too small or too arge for identifier '%s'", identifier );
		return TT_ERR;
	}
	return TT_OK;
}


int  default__impl__helpdesk_storage_save(const char* identifier, const dflt_helpdesk_t* helpdesk) {
	char    buffer[DEFAULT__FILESIZE_MAX+1] = {0};
	size_t  buffer_size = sizeof(buffer);
	cfg_t*  cfg = NULL;

	TT_TRACE( "plugin/default", "%s(identifier='%s',helpdesk=%p)", __FUNCTION__, identifier, helpdesk );
	if( identifier == NULL || identifier[0] == '\0' || helpdesk == NULL || strncmp( identifier, helpdesk->key_id, TT_IDENTIFIER_CHAR_MAX ) != 0 ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameters in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	cfg = cfg_init( opt_helpdesk, CFGF_NONE );
	if( cfg == NULL ) {
		TT_LOG_ERROR( "plugin/default", "cfg_init() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	snprintf( buffer, buffer_size, "plugin/default (%d.%d.%d)", TT_VERSION_MAJOR, TT_VERSION_MINOR, TT_VERSION_PATCH );
	cfg_setstr( cfg, "api", buffer );
	cfg_setstr( cfg, "helpdesk|sys-id", helpdesk->sys_id );
	cfg_setstr( cfg, "helpdesk|key-id", helpdesk->key_id );
	cfg_setstr( cfg, "helpdesk|otp-hash", helpdesk->otp_hash );
	cfg_setint( cfg, "helpdesk|otp-iter", helpdesk->otp_iter );
	cfg_setint( cfg, "helpdesk|otp-bits", (int)helpdesk->otp_bits );
	memset( buffer,'\0', sizeof(buffer) );
	if( libtokentube_util_base64_encode( helpdesk->key_data, helpdesk->key_data_size, buffer, &buffer_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "API:base64_encode failed for identifier '%s' in %s()", identifier, __FUNCTION__ );
		cfg_free( cfg );
		return TT_ERR;
	}
	cfg_setstr( cfg, "helpdesk|key-data", buffer );
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


int  default__impl__helpdesk_storage_exists(const char* identifier, tt_status_t* status) {
	TT_TRACE( "plugin/default", "%s(identifier='%s',status=%p)", __FUNCTION__, identifier, status );
	if( identifier == NULL || identifier[0] == '\0' || status == NULL ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameters in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	return TT_OK;
}


int  default__impl__helpdesk_storage_delete(const char* identifier, tt_status_t* status) {
	TT_TRACE( "plugin/default", "%s(identifier='%s',status=%p)", __FUNCTION__, identifier, status );
	if( identifier == NULL || identifier[0] == '\0' || status == NULL ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameters in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	return TT_OK;
}

