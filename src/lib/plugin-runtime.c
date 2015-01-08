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
		TT_LOG_ERROR( "library/plugin", "invalid parameters in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	TT_DEBUG3( "library/plugin", "invoking 'runtime_get_sysid' handlers" );
	for( i=0; i<MAX_PLUGINS+1; i++ ) {
		module = g_modules[i];
		if( module != NULL && module->plugin != NULL ) {
			TT_DEBUG5( "library/plugin", "checking 'runtime_get_sysid' for plugin '%s'", module->name );
			if( module->plugin->interface.api.runtime.get_sysid != NULL ) {
				TT_DEBUG4( "library/plugin", "invoking 'runtime_get_sysid' for plugin '%s'", module->name );
				switch( module->plugin->interface.api.runtime.get_sysid( sysid, sysid_size ) ) {
					case TT_OK:
						TT_DEBUG4( "library/plugin", "plugin '%s' successfully handled 'runtime_get_sysid'", module->name );
						return TT_OK;
					case TT_IGN:
						TT_DEBUG5( "library/plugin", "plugin '%s' ignored 'runtime_get_sysid'", module->name );;
						break;
					default:
						TT_LOG_ERROR( "library/plugin", "plugin '%s' returned error for 'runtime_get_sysid'", module->name );
				}
			}
		}
	}
	TT_LOG_ERROR( "library/plugin", "no plugin handled 'runtime_get_sysid', returning TT_ERR" );
	return TT_ERR;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_plugin__runtime_get_type(tt_runtime_t* result) {
	tt_module_t* module;
	size_t i;

	TT_TRACE( "library/plugin", "%s()", __FUNCTION__ );
	if( result == NULL ) {
		TT_LOG_ERROR( "library/plugin", "invalid parameters in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	TT_DEBUG3( "library/plugin", "invoking 'runtime_get_type' handlers" );
	for( i=0; i<MAX_PLUGINS+1; i++ ) {
		module = g_modules[i];
		if( module != NULL && module->plugin != NULL ) {
			TT_DEBUG5( "library/plugin", "checking 'runtime_get_type' for plugin '%s'", module->name );
			if( module->plugin->interface.api.runtime.get_type != NULL ) {
				TT_DEBUG4( "library/plugin", "invoking 'runtime_get_type' for plugin '%s'", module->name );
				switch( module->plugin->interface.api.runtime.get_type(result) ) {	
					case TT_OK:
						TT_DEBUG4( "library/plugin", "plugin '%s' successfully handled 'runtime_get_type'", module->name );
						return TT_OK;
					case TT_IGN:
						TT_DEBUG5( "library/plugin", "plugin '%s' ignored 'runtime_get_type'", module->name );
						break;
					default:
						TT_LOG_ERROR( "library/plugin", "plugin '%s' returned error for 'runtime_get_type'", module->name );
				}
			}
		}
	}
	TT_LOG_ERROR( "library/plugin", "no plugin handled 'runtime_get_type', returning TT_ERR" );
	return TT_ERR;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_plugin__runtime_get_bootdevice(char* const buffer, const size_t buffer_len) {
	tt_module_t* module;
	size_t i;

	TT_TRACE( "library/plugin", "%s()", __FUNCTION__ );
	if( buffer == NULL || buffer_len == 0 ) {
		TT_LOG_ERROR( "library/plugin", "invalid parameters in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	TT_DEBUG3( "library/plugin", "invoking 'runtime_get_bootdevice' handlers" );
	for( i=0; i<MAX_PLUGINS+1; i++ ) {
		module = g_modules[i];
		if( module != NULL && module->plugin != NULL ) {
			TT_DEBUG5( "library/plugin", "checking 'runtime_get_bootdevice' for plugin '%s'", module->name );
			if( module->plugin->interface.api.runtime.get_bootdevice != NULL ) {
				TT_DEBUG4( "library/plugin", "invoking 'runtime_get_bootdevice' for plugin '%s'", module->name );
				switch( module->plugin->interface.api.runtime.get_bootdevice(buffer, buffer_len) ) {
					case TT_OK:
						TT_DEBUG4( "library/plugin", "plugin '%s' successfully handled 'runtime_get_bootdevice'", module->name );
						return TT_OK;
					case TT_IGN:
						TT_DEBUG5( "library/plugin", "plugin '%s' ignored 'runtime_get_bootdevice'", module->name );
						break;
					default:
						TT_LOG_ERROR( "library/plugin", "plugin '%s' returned error for 'runtime_get_bootdevice'", module->name );
				}
			}
		}
	}
	TT_LOG_ERROR( "library/plugin", "no plugin handled 'runtime_get_bootdevice', returning TT_ERR" );
	return TT_ERR;
}

