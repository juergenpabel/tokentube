#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <confuse.h>
#include <gcrypt.h>
#include "libtokentube.h"


static cfg_t*	g_configuration = NULL;
static char*	g_configuration_filename = NULL;


static cfg_opt_t opt_tokentube_runtime_debug[] = {
	CFG_INT("level", 0, CFGF_NONE),
	CFG_END()
};

static cfg_opt_t opt_tokentube_runtime_log[] = {
	CFG_STR("level", "", CFGF_NONE),
	CFG_END()
};

static cfg_opt_t opt_tokentube_runtime[] = {
	CFG_SEC("log", opt_tokentube_runtime_log, CFGF_NONE),
	CFG_SEC("debug", opt_tokentube_runtime_debug, CFGF_NONE),
	CFG_END()
};

static cfg_opt_t opt_tokentube_crypto[] = {
	CFG_STR("cipher", "", CFGF_NONE),
	CFG_STR("hash",   "", CFGF_NONE),
	CFG_STR("kdf",    "", CFGF_NONE),
	CFG_INT("kdf-iterations", TT_UNINITIALIZED, CFGF_NONE),
	CFG_END()
};

static cfg_opt_t opt_tokentube_storage_files[] = {
	CFG_STR("directory",  "", CFGF_NONE),
	CFG_STR("filename-hash", "", CFGF_NONE),
	CFG_STR("owner", "", CFGF_NONE),
	CFG_STR("group", "", CFGF_NONE),
	CFG_STR("permission",  "", CFGF_NONE),
	CFG_END()
};

static cfg_opt_t opt_tokentube_storage[] = {
	CFG_SEC("user-files", opt_tokentube_storage_files, CFGF_NONE),
	CFG_SEC("otp-files", opt_tokentube_storage_files, CFGF_NONE),
	CFG_SEC("key-files", opt_tokentube_storage_files, CFGF_NONE),
	CFG_SEC("helpdesk-files", opt_tokentube_storage_files, CFGF_NONE),
	CFG_END()
};

static cfg_opt_t opt_tokentube_user_autoenrollment[] = {
        CFG_INT("uid-minimum", TT_UNINITIALIZED, CFGF_NONE),
        CFG_INT("uid-maximum", TT_UNINITIALIZED, CFGF_NONE),
        CFG_INT("gid-minimum", TT_UNINITIALIZED, CFGF_NONE),
        CFG_INT("gid-maximum", TT_UNINITIALIZED, CFGF_NONE),
        CFG_STR_LIST("groups", "", CFGF_NONE),
        CFG_END()
};

static cfg_opt_t opt_tokentube_user[] = {
        CFG_SEC("auto-enrollment", opt_tokentube_user_autoenrollment, CFGF_NONE),
        CFG_END()
};

static cfg_opt_t opt_tokentube_otp[] = {
	CFG_INT("bits", TT_UNINITIALIZED, CFGF_NONE),
	CFG_END()
};

static cfg_opt_t opt_tokentube_plugin_disable[] = {
	CFG_STR_LIST("api", "", CFGF_NONE),
	CFG_STR_LIST("event", "", CFGF_NONE),
	CFG_END()
};

static cfg_opt_t opt_tokentube_plugin[] = {
	CFG_STR("library", "", CFGF_NONE),
	CFG_STR("config",  "", CFGF_NONE),
	CFG_SEC("disable", opt_tokentube_plugin_disable, CFGF_NONE),
	CFG_END()
};

static cfg_opt_t opt_tokentube_plugins[] = {
	CFG_STR_LIST("enabled", "", CFGF_NONE),
	CFG_SEC("plugin", opt_tokentube_plugin, CFGF_TITLE|CFGF_MULTI),
	CFG_END()
};

static cfg_opt_t opt_tokentube[] = {
	CFG_FUNC("include", libtokentube_runtime_conf__include),
	CFG_SEC("runtime", opt_tokentube_runtime, CFGF_NONE),
        CFG_SEC("user",    opt_tokentube_user,    CFGF_NONE),
        CFG_SEC("otp",     opt_tokentube_otp,     CFGF_NONE),
	CFG_SEC("crypto",  opt_tokentube_crypto,  CFGF_NONE),
	CFG_SEC("storage", opt_tokentube_storage, CFGF_NONE),
	CFG_SEC("plugins", opt_tokentube_plugins, CFGF_NONE),
	CFG_END()
};


__attribute__ ((visibility ("hidden")))
int libtokentube_conf_configure(const char* filename) {
	char	buffer[TT_CONFIG_MAX] = {0};
	size_t	buffer_size = sizeof(buffer);
	int	err = TT_OK;

	if( filename == NULL || filename[0] == '\0' ) {
		filename = TT_FILENAME__TOKENTUBE_CONF;
	}
	g_configuration_filename = strndup( filename, FILENAME_MAX+1 );
	if( libtokentube_plugin__storage_load( TT_FILE__CONFIG_STANDARD, g_configuration_filename, buffer, &buffer_size ) != TT_OK ) {
		TT_LOG_FATAL( "library/runtime", "could not load configuration file '%s'", g_configuration_filename );
		free( g_configuration_filename );
		g_configuration_filename = NULL;
		return TT_IGN;
	}
	g_configuration = cfg_init( opt_tokentube, CFGF_NONE );
	cfg_set_error_function( g_configuration, libtokentube_runtime_conf__log_error );
	err = cfg_parse_buf( g_configuration, buffer );
	if( err != 0 ) {
		TT_LOG_FATAL( "library/runtime", "syntax error in configuration file '%s'", g_configuration_filename );
		return TT_ERR;
	}
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_conf_finalize() {
	if( g_configuration != NULL ) {
		cfg_free( g_configuration );
		g_configuration = NULL;
	}
	if( g_configuration_filename != NULL ) {
		free( g_configuration_filename );
		g_configuration_filename = NULL;
	}
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_conf_serialize(char* buffer, size_t* buffer_size) {
	return libtokentube_runtime_conf__serialize( g_configuration, buffer, buffer_size );
}


__attribute__ ((visibility ("hidden")))
int libtokentube_conf_get_filename(char* buffer, size_t* buffer_size) {
	if( g_configuration_filename == NULL || buffer == NULL ||  buffer_size == NULL ) {
		TT_LOG_ERROR( "library/runtime", "internal error in %s at %d", __FUNCTION__, __LINE__ );
		return TT_ERR;
	}
	if( *buffer_size <= strnlen( g_configuration_filename, FILENAME_MAX+1 ) ) {
		TT_LOG_ERROR( "library/runtime", "internal error in %s at %d", __FUNCTION__, __LINE__ );
		return TT_ERR;
	}
	memset( buffer, '\0', *buffer_size );
	strncpy( buffer, g_configuration_filename, (*buffer_size)-1 );
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_conf_read_size(const char* name, size_t* value) {
	TT_TRACE( "library/runtime", "%s(name='%s')", __FUNCTION__, name );
	if( name == NULL || name[0] == '\0' || value == NULL ) {
		TT_LOG_ERROR( "library/runtime", "internal error in %s at %d", __FUNCTION__, __LINE__ );
		return TT_ERR;
	}
	*value = (size_t)cfg_getint( g_configuration, name );
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_conf_read_int(const char* name, int* value) {
	TT_TRACE( "library/runtime", "%s(name='%s')", __FUNCTION__, name );
	if( name == NULL || name[0] == '\0' || value == NULL ) {
		TT_LOG_ERROR( "library/runtime", "internal error in %s at %d", __FUNCTION__, __LINE__ );
		return TT_ERR;
	}
	*value = cfg_getint( g_configuration, name );
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_conf_read_bool(const char* name, int* value) {
	TT_TRACE( "library/runtime", "%s(name='%s')", __FUNCTION__, name );
	if( name == NULL || name[0] == '\0' || value == NULL ) {
		TT_LOG_ERROR( "library/runtime", "internal error in %s at %d", __FUNCTION__, __LINE__ );
		return TT_ERR;
	}
	*value = cfg_getbool( g_configuration, name );
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_conf_read_str(const char* name, char* value, size_t* value_size) {
	char*	data = NULL;

	TT_TRACE( "library/runtime", "%s(name='%s')", __FUNCTION__, name );
	if( name == NULL || name[0] == '\0' || value == NULL || value_size == NULL || *value_size == 0 ) {
		TT_LOG_ERROR( "library/runtime", "internal error in %s at %d", __FUNCTION__, __LINE__ );
		return TT_ERR;
	}
	data = cfg_getstr( g_configuration, name );
	if( data == NULL ) {
		TT_LOG_ERROR( "library/runtime", "internal error in %s at %d", __FUNCTION__, __LINE__ );
		return TT_ERR;
	}
	if( *value_size <= strnlen( data, *value_size ) ) {
		TT_LOG_ERROR( "library/runtime", "internal error in %s at %d", __FUNCTION__, __LINE__ );
		return TT_ERR;
	}
	strncpy( value, data, *value_size );
	*value_size = strnlen( value, *value_size );
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_conf_read_list(const char* name, size_t index, char* value, size_t* value_size) {
	char*	data = NULL;

	TT_TRACE( "library/runtime", "%s(name='%s',index=%zd)", __FUNCTION__, name, index );
	if( name == NULL || name[0] == '\0' || value == NULL || value_size == NULL || *value_size == 0 ) {
		TT_LOG_ERROR( "library/runtime", "internal error in %s at %d", __FUNCTION__, __LINE__ );
		return TT_ERR;
	}
	if( index == cfg_size( g_configuration, name ) ) {
		*value_size = 0;
		return TT_OK;
	}
	if( index > cfg_size( g_configuration, name ) ) {
		TT_LOG_ERROR( "library/runtime", "internal error in %s at %d", __FUNCTION__, __LINE__ );
		return TT_ERR;
	}
	data = cfg_getnstr( g_configuration, name, index );
	if( data == NULL ) {
		TT_LOG_ERROR( "library/runtime", "internal error in %s at %d", __FUNCTION__, __LINE__ );
		return TT_ERR;
	}
	if( *value_size <= strnlen( data, *value_size ) ) {
		TT_LOG_ERROR( "library/runtime", "internal error in %s at %d", __FUNCTION__, __LINE__ );
		return TT_ERR;
	}
	memset( value, '\0', *value_size );
	strncpy( value, data, *value_size-1 );
	*value_size = strnlen( value, *value_size );
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_conf_read_plugin_library(const char* plugin, char* value, size_t* value_size) {
	cfg_t*	section = NULL;
	char*	data = NULL;

	TT_TRACE( "library/runtime", "%s(plugin='%s',index=%zd)", __FUNCTION__, plugin, index );
	if( plugin == NULL || plugin[0] == '\0' || value == NULL || value_size == NULL || *value_size == 0 ) {
		TT_LOG_ERROR( "library/runtime", "internal error in %s at %d", __FUNCTION__, __LINE__ );
		return TT_ERR;
	}
	section = cfg_getsec( g_configuration, "plugins" );
	if( section == NULL ) {
		TT_LOG_ERROR( "library/runtime", "section 'plugins' not configured in '%s'", g_configuration_filename );
		return TT_ERR;
	}
	section = cfg_gettsec( section, "plugin", plugin );
	if(section == NULL) {
		TT_LOG_ERROR( "library/runtime", "section 'plugins|plugin' not configured in '%s'", g_configuration_filename );
		return TT_ERR;
	}
	data = cfg_getstr( section, "library" );
	if( data == NULL ) {
		TT_LOG_ERROR( "library/runtime", "value 'plugins|plugin|library' not configured in '%s'", g_configuration_filename );
		return TT_ERR;
	}
	if( *value_size <= strnlen( data, *value_size ) ) {
		TT_LOG_ERROR( "library/runtime", "internal error in %s at %d", __FUNCTION__, __LINE__ );
		return TT_ERR;
	}
	strncpy( value, data, *value_size );
	*value_size = strnlen( value, *value_size );
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_conf_read_plugin_config(const char* plugin, char* value, size_t* value_size) {
	cfg_t*	section = NULL;
	char*	data = NULL;

	TT_TRACE( "library/runtime", "%s(plugin='%s',index=%zd)", __FUNCTION__, plugin, index );
	if( plugin == NULL || plugin[0] == '\0' || value == NULL || value_size == NULL || *value_size == 0 ) {
		TT_LOG_ERROR( "library/runtime", "internal error in %s at %d", __FUNCTION__, __LINE__ );
		return TT_ERR;
	}
	section = cfg_getsec( g_configuration, "plugins" );
	if( section == NULL ) {
		TT_LOG_ERROR( "library/runtime", "section 'plugins' not configured in '%s'", g_configuration_filename );
		return TT_ERR;
	}
	section = cfg_gettsec( section, "plugin", plugin );
	if(section == NULL) {
		TT_LOG_ERROR( "library/runtime", "section 'plugins|plugin' not configured in '%s'", g_configuration_filename );
		return TT_ERR;
	}
	data = cfg_getstr( section, "config" );
	if( data == NULL ) {
		TT_LOG_ERROR( "library/runtime", "value 'plugins|plugin|library' not configured in '%s'", g_configuration_filename );
		return TT_ERR;
	}
	if( *value_size <= strnlen( data, *value_size ) ) {
		TT_LOG_ERROR( "library/runtime", "internal error in %s at %d", __FUNCTION__, __LINE__ );
		return TT_ERR;
	}
	strncpy( value, data, *value_size );
	*value_size = strnlen( value, *value_size );
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_conf_read_plugin_filter_api(const char* plugin, size_t index, char* value, size_t* value_size) {
	cfg_t*	section = NULL;
	char*	data = NULL;

	TT_TRACE( "library/runtime", "%s(plugin='%s',index=%zd)", __FUNCTION__, plugin, index );
	if( plugin == NULL || plugin[0] == '\0' || value == NULL || value_size == NULL || *value_size == 0 ) {
		TT_LOG_ERROR( "library/runtime", "internal error in %s at %d", __FUNCTION__, __LINE__ );
		return TT_ERR;
	}
	section = cfg_getsec( g_configuration, "plugins" );
	if( section == NULL ) {
		TT_LOG_ERROR( "library/runtime", "section 'plugins' not configured in '%s'", g_configuration_filename );
		return TT_ERR;
	}
	section = cfg_gettsec( section, "plugin", plugin );
	if(section == NULL) {
		TT_LOG_ERROR( "library/runtime", "section 'plugins|plugin' not configured in '%s'", g_configuration_filename );
		return TT_ERR;
	}
	section = cfg_getsec( section, "disable" );
	if(section == NULL) {
		TT_DEBUG2( "library/runtime", "section 'plugins|plugin|disable' not configured in '%s'", g_configuration_filename );
		*value_size = 0;
		return TT_OK;
	}
	if( index == cfg_size( section, "api" ) ) {
		*value_size = 0;
		return TT_OK;
	}
	if( index > cfg_size( section, "api" ) ) {
		TT_LOG_ERROR( "library/runtime", "internal error in %s at %d", __FUNCTION__, __LINE__ );
		return TT_ERR;
	}
	data = cfg_getnstr( section, "api", index );
	if( data == NULL ) {
		TT_DEBUG2( "library/runtime", "section 'plugins|plugin|disable|api' not configured in '%s'", g_configuration_filename );
		*value_size = 0;
		return TT_OK;
	}
	if( *value_size <= strnlen( data, *value_size ) ) {
		TT_LOG_ERROR( "library/runtime", "internal error in %s at %d", __FUNCTION__, __LINE__ );
		return TT_ERR;
	}
	strncpy( value, data, *value_size );
	*value_size = strnlen( value, *value_size );
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_conf_read_plugin_filter_event(const char* plugin, size_t index, char* value, size_t* value_size) {
	cfg_t*	section = NULL;
	char*	data = NULL;

	TT_TRACE( "library/runtime", "%s(plugin='%s',index=%zd)", __FUNCTION__, plugin, index );
	if( plugin == NULL || plugin[0] == '\0' || value == NULL || value_size == NULL || *value_size == 0 ) {
		TT_LOG_ERROR( "library/runtime", "internal error in %s at %d", __FUNCTION__, __LINE__ );
		return TT_ERR;
	}
	section = cfg_getsec( g_configuration, "plugins" );
	if( section == NULL ) {
		TT_LOG_ERROR( "library/runtime", "section 'plugins' not configured in '%s'", g_configuration_filename );
		return TT_ERR;
	}
	section = cfg_gettsec( section, "plugin", plugin );
	if(section == NULL) {
		TT_LOG_ERROR( "library/runtime", "section 'plugins|plugin' not configured in '%s'", g_configuration_filename );
		return TT_ERR;
	}
	section = cfg_getsec( section, "disable" );
	if(section == NULL) {
		TT_DEBUG2( "library/runtime", "section 'plugins|plugin|disable' not configured in '%s'", g_configuration_filename );
		*value_size = 0;
		return TT_OK;
	}
	if( index == cfg_size( section, "event" ) ) {
		*value_size = 0;
		return TT_OK;
	}
	if( index > cfg_size( section, "event" ) ) {
		TT_LOG_ERROR( "library/runtime", "internal error in %s at %d", __FUNCTION__, __LINE__ );
		return TT_ERR;
	}
	data = cfg_getnstr( section, "event", index );
	if( data == NULL ) {
		TT_DEBUG2( "library/runtime", "section 'plugins|plugin|disable|event' not configured in '%s'", g_configuration_filename );
		*value_size = 0;
		return TT_OK;
	}
	if( *value_size <= strnlen( data, *value_size ) ) {
		TT_LOG_ERROR( "library/runtime", "internal error in %s at %d", __FUNCTION__, __LINE__ );
		return TT_ERR;
	}
	strncpy( value, data, *value_size );
	*value_size = strnlen( value, *value_size );
	return TT_OK;
}

