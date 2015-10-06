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
int libtokentube_plugin__uhd_create(const char* identifier, tt_status_t* status) {
	tt_module_t*	module;
	size_t		i;

	TT_TRACE( "library/plugin", "%s(identifier='%s')", __FUNCTION__, identifier );
	if( identifier == NULL || identifier[0] == '\0' || status == NULL ) {
		TT_LOG_ERROR( "library/plugin", "invalid parameters in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	*status = TT_STATUS__UNDEFINED;
	TT_DEBUG3( "library/plugin", "invoking 'uhd_create' handlers" );
	for( i=0; i<MAX_PLUGINS+1; i++ ) {
		module = g_modules[i];
		if( module != NULL && module->plugin != NULL ) {
			TT_DEBUG5( "library/plugin", "processing 'uhd_create' for plugin '%s'", module->name );
			if( module->plugin->interface.api.database.uhd.create != NULL ) {
				TT_DEBUG4( "library/plugin", "invoking 'uhd_create' for plugin '%s'", module->name );
				switch( module->plugin->interface.api.database.uhd.create( identifier, status ) ) {
					case TT_OK:
						TT_DEBUG4( "library/plugin", "plugin '%s' successfully handled 'uhd_create'", module->name );
						if( *status == TT_STATUS__YES ) {
							if( libtokentube_runtime_broadcast( TT_EVENT__UHD_CREATED, identifier ) != TT_OK ) {
								TT_LOG_WARN( "library/plugin", "libtokentube_runtime_broadcast() failed in %s()", __FUNCTION__ );
							}
						}
						return TT_OK;
					case TT_IGN:
						TT_DEBUG5( "library/plugin", "plugin '%s' ignored 'uhd_create'", module->name );
						break;
					default:
						TT_LOG_ERROR( "library/plugin", "plugin '%s' returned error for 'uhd_create'", module->name );
				}
			}
		}
	}
	TT_LOG_ERROR( "library/plugin", "no plugin handled 'uhd_create', returning TT_ERR" );
	return TT_ERR;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_plugin__uhd_modify(const char* identifier, tt_modify_t action, void* data, tt_status_t* status) {
	tt_module_t*	module;
	size_t		i;

	TT_TRACE( "library/plugin", "%s(identifier='%s')", __FUNCTION__, identifier );
	if( identifier == NULL || identifier[0] == '\0' || data == NULL || status == NULL ) {
		TT_LOG_ERROR( "library/plugin", "invalid parameters in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	*status = TT_STATUS__UNDEFINED;
	TT_DEBUG3( "library/plugin", "invoking 'uhd_modify' handlers" );
	for( i=0; i<MAX_PLUGINS+1; i++ ) {
		module = g_modules[i];
		if( module != NULL && module->plugin != NULL ) {
			TT_DEBUG5( "library/plugin", "processing 'uhd_modify' for plugin '%s'", module->name );
			if( module->plugin->interface.api.database.uhd.modify != NULL ) {
				TT_DEBUG4( "library/plugin", "invoking 'uhd_modify' for plugin '%s'", module->name );
				switch( module->plugin->interface.api.database.uhd.modify( identifier, action, data, status ) ) {
					case TT_OK:
						TT_DEBUG4( "library/plugin", "plugin '%s' successfully handled 'uhd_modify'", module->name );
						if( *status == TT_STATUS__YES ) {
							if( libtokentube_runtime_broadcast( TT_EVENT__UHD_CREATED, identifier ) != TT_OK ) {
								TT_LOG_WARN( "library/plugin", "libtokentube_runtime_broadcast() failed in %s()", __FUNCTION__ );
							}
						}
						return TT_OK;
					case TT_IGN:
						TT_DEBUG5( "library/plugin", "plugin '%s' ignored 'uhd_modify'", module->name );
						break;
					default:
						TT_LOG_ERROR( "library/plugin", "plugin '%s' returned error for 'uhd_modify'", module->name );
				}
			}
		}
	}
	TT_LOG_ERROR( "library/plugin", "no plugin handled 'uhd_modify', returning TT_ERR" );
	return TT_ERR;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_plugin__uhd_delete(const char* identifier, tt_status_t* status) {
	tt_module_t*	module;
	size_t		i;

	TT_TRACE( "library/plugin", "%s(identifier='%s')", __FUNCTION__, identifier );
	if( identifier == NULL || identifier[0] == '\0' || status == NULL ) {
		TT_LOG_ERROR( "library/plugin", "invalid parameters in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	TT_DEBUG3( "library/plugin", "invoking 'uhd_delete' handlers" );
	for( i=0; i<MAX_PLUGINS+1; i++ ) {
		module = g_modules[i];
		if( module != NULL && module->plugin != NULL ) {
			TT_DEBUG5( "library/plugin", "checking 'uhd_delete' for plugin '%s'", module->name );
			if( module->plugin->interface.api.database.uhd.delete != NULL ) {
				switch( module->plugin->interface.api.database.uhd.delete( identifier, status ) ) {
					case TT_OK:
						TT_DEBUG4( "library/plugin", "plugin '%s' successfully handled 'uhd_delete'", module->name );
						if( *status == TT_YES ) {
							if( libtokentube_runtime_broadcast( TT_EVENT__UHD_DELETED, identifier ) != TT_OK ) {
								TT_LOG_WARN( "library/plugin", "libtokentube_runtime_broadcast() failed in %s()", __FUNCTION__ );
							}
						}
						return TT_OK;
					case TT_IGN:
						TT_DEBUG4( "library/plugin", "plugin '%s' ignored 'uhd_delete'", module->name );
						break;
					default:
						TT_DEBUG4( "library/plugin", "plugin '%s' returned error for 'uhd_delete'", module->name );
				}
			}
		}
	}
	TT_LOG_ERROR( "library/plugin", "no plugin handled 'uhd_delete', returning TT_ERR" );
	return TT_ERR;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_plugin__uhd_exists(const char* identifier, tt_status_t* status) {
	tt_module_t*	module;
	size_t		i;

	TT_TRACE( "library/plugin", "%s(identifier='%s')", __FUNCTION__, identifier );
	if( identifier == NULL || identifier[0] == '\0' || status == NULL ) {
		TT_LOG_ERROR( "library/plugin", "invalid parameters in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	TT_DEBUG3( "library/plugin", "invoking 'uhd_exists' handlers" );
	for( i=0; i<MAX_PLUGINS+1; i++ ) {
		module = g_modules[i];
		if( module != NULL && module->plugin != NULL ) {
			TT_DEBUG5( "library/plugin", "checking 'uhd_exists' for plugin '%s'", module->name );
			if( module->plugin->interface.api.database.uhd.exists != NULL ) {
				switch( module->plugin->interface.api.database.uhd.exists( identifier, status ) ) {
					case TT_OK:
						TT_DEBUG4( "library/plugin", "plugin '%s' successfully handled 'uhd_exists'", module->name );
						return TT_OK;
					case TT_IGN:
						TT_DEBUG4( "library/plugin", "plugin '%s' ignored 'uhd_exists'", module->name );
						break;
					default:
						TT_DEBUG4( "library/plugin", "plugin '%s' returned error for 'uhd_exists'", module->name );
				}
			}
		}
	}
	TT_LOG_ERROR( "library/plugin", "no plugin handled 'uhd_exists', returning TT_ERR" );
	return TT_ERR;
}

