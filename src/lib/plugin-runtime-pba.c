#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <sys/types.h>
#include "libtokentube.h"


__attribute__ ((visibility ("hidden")))
int libtokentube_plugin__pba_install_pre(const char* type, const char* path) {
	tt_module_t*	module;
	size_t		i;

	TT_TRACE( "library/plugin", "%s(type='%s',path='%s')", __FUNCTION__, type, path );
	if( type == NULL || type[0] == '\0' || path == NULL || path[0] == '\0' ) {
		TT_LOG_ERROR( "library/plugin", "BUG: invalid invocation of API:pba.install.pre()" );
		TT_DEBUG_SRC( "library/plugin", "invalid parameters" );
		return TT_ERR;
	}
	TT_DEBUG3( "library/plugin", "invoking runtime.pba.install_pre() on plugin 'default'" );
	switch( g_modules[MAX_PLUGINS]->plugin->interface.api.runtime.pba.install_pre( type, path ) ) {
		case TT_OK:
			TT_DEBUG4( "library/plugin", "plugin '%s' successfully processed runtime.pba.install_pre()", g_modules[MAX_PLUGINS]->name );
			break;
		case TT_IGN:
			TT_DEBUG4( "library/plugin", "plugin '%s' returned TT_IGN for runtime.pba.install_pre()", g_modules[MAX_PLUGINS]->name );
			break;
		default:
			TT_LOG_ERROR( "library/plugin", "plugin '%s' returned TT_ERR for runtime.pba.install_pre()", g_modules[MAX_PLUGINS]->name );
			return TT_ERR;
	}
	for( i=0; i<MAX_PLUGINS; i++ ) {
		module = g_modules[i];
		if( module != NULL && module->plugin != NULL ) {
			if( module->plugin->interface.api.runtime.pba.install_pre != NULL ) {
				TT_DEBUG3( "library/plugin", "invoking runtime.pba.install_pre() on plugin '%s'", module->name );
				switch( module->plugin->interface.api.runtime.pba.install_pre( type, path ) ) {
					case TT_OK:
						TT_DEBUG4( "library/plugin", "plugin '%s' successfully processed runtime.pba.install_pre()", module->name );
						break;
					case TT_IGN:
						TT_DEBUG4( "library/plugin", "plugin '%s' returned TT_IGN for runtime.pba.install_pre()", module->name );
						break;
					default:
						TT_LOG_ERROR( "library/plugin", "plugin '%s' returned TT_ERR for runtime.pba.install_pre()", module->name );
						return TT_ERR;
				}
			}
		}
	}
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_plugin__pba_install(const char* type, const char* path) {
	tt_module_t*	module;
	size_t		i;

	TT_TRACE( "library/plugin", "%s(type='%s',path='%s')", __FUNCTION__, type, path );
	if( type == NULL || type[0] == '\0' || path == NULL || path[0] == '\0' ) {
		TT_LOG_ERROR( "library/plugin", "BUG: invalid invocation of API:pba.install()" );
		TT_DEBUG_SRC( "library/plugin", "invalid parameters" );
		return TT_ERR;
	}
	TT_DEBUG3( "library/plugin", "invoking runtime.pba.install_run() on plugin 'default'" );
	switch( g_modules[MAX_PLUGINS]->plugin->interface.api.runtime.pba.install_run( type, path ) ) {
		case TT_OK:
			TT_DEBUG4( "library/plugin", "plugin '%s' successfully processed runtime.pba.install_run()", g_modules[MAX_PLUGINS]->name );
			break;
		case TT_IGN:
			TT_DEBUG4( "library/plugin", "plugin '%s' returned TT_IGN for runtime.pba.install_run()", g_modules[MAX_PLUGINS]->name );
			break;
		default:
			TT_LOG_ERROR( "library/plugin", "plugin '%s' returned TT_ERR for runtime.pba.install_run()", g_modules[MAX_PLUGINS]->name );
			return TT_ERR;
	}
	for( i=0; i<MAX_PLUGINS; i++ ) {
		module = g_modules[i];
		if( module != NULL && module->plugin != NULL ) {
			if( module->plugin->interface.api.runtime.pba.install_run != NULL ) {
				TT_DEBUG3( "library/plugin", "invoking runtime.pba.install_run() on plugin '%s'", module->name );
				switch( module->plugin->interface.api.runtime.pba.install_run( type, path ) ) {
					case TT_OK:
						TT_DEBUG4( "library/plugin", "plugin '%s' successfully processed runtime.pba.install_run()", module->name );
						break;
					case TT_IGN:
						TT_DEBUG4( "library/plugin", "plugin '%s' returned TT_IGN for runtime.pba.install_run()", module->name );
						break;
					default:
						TT_LOG_ERROR( "library/plugin", "plugin '%s' returned TT_ERR for runtime.pba.install_run()", module->name );
						return TT_ERR;
				}
			}
		}
	}
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_plugin__pba_install_post(const char* type, const char* path) {
	tt_module_t*	module;
	size_t		i;

	TT_TRACE( "library/plugin", "%s(type='%s',path='%s')", __FUNCTION__, type, path );
	if( type == NULL || type[0] == '\0' || path == NULL || path[0] == '\0' ) {
		TT_LOG_ERROR( "library/plugin", "BUG: invalid invocation of API:pba.install()" );
		TT_DEBUG_SRC( "library/plugin", "invalid parameters" );
		return TT_ERR;
	}
	for( i=0; i<MAX_PLUGINS; i++ ) {
		module = g_modules[i];
		if( module != NULL && module->plugin != NULL ) {
			if( module->plugin->interface.api.runtime.pba.install_post != NULL ) {
				TT_DEBUG3( "library/plugin", "invoking runtime.pba.install_post() on plugin '%s'", module->name );
				switch( module->plugin->interface.api.runtime.pba.install_post( type, path ) ) {
					case TT_OK:
						TT_DEBUG4( "library/plugin", "plugin '%s' successfully processed runtime.pba.install_post()", module->name );
						break;
					case TT_IGN:
						TT_DEBUG4( "library/plugin", "plugin '%s' returned TT_IGN for runtime.pba.install_post()", module->name );
						break;
					default:
						TT_LOG_ERROR( "library/plugin", "plugin '%s' returned TT_ERR for runtime.pba.install_post()", module->name );
						return TT_ERR;
				}
			}
		}
	}
	TT_DEBUG3( "library/plugin", "invoking runtime.pba.install_post() on plugin 'default'" );
	switch( g_modules[MAX_PLUGINS]->plugin->interface.api.runtime.pba.install_post( type, path ) ) {
		case TT_OK:
			TT_DEBUG4( "library/plugin", "plugin '%s' successfully processed runtime.pba.install_post()", g_modules[MAX_PLUGINS]->name );
			break;
		case TT_IGN:
			TT_DEBUG4( "library/plugin", "plugin '%s' returned TT_IGN for runtime.pba.install_post()", g_modules[MAX_PLUGINS]->name );
			break;
		default:
			TT_LOG_ERROR( "library/plugin", "plugin '%s' returned TT_ERR for runtime.pba.install_post()", g_modules[MAX_PLUGINS]->name );
			return TT_ERR;
	}
	return TT_OK;
}

