#include <stdlib.h>
#include <string.h>
#include <confuse.h>
#include "libtokentube.h"


static cfg_opt_t opt_otp[] = {
	CFG_STR("hash",	"", CFGF_NONE),
	CFG_INT("bits",	TT_UNINITIALIZED, CFGF_NONE),
	CFG_STR("data",	"", CFGF_NONE),
	CFG_END()
};

static cfg_opt_t opt[] = {
	CFG_STR("api",	"", CFGF_NONE),
	CFG_SEC("otp",	opt_otp, CFGF_NONE),
	CFG_END()
};


__attribute__ ((visibility ("hidden")))
int default__impl__otp_load(const char* identifier, tt_otp_t* otp) {
	char    buffer[DEFAULT__FILESIZE_MAX+1] = { 0 };
	size_t	buffer_size = sizeof(buffer);
	cfg_t*  cfg = NULL;

	TT_TRACE( "plugin/default", "%s(identifier='%s',otp=%p)", __FUNCTION__, identifier, otp );
	if( identifier == NULL || identifier[0] == '\0' || otp == NULL ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( libtokentube_plugin__file_load( TT_FILE__OTP, identifier, buffer, &buffer_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_plugin__file_load() failed for identifier='%s' in %s", identifier, __FUNCTION__ );
		return TT_ERR;
	}
	cfg = cfg_init( opt, CFGF_NONE );
	if( cfg == NULL ) {
		TT_LOG_ERROR( "plugin/default", "cfg_init() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( cfg_parse_buf( cfg, buffer ) != 0 ) {
		TT_LOG_ERROR( "plugin/default", "parse error for identifier='%s' in %s()", identifier, __FUNCTION__ );
		cfg_free( cfg );
		return TT_ERR;
	}
	strncpy( otp->hash, cfg_getstr( cfg, "otp|hash" ), sizeof(otp->hash)-1 );
	otp->bits = cfg_getint( cfg, "otp|bits" );
	otp->data_len =  sizeof(otp->data);
	if( libtokentube_util_base64_decode( cfg_getstr( cfg, "otp|data" ), 0, otp->data, &otp->data_len ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_util_base64_decode() failed for otp|data in %s()", __FUNCTION__ );
		cfg_free( cfg );
		return TT_ERR;
	}
	cfg_free( cfg );
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int default__impl__otp_save(const char* identifier, tt_otp_t* otp) {
	char    buffer[DEFAULT__FILESIZE_MAX+1] = { 0 };
	size_t  buffer_size = sizeof(buffer);
	cfg_t*	cfg = NULL;

	TT_TRACE( "plugin/default", "%s(identifier='%s',otp=%p)", __FUNCTION__, identifier, otp );
	if( identifier == NULL || identifier[0] == '\0' || otp == NULL ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	cfg = cfg_init( opt, CFGF_NONE );
	if( cfg == NULL ) {
		TT_LOG_ERROR( "plugin/default", "cfg_init() failed in %s()", identifier, __FUNCTION__ );
		return TT_ERR;
	}
	snprintf( buffer, sizeof(buffer), "plugin/default (%d.%d.%d)", TT_VERSION_MAJOR, TT_VERSION_MINOR, TT_VERSION_PATCH );
	cfg_setstr( cfg, "api", buffer );
	cfg_setstr( cfg, "otp|hash", otp->hash );
	cfg_setint( cfg, "otp|bits", otp->bits );
	if( libtokentube_util_base64_encode( otp->data, otp->data_len, buffer, &buffer_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_util_base64_encode() failed for otp|data in %s()", __FUNCTION__ );
		cfg_free( cfg );
		return TT_ERR;
	}
	cfg_setstr( cfg, "otp|data", buffer );
	buffer_size = sizeof(buffer);
	if( libtokentube_cfg_print( cfg, buffer, &buffer_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_cfg_print() failed in %s()", __FUNCTION__ );
		cfg_free( cfg );
		return TT_ERR;
	}
	cfg_free( cfg );
	if( libtokentube_plugin__file_save( TT_FILE__OTP, identifier, buffer, strnlen(buffer, sizeof(buffer)-1) ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_plugin__file_save() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	TT_DEBUG3( "plugin/default", "libtokentube_plugin__file_save() successful in %s()", __FUNCTION__ );
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int default__impl__otp_delete(const char* identifier) {
	tt_status_t	status = TT_STATUS__UNDEFINED;

	TT_TRACE( "plugin/default", "%s(identifier='%s')", __FUNCTION__, identifier );
	if( identifier == NULL || identifier[0] == '\0' ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( libtokentube_plugin__file_delete( TT_FILE__OTP, identifier, &status ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "api:file_delete() failed for '%s' in %s()", identifier, __FUNCTION__ );
		return TT_ERR;
	}
	if( status == TT_YES ) {
		libtokentube_runtime_broadcast( TT_EVENT__OTP_DELETED, identifier );
	}
	return TT_OK;
}

