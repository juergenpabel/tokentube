#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <confuse.h>
#include "libtokentube.h"


static cfg_opt_t opt_user[] = {
	CFG_STR("cipher",	NULL, CFGF_NODEFAULT),
	CFG_STR("hash",		NULL, CFGF_NODEFAULT),
	CFG_STR("kdf",		NULL, CFGF_NODEFAULT),
	CFG_INT("kdf-iterations", 0,  CFGF_NODEFAULT),
	CFG_STR("key",		NULL, CFGF_NODEFAULT),
	CFG_STR("verifier",	NULL, CFGF_NODEFAULT),
	CFG_END()
};

static cfg_opt_t opt[] = {
	CFG_STR("api",	NULL, CFGF_NODEFAULT),
	CFG_SEC("user",		opt_user, CFGF_NONE),
	CFG_END()
};


__attribute__ ((visibility ("hidden")))
int default__impl__user_storage_load(const char* username, tt_user_t* user) {
	char	buffer[DEFAULT__FILESIZE_MAX] = {0};
	size_t	buffer_size = sizeof(buffer);
	cfg_t*	cfg = NULL;

	TT_TRACE( "library/plugin", "%s(username='%s',user=%p)", __FUNCTION__, username, user );
	if( username == NULL || username[0] == '\0' || user == NULL ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( libtokentube_plugin__file_load( TT_FILE__USER, username, buffer, &buffer_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_plugin__file_load() failed for username='%s' in %s()", username, __FUNCTION__ );
		return TT_ERR;
	}
	cfg = cfg_init( opt, CFGF_NONE );
	if( cfg == NULL ) {
		TT_LOG_ERROR( "plugin/default", "cfg_init() failed in %s()", username, __FUNCTION__ );
		return TT_ERR;
	}
	if( cfg_parse_buf( cfg, buffer ) != 0 ) {
		TT_LOG_ERROR( "plugin/default", "parse error for username='%s' in %s()", username, __FUNCTION__ );
		cfg_free( cfg );
		return TT_ERR;
	}
	memset( user, '\0', sizeof(tt_user_t) );
	strncpy( user->cipher, cfg_getstr( cfg, "user|cipher" ), sizeof(user->cipher)-1 );
	strncpy( user->hash, cfg_getstr( cfg, "user|hash" ), sizeof(user->hash)-1 );
	strncpy( user->kdf, cfg_getstr( cfg, "user|kdf" ), sizeof(user->kdf)-1 );
	user->kdf_iter = cfg_getint( cfg, "user|kdf-iterations" );
	user->luks_data_len = sizeof(user->luks_data);
	if( libtokentube_util_base64_decode( cfg_getstr( cfg, "user|key" ), 0, user->luks_data, &user->luks_data_len ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_util_base64_decode() failed for user|key in %s()", __FUNCTION__ );
		cfg_free( cfg );
		return TT_ERR;
	}
	user->luks_vrfy_len = sizeof(user->luks_vrfy);
	if( libtokentube_util_base64_decode( cfg_getstr( cfg, "user|verifier" ), 0, user->luks_vrfy, &user->luks_vrfy_len ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_util_base64_decode() failed for user|verifier in %s()", __FUNCTION__ );
		cfg_free( cfg );
		return TT_ERR;
	}
	cfg_free( cfg );
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int default__impl__user_storage_save(const char* username, const tt_user_t* user) {
	char	buffer[DEFAULT__FILESIZE_MAX] = {0};
	cfg_t*	cfg = NULL;
	size_t	buffer_size = 0;

	TT_TRACE( "library/plugin", "%s(username='%s',user=%p)", __FUNCTION__, username, user );
	if( username == NULL || username[0] == '\0' || user == NULL ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	cfg = cfg_init( opt, CFGF_NONE );
	if( cfg == NULL ) {
		TT_LOG_ERROR( "plugin/default", "cfg_init() failed in %s()", username, __FUNCTION__ );
		return TT_ERR;
	}
	snprintf( buffer, sizeof(buffer), "plugin/default (%d.%d.%d)", TT_VERSION_MAJOR, TT_VERSION_MINOR, TT_VERSION_PATCH );
	cfg_setstr( cfg, "api", buffer );
	cfg_setstr( cfg, "user|cipher", user->cipher );
	cfg_setstr( cfg, "user|hash", user->hash );
	cfg_setstr( cfg, "user|kdf", user->kdf );
	cfg_setint( cfg, "user|kdf-iterations", user->kdf_iter );
	buffer_size = sizeof(buffer);
	memset( buffer,'\0', sizeof(buffer) );
	if( libtokentube_util_base64_encode( user->luks_data, user->luks_data_len, buffer, &buffer_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_util_base64_encode() failed for user|key in %s()", __FUNCTION__ );
		cfg_free( cfg );
		return TT_ERR;
	}
	cfg_setstr( cfg, "user|key", buffer );
	buffer_size = sizeof(buffer);
	memset( buffer,'\0', sizeof(buffer) );
	if( libtokentube_util_base64_encode( user->luks_vrfy, user->luks_vrfy_len, buffer, &buffer_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_util_base64_encode() failed for user|verifier in %s()", __FUNCTION__ );
		cfg_free( cfg );
		return TT_ERR;
	}
	cfg_setstr( cfg, "user|verifier", buffer );
	buffer_size = sizeof(buffer);
	if( libtokentube_cfg_print( cfg, buffer, &buffer_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_cfg_print() failed in %s()", __FUNCTION__ );
		cfg_free( cfg );
		return TT_ERR;
	}
	cfg_free( cfg );
	if( libtokentube_plugin__file_save( TT_FILE__USER, username, buffer, buffer_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_plugin__file_save() failed for username '%s' in %s()", username, __FUNCTION__ );
		return TT_ERR;
	}
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int default__impl__user_storage_delete(const char* username, tt_status_t* status) {
	TT_TRACE( "library/plugin", "%s(username='%s',status=%p)", __FUNCTION__, username, status );
	if( username == NULL || username[0] == '\0' || status == NULL ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	return libtokentube_plugin__file_delete( TT_FILE__USER, username, status );
}


__attribute__ ((visibility ("hidden")))
int default__impl__user_storage_exists(const char* username, tt_status_t* status) {
	TT_TRACE( "library/plugin", "%s(username='%s',status=%p)", __FUNCTION__, username, status );
	if( username == NULL || username[0] == '\0' || status == NULL ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	return libtokentube_plugin__file_exists( TT_FILE__USER, username, status );
}

