#include <stdlib.h>
#include <stdio.h>
#include <libgen.h>
#include <string.h>
#include <unistd.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <sys/types.h>
#include "libtokentube.h"


__attribute__ ((visibility ("hidden")))
int libtokentube_plugin__runtime_get_sysid(char* sysid, size_t* sysid_size) {
	tt_module_t* module;
	size_t i;

	TT_TRACE( "library/plugin", "%s()", __FUNCTION__ );
	if( sysid == NULL || sysid_size == NULL || *sysid_size == 0 ) {
		TT_LOG_ERROR( "library/plugin", "BUG: invalid invocation of API:runtime.system.get_sysid()" );
		TT_DEBUG_SRC( "library/plugin", "invalid parameters" );
		return TT_ERR;
	}
	for( i=0; i<MAX_PLUGINS+1; i++ ) {
		module = g_modules[i];
		if( module != NULL && module->plugin != NULL ) {
			if( module->plugin->interface.api.runtime.system.get_sysid != NULL ) {
				TT_DEBUG3( "library/plugin", "invoking runtime.system.get_sysid() on plugin '%s'", module->name );
				switch( module->plugin->interface.api.runtime.system.get_sysid( sysid, sysid_size ) ) {
					case TT_OK:
						TT_DEBUG4( "library/plugin", "plugin '%s' successfully processed runtime.system.get_sysid()", module->name );
						return TT_OK;
					case TT_IGN:
						TT_DEBUG4( "library/plugin", "plugin '%s' returned TT_IGN for runtime.system.get_sysid()", module->name );;
						break;
					default:
						TT_LOG_ERROR( "library/plugin", "plugin '%s' returned TT_ERR for runtime.system.get_sysid()", module->name );
				}
			}
		}
	}
	TT_LOG_ERROR( "library/plugin", "no plugin handled runtime.system.get_sysid(), returning TT_ERR" );
	return TT_ERR;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_plugin__runtime_get_type(tt_runtime_t* result) {
	tt_module_t* module;
	size_t i;

	TT_TRACE( "library/plugin", "%s()", __FUNCTION__ );
	if( result == NULL ) {
		TT_LOG_ERROR( "library/plugin", "BUG: invalid invocation of API:runtime.system.get_type()" );
		TT_DEBUG_SRC( "library/plugin", "invalid parameters" );
		return TT_ERR;
	}
	for( i=0; i<MAX_PLUGINS+1; i++ ) {
		module = g_modules[i];
		if( module != NULL && module->plugin != NULL ) {
			if( module->plugin->interface.api.runtime.system.get_type != NULL ) {
				TT_DEBUG3( "library/plugin", "invoking runtime.system.get_type() on plugin '%s'", module->name );
				switch( module->plugin->interface.api.runtime.system.get_type(result) ) {	
					case TT_OK:
						TT_DEBUG4( "library/plugin", "plugin '%s' successfully processed runtime.system.get_type()", module->name );
						return TT_OK;
					case TT_IGN:
						TT_DEBUG4( "library/plugin", "plugin '%s' returned TT_IGN for runtime.system.get_type()", module->name );
						break;
					default:
						TT_LOG_ERROR( "library/plugin", "plugin '%s' returned TT_ERR for runtime.system.get_type()", module->name );
				}
			}
		}
	}
	TT_LOG_ERROR( "library/plugin", "no plugin handled runtime.system.get_type(), returning TT_ERR" );
	return TT_ERR;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_plugin__runtime_get_bootdevice(char* const buffer, const size_t buffer_len) {
	tt_module_t* module;
	size_t i;

	TT_TRACE( "library/plugin", "%s()", __FUNCTION__ );
	if( buffer == NULL || buffer_len == 0 ) {
		TT_LOG_ERROR( "library/plugin", "BUG: invalid invocation of API:runtime.system.get_bootdevice()" );
		TT_DEBUG_SRC( "library/plugin", "invalid parameters" );
		return TT_ERR;
	}
	for( i=0; i<MAX_PLUGINS+1; i++ ) {
		module = g_modules[i];
		if( module != NULL && module->plugin != NULL ) {
			if( module->plugin->interface.api.runtime.system.get_bootdevice != NULL ) {
				TT_DEBUG3( "library/plugin", "invoking runtime.system.get_bootdevice() on plugin '%s'", module->name );
				switch( module->plugin->interface.api.runtime.system.get_bootdevice(buffer, buffer_len) ) {
					case TT_OK:
						TT_DEBUG4( "library/plugin", "plugin '%s' successfully processed runtime.system.get_bootdevice()", module->name );
						return TT_OK;
					case TT_IGN:
						TT_DEBUG4( "library/plugin", "plugin '%s' returned TT_IGN for runtime.system.get_bootdevice()", module->name );
						break;
					default:
						TT_LOG_ERROR( "library/plugin", "plugin '%s' returned TT_ERR for runtime.system.get_bootdevice()", module->name );
				}
			}
		}
	}
	TT_LOG_ERROR( "library/plugin", "no plugin handled runtime.system.get_bootdevice(), returning TT_ERR" );
	return TT_ERR;
}

