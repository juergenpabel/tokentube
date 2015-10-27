#include <stdlib.h>
#include <stdio.h>
#include <libgen.h>
#include <string.h>
#include <unistd.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <sys/types.h>
#include "libtokentube.h"


static char* g_types[] = {
	"<UNDEFINED>",
	"CONFIG:PBA",
	"CONFIG:STANDARD",
	"KEY",
	"USER",
	"OTP",
	"UHD"
};


__attribute__ ((visibility ("hidden")))
int libtokentube_plugin__storage_load(tt_file_t type, const char* identifier, char* buffer, size_t* buffer_len) {
	tt_module_t*	module;
	size_t		i;

	TT_TRACE( "library/plugin", "%s(type=%d,identifier='%s')", __FUNCTION__, (int)type, identifier );
	if( type == TT_FILE__UNDEFINED || type >= TT_FILE__INVALID || identifier == NULL || buffer == NULL || buffer_len == NULL || *buffer_len == 0 ) {
		TT_LOG_ERROR( "library/plugin", "BUG: invalid invocation of API:storage.load()" );
		TT_DEBUG_SRC( "library/plugin", "invalid parameters" );
		return TT_ERR;
	}
	for( i=0; i<MAX_PLUGINS+1; i++ ) {
		module = g_modules[i];
		if( module != NULL && module->plugin != NULL ) {
			if( module->plugin->interface.api.storage.load != NULL ) {
				TT_DEBUG3( "library/plugin", "invoking storage.load('%s','%s') on plugin '%s'", g_types[type], identifier, module->name );
				switch( module->plugin->interface.api.storage.load(type, identifier, buffer, buffer_len) ) {
					case TT_OK:
						TT_DEBUG4( "library/plugin", "plugin '%s' successfully processed storage.load()", module->name );
						return TT_OK;
					case TT_IGN:
						TT_DEBUG4( "library/plugin", "plugin '%s' returned TT_IGN for storage.load()", module->name );
						break;
					default:
						TT_LOG_ERROR( "library/plugin", "plugin '%s' returned TT_ERR for storage.load()", module->name );
				}
			}
		}
	}
	TT_LOG_ERROR( "library/plugin", "no plugin handled storage.load(), returning TT_ERR" );
	return TT_ERR;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_plugin__storage_save(tt_file_t type, const char* identifier, const char* buffer, size_t buffer_len) {
	tt_module_t* module;
	size_t i;

	TT_TRACE( "library/plugin", "%s(type=%d,identifier='%s')", __FUNCTION__, (int)type, identifier );
	if( type == TT_FILE__UNDEFINED || type >= TT_FILE__INVALID || identifier == NULL || identifier[0] == '\0' || buffer == NULL || buffer_len == 0 ) {
		TT_LOG_ERROR( "library/plugin", "BUG: invalid invocation of API:storage.load()" );
		TT_DEBUG_SRC( "library/plugin", "invalid parameters" );
		return TT_ERR;
	}
	for( i=0; i<MAX_PLUGINS+1; i++ ) {
		module = g_modules[i];
		if( module != NULL && module->plugin != NULL ) {
			if( module->plugin->interface.api.storage.save != NULL ) {
				TT_DEBUG3( "library/plugin", "invoking storage.save('%s','%s') on plugin '%s'", g_types[type], identifier, module->name );
				switch( module->plugin->interface.api.storage.save(type, identifier, buffer, buffer_len) ) {
					case TT_OK:
						TT_DEBUG4( "library/plugin", "plugin '%s' successfully processed storage.save()", module->name );
						return TT_OK;
					case TT_IGN:
						TT_DEBUG4( "library/plugin", "plugin '%s' returned TT_IGN for storage.save()", module->name );
						break;
					default:
						TT_LOG_ERROR( "library/plugin", "plugin '%s' returned TT_ERR for storage.save()", module->name );
				}
			}
		}
	}
	TT_LOG_ERROR( "library/plugin", "no plugin handled storage.save(), returning TT_ERR" );
	return TT_ERR;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_plugin__storage_exists(tt_file_t type, const char* identifier, tt_status_t* status) {
	tt_module_t* module;
	size_t	i;

	TT_TRACE( "library/plugin", "%s(type=%d,identifier='%s')", __FUNCTION__, (int)type, identifier );
	if( type == TT_FILE__UNDEFINED || type >= TT_FILE__INVALID || identifier == NULL || identifier[0] == '\0' || status == NULL ) {
		TT_LOG_ERROR( "library/plugin", "BUG: invalid invocation of API:storage.exists()" );
		TT_DEBUG_SRC( "library/plugin", "invalid parameters" );
		return TT_ERR;
	}
	*status = TT_STATUS__UNDEFINED;
	for( i=0; i<MAX_PLUGINS+1; i++ ) {
		module = g_modules[i];
		if( module != NULL && module->plugin != NULL ) {
			if( module->plugin->interface.api.storage.exists != NULL ) {
				TT_DEBUG3( "library/plugin", "invoking storage.exists('%s','%s') on plugin '%s'", g_types[type], identifier, module->name );
				switch( module->plugin->interface.api.storage.exists(type, identifier, status) ) {
					case TT_OK:
						if( *status == TT_YES ) {
							TT_DEBUG4( "library/plugin", "plugin '%s' successfully processed storage.exists()", module->name );
							return TT_OK;
						} else {
							TT_DEBUG4( "library/plugin", "plugin '%s' returned TT_NO for storage.exists()", module->name );
						}
						break;
					case TT_IGN:
						TT_DEBUG4( "library/plugin", "plugin '%s' returned TT_IGN for storage.exists()", module->name );
						break;
					default:
						TT_LOG_ERROR( "library/plugin", "plugin '%s' returned TT_ERR for storage.exists()", module->name );
				}
			}
		}
	}
	if( *status == TT_NO ) {
		TT_DEBUG5( "library/plugin", "returning TT_NO for storage.exists()" );
		return TT_OK;
	}
	TT_LOG_ERROR( "library/plugin", "no plugin handled storage.exists(), returning TT_ERR" );
	return TT_ERR;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_plugin__storage_delete(tt_file_t type, const char* identifier, tt_status_t* status) {
	tt_module_t*	module;
	size_t		i;

	TT_TRACE( "library/plugin", "%s(type=%d,identifier='%s')", __FUNCTION__, (int)type, identifier );
	if( type == TT_FILE__UNDEFINED || type >= TT_FILE__INVALID || identifier == NULL || identifier[0] == '\0' || status == NULL ) {
		TT_LOG_ERROR( "library/plugin", "BUG: invalid invocation of API:storage.delete()" );
		TT_DEBUG_SRC( "library/plugin", "invalid parameters" );
		return TT_ERR;
	}
	*status = TT_STATUS__UNDEFINED;
	for( i=0; i<MAX_PLUGINS+1; i++ ) {
		module = g_modules[i];
		if( module != NULL && module->plugin != NULL ) {
			if( module->plugin->interface.api.storage.delete != NULL ) {
				TT_DEBUG3( "library/plugin", "invoking storage.delete('%s','%s') on plugin '%s'", g_types[type], identifier, module->name );
				switch( module->plugin->interface.api.storage.delete(type, identifier, status) ) {
					case TT_OK:
						if( *status == TT_YES ) {
							TT_DEBUG4( "library/plugin", "plugin '%s' successfully processed storage.delete()", module->name );
							return TT_OK;
						} else {
							TT_DEBUG4( "library/plugin", "plugin '%s' returned TT_NO for storage.delete()", module->name );
						}
						break;
					case TT_IGN:
						TT_DEBUG4( "library/plugin", "plugin '%s' returned TT_IGN for storage.delete()", module->name );
						break;
					default:
						TT_LOG_ERROR( "library/plugin", "plugin '%s' returned TT_ERR for storage.delete()", module->name );
				}
			}
		}
	}
	if( *status == TT_NO ) {
		TT_DEBUG5( "library/plugin", "returning TT_NO for storage.delete()", module->name );
		return TT_OK;
	}
	TT_LOG_ERROR( "library/plugin", "no plugin handled storage.delete(), returning TT_ERR" );
	return TT_ERR;
}

