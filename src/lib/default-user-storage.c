#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <confuse.h>
#include "libtokentube.h"


static cfg_opt_t opt_user_crypto[] = {
	CFG_STR("cipher",	  NULL, CFGF_NODEFAULT),
	CFG_STR("hash",		  NULL, CFGF_NODEFAULT),
	CFG_STR("kdf",		  NULL, CFGF_NODEFAULT),
	CFG_INT("kdf-iterations", 0, CFGF_NODEFAULT),
	CFG_END()
};

static cfg_opt_t opt_user_named_value[] = {
	CFG_STR("value", NULL, CFGF_NODEFAULT),
	CFG_END()
};

static cfg_opt_t opt_user_key[] = {
	CFG_STR("value",      NULL, CFGF_NODEFAULT),
	CFG_SEC("constraint", opt_user_named_value, CFGF_MULTI|CFGF_TITLE),
	CFG_SEC("parameter",  opt_user_named_value, CFGF_MULTI|CFGF_TITLE),
	CFG_END()
};

static cfg_opt_t opt_user_cred[] = {
	CFG_SEC("parameter", opt_user_named_value, CFGF_MULTI|CFGF_TITLE),
	CFG_END()
};

static cfg_opt_t opt_user[] = {
	CFG_SEC("crypto",      opt_user_crypto, CFGF_NONE),
	CFG_SEC("credentials", opt_user_cred, CFGF_NONE),
	CFG_SEC("key",         opt_user_key, CFGF_MULTI|CFGF_TITLE),
	CFG_END()
};

static cfg_opt_t opt[] = {
	CFG_STR("api",       NULL, CFGF_NODEFAULT),
	CFG_SEC("user",      opt_user, CFGF_NONE),
	CFG_STR("user-hmac", NULL, CFGF_NODEFAULT),
	CFG_END()
};


__attribute__ ((visibility ("hidden")))
int default__impl__user_storage_load(const char* username, dflt_user_t* user) {
	char    buffer[DEFAULT__FILESIZE_MAX+1] = {0};
	size_t  buffer_size = sizeof(buffer);
	cfg_t*  cfg = NULL;
	cfg_t*  section = NULL;
	size_t  key_offset = 0;
	size_t  size = 0;

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
	memset( user, '\0', sizeof(dflt_user_t) );
	strncpy( user->crypto.cipher, cfg_getstr( cfg, "user|crypto|cipher" ), sizeof(user->crypto.cipher)-1 );
	strncpy( user->crypto.hash, cfg_getstr( cfg, "user|crypto|hash" ), sizeof(user->crypto.hash)-1 );
	strncpy( user->crypto.kdf, cfg_getstr( cfg, "user|crypto|kdf" ), sizeof(user->crypto.kdf)-1 );
	user->crypto.kdf_iter = cfg_getint( cfg, "user|crypto|kdf-iterations" );
	section = cfg_getnsec( cfg, "user|key", key_offset );
	while( section != NULL && key_offset < DEFAULT__KEY_MAX ) {
		if( cfg_title( section ) == NULL ) {
			TT_LOG_ERROR( "plugin/default", "key without ID for username='%s' in %s()", username, __FUNCTION__ );
			cfg_free( cfg );
			return TT_ERR;
		}
		user->key[key_offset].data.key_size = sizeof(user->key[key_offset].data.key);
		if( libtokentube_util_hex_decode( cfg_title( section ), strnlen( cfg_title( section ), TT_IDENTIFIER_CHAR_MAX ), user->key[key_offset].data.key, &user->key[key_offset].data.key_size ) != TT_OK ) {
			TT_LOG_ERROR( "plugin/default", "libtokentube_util_base64_decode() failed for user|key in %s()", __FUNCTION__ );
			cfg_free( cfg );
			return TT_ERR;
		}
		user->key[key_offset].data.value_size = sizeof(user->key[key_offset].data.value);
		if( libtokentube_util_base64_decode( cfg_getstr( section, "value" ), 0, user->key[key_offset].data.value, &user->key[key_offset].data.value_size ) != TT_OK ) {
			TT_LOG_ERROR( "plugin/default", "libtokentube_util_base64_decode() failed for user|key in %s()", __FUNCTION__ );
			cfg_free( cfg );
			return TT_ERR;
		}
		key_offset++;
		section = cfg_getnsec( cfg, "user|key", key_offset );
	}
	size = sizeof(user->hmac.data);
	if( libtokentube_util_base64_decode( cfg_getstr( cfg, "user-hmac" ), 0, user->hmac.data, &size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_util_base64_decode() failed for user-hmac in %s()", __FUNCTION__ );
		cfg_free( cfg );
		return TT_ERR;
	}
	user->hmac.data_size = size;
	cfg_free( cfg );
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int default__impl__user_storage_save(const char* username, const dflt_user_t* user) {
	char	buffer[DEFAULT__FILESIZE_MAX+1] = {0};
	size_t	buffer_size = 0;
	cfg_t*	cfg = NULL;
	cfg_t*  section = NULL;
	cfg_opt_t*  option = NULL;
	cfg_value_t*  value = NULL;
	size_t  key_offset = 0;

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
	cfg_setstr( cfg, "user|crypto|cipher", user->crypto.cipher );
	cfg_setstr( cfg, "user|crypto|hash", user->crypto.hash );
	cfg_setstr( cfg, "user|crypto|kdf", user->crypto.kdf );
	cfg_setint( cfg, "user|crypto|kdf-iterations", user->crypto.kdf_iter );
	while( key_offset < DEFAULT__KEY_MAX ) {
		section = cfg_getsec( cfg, "user" );
		option = cfg_getopt( section, "key" );
		if( option == NULL ) {
			TT_LOG_ERROR( "plugin/default", "option NULL for user|key in %s()", __FUNCTION__ );
			cfg_free( cfg );
			return TT_ERR;
		}
		if( user->key[key_offset].data.key_size > 0 ) {
			buffer_size = sizeof(buffer);
			memset( buffer,'\0', sizeof(buffer) );
			if( libtokentube_util_hex_encode( user->key[key_offset].data.key, user->key[key_offset].data.key_size, buffer, &buffer_size ) != TT_OK ) {
				TT_LOG_ERROR( "plugin/default", "libtokentube_util_hex_encode() failed for uuid in %s()", __FUNCTION__ );
				cfg_free( cfg );
				return TT_ERR;
			}
			value = cfg_setopt( section, option, strndup( buffer, sizeof(buffer) ) );
			if( value == NULL ) {
				TT_LOG_ERROR( "plugin/default", "cfg_setopt() failed for user|key in %s()", __FUNCTION__ );
				cfg_free( cfg );
				return TT_ERR;
			}
			section = cfg_gettsec( cfg, "user|key", buffer );
			if( section == NULL ) {
				TT_LOG_ERROR( "plugin/default", "cfg_value_t without section for user|key in %s()", __FUNCTION__ );
				cfg_free( cfg );
				return TT_ERR;
			}
			buffer_size = sizeof(buffer);
			memset( buffer,'\0', sizeof(buffer) );
			if( libtokentube_util_base64_encode( user->key[key_offset].data.value, user->key[key_offset].data.value_size, buffer, &buffer_size ) != TT_OK ) {
				TT_LOG_ERROR( "plugin/default", "libtokentube_util_base64_encode() failed for user|key in %s()", __FUNCTION__ );
				cfg_free( cfg );
				return TT_ERR;
			}
			cfg_setstr( section, "value", buffer );
		}
		key_offset++;
	}
	buffer_size = sizeof(buffer);
	memset( buffer,'\0', buffer_size );
	if( libtokentube_util_base64_encode( user->hmac.data, user->hmac.data_size, buffer, &buffer_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_util_base64_encode() failed for user-hmac in %s()", __FUNCTION__ );
		cfg_free( cfg );
		return TT_ERR;
	}
	cfg_setstr( cfg, "user-hmac", buffer );
	buffer_size = sizeof(buffer);
	if( libtokentube_runtime_conf__serialize( cfg, buffer, &buffer_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_cfg_serialze() failed in %s()", __FUNCTION__ );
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

