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
		TT_LOG_ERROR( "library/plugin", "invalid parameters in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	TT_DEBUG3( "library/plugin", "invoking 'pba_install_pre' handlers" );
	TT_DEBUG4( "library/plugin", "invoking 'pba_install' for plugin '%s'", g_modules[MAX_PLUGINS]->name );
	switch( g_modules[MAX_PLUGINS]->plugin->interface.api.pba.install_pre( type, path ) ) {
		case TT_OK:
			TT_DEBUG4( "library/plugin", "plugin '%s' successfully handled 'pba_install_pre'", g_modules[MAX_PLUGINS]->name );
			break;
		case TT_IGN:
			TT_DEBUG4( "library/plugin", "plugin '%s' ignored 'pba_install_pre'", g_modules[MAX_PLUGINS]->name );
			break;
		default:
			TT_LOG_ERROR( "library/plugin", "plugin '%s' returned error for 'pba_install_pre'", g_modules[MAX_PLUGINS]->name );
			return TT_ERR;
	}
	for( i=0; i<MAX_PLUGINS; i++ ) {
		module = g_modules[i];
		if( module != NULL && module->plugin != NULL ) {
			TT_DEBUG5( "library/plugin", "checking 'pba_install_pre' for plugin '%s'", module->name );
			if( module->plugin->interface.api.pba.install_pre != NULL ) {
				TT_DEBUG4( "library/plugin", "invoking 'pba_install_pre' for plugin '%s'", module->name );
				switch( module->plugin->interface.api.pba.install_pre( type, path ) ) {
					case TT_OK:
						TT_DEBUG4( "library/plugin", "plugin '%s' successfully handled 'pba_install_pre'", module->name );
						break;
					case TT_IGN:
						TT_DEBUG5( "library/plugin", "plugin '%s' ignored 'pba_install_pre'", module->name );
						break;
					default:
						TT_LOG_ERROR( "library/plugin", "plugin '%s' returned error for 'pba_install_pre'", module->name );
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
		TT_LOG_ERROR( "library/plugin", "invalid parameters in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	TT_DEBUG3( "library/plugin", "invoking 'pba_install' handlers" );
	TT_DEBUG4( "library/plugin", "invoking 'pba_install' for plugin '%s'", g_modules[MAX_PLUGINS]->name );
	switch( g_modules[MAX_PLUGINS]->plugin->interface.api.pba.install_run( type, path ) ) {
		case TT_OK:
			TT_DEBUG4( "library/plugin", "plugin '%s' successfully handled 'pba_install'", g_modules[MAX_PLUGINS]->name );
			break;
		case TT_IGN:
			TT_DEBUG4( "library/plugin", "plugin '%s' ignored 'pba_install'", g_modules[MAX_PLUGINS]->name );
			break;
		default:
			TT_LOG_ERROR( "library/plugin", "plugin '%s' returned error for 'pba_install'", g_modules[MAX_PLUGINS]->name );
			return TT_ERR;
	}
	for( i=0; i<MAX_PLUGINS; i++ ) {
		module = g_modules[i];
		if( module != NULL && module->plugin != NULL ) {
			TT_DEBUG5( "library/plugin", "checking 'pba_install' for plugin '%s'", module->name );
			if( module->plugin->interface.api.pba.install_run != NULL ) {
				TT_DEBUG4( "library/plugin", "invoking 'pba_install_run' for plugin '%s'", module->name );
				switch( module->plugin->interface.api.pba.install_run( type, path ) ) {
					case TT_OK:
						TT_DEBUG4( "library/plugin", "plugin '%s' successfully handled 'pba_install'", module->name );
						break;
					case TT_IGN:
						TT_DEBUG5( "library/plugin", "plugin '%s' ignored 'pba_install'", module->name );
						break;
					default:
						TT_LOG_ERROR( "library/plugin", "plugin '%s' returned error for 'pba_install'", module->name );
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
		TT_LOG_ERROR( "library/plugin", "invalid parameters in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	TT_DEBUG3( "library/plugin", "invoking 'pba_install_post' handlers" );
	for( i=0; i<MAX_PLUGINS; i++ ) {
		module = g_modules[i];
		if( module != NULL && module->plugin != NULL ) {
			TT_DEBUG5( "library/plugin", "checking 'pba_install_post' for plugin '%s'", module->name );
			if( module->plugin->interface.api.pba.install_post != NULL ) {
				TT_DEBUG4( "library/plugin", "invoking 'pba_install_post' for plugin '%s'", module->name );
				switch( module->plugin->interface.api.pba.install_post( type, path ) ) {
					case TT_OK:
						TT_DEBUG4( "library/plugin", "plugin '%s' successfully handled 'pba_install_post'", module->name );
						break;
					case TT_IGN:
						TT_DEBUG5( "library/plugin", "plugin '%s' ignored 'pba_install_post'", module->name );
						break;
					default:
						TT_LOG_ERROR( "library/plugin", "plugin '%s' returned error for 'pba_install_post'", module->name );
						return TT_ERR;
				}
			}
		}
	}
	TT_DEBUG4( "library/plugin", "invoking 'pba_install_post' for plugin '%s'", g_modules[MAX_PLUGINS]->name );
	switch( g_modules[MAX_PLUGINS]->plugin->interface.api.pba.install_post( type, path ) ) {
		case TT_OK:
			TT_DEBUG4( "library/plugin", "plugin '%s' successfully handled 'pba_install_post'", g_modules[MAX_PLUGINS]->name );
			break;
		case TT_IGN:
			TT_DEBUG5( "library/plugin", "plugin '%s' ignored 'pba_install_post'", g_modules[MAX_PLUGINS]->name );
			break;
		default:
			TT_LOG_ERROR( "library/plugin", "plugin '%s' returned error for 'pba_install_post'", g_modules[MAX_PLUGINS]->name );
			return TT_ERR;
	}
	return TT_OK;
}

