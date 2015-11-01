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
		TT_LOG_ERROR( "library/plugin", "BUG: invalid invocation of API:database.uhd.create()" );
		TT_DEBUG_SRC( "library/plugin", "invalid parameters" );
		return TT_ERR;
	}
	*status = TT_STATUS__UNDEFINED;
	for( i=0; i<MAX_PLUGINS+1; i++ ) {
		module = g_modules[i];
		if( module != NULL && module->plugin != NULL ) {
			if( module->plugin->interface.api.database.uhd.create != NULL ) {
				TT_DEBUG3( "library/plugin", "invoking database.uhd.create('%s') on plugin '%s'", identifier, module->name );
				switch( module->plugin->interface.api.database.uhd.create( identifier, status ) ) {
					case TT_OK:
						TT_DEBUG4( "library/plugin", "plugin '%s' successfully processed database.uhd.create()", module->name );
						if( *status == TT_STATUS__YES ) {
							if( libtokentube_runtime_broadcast( TT_EVENT__UHD_CREATED, identifier ) != TT_OK ) {
								TT_LOG_WARN( "library/plugin", "libtokentube_runtime_broadcast() failed in %s()", __FUNCTION__ );
							}
						}
						return TT_OK;
					case TT_IGN:
						TT_DEBUG4( "library/plugin", "plugin '%s' ignored database.uhd.create()", module->name );
						break;
					default:
						TT_LOG_ERROR( "library/plugin", "plugin '%s' returned error for database.uhd.create()", module->name );
				}
			}
		}
	}
	TT_LOG_ERROR( "library/plugin", "no plugin handled database.uhd.create(), returning TT_ERR" );
	return TT_ERR;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_plugin__uhd_modify(const char* identifier, tt_modify_t action, void* data, tt_status_t* status) {
	tt_module_t*	module;
	size_t		i;

	TT_TRACE( "library/plugin", "%s(identifier='%s')", __FUNCTION__, identifier );
	if( identifier == NULL || identifier[0] == '\0' || data == NULL || status == NULL ) {
		TT_LOG_ERROR( "library/plugin", "BUG: invalid invocation of API:database.uhd.modify()" );
		TT_DEBUG_SRC( "library/plugin", "invalid parameters" );
		return TT_ERR;
	}
	*status = TT_STATUS__UNDEFINED;
	for( i=0; i<MAX_PLUGINS+1; i++ ) {
		module = g_modules[i];
		if( module != NULL && module->plugin != NULL ) {
			if( module->plugin->interface.api.database.uhd.modify != NULL ) {
				TT_DEBUG3( "library/plugin", "invoking database.uhd.modify('%s') on plugin '%s'", identifier, module->name );
				switch( module->plugin->interface.api.database.uhd.modify( identifier, action, data, status ) ) {
					case TT_OK:
						TT_DEBUG4( "library/plugin", "plugin '%s' successfully processed database.uhd.modify()", module->name );
						if( *status == TT_STATUS__YES ) {
							if( libtokentube_runtime_broadcast( TT_EVENT__UHD_CREATED, identifier ) != TT_OK ) {
								TT_LOG_WARN( "library/plugin", "libtokentube_runtime_broadcast() failed in %s()", __FUNCTION__ );
							}
						}
						return TT_OK;
					case TT_IGN:
						TT_DEBUG4( "library/plugin", "plugin '%s' ignored database.uhd.modify()", module->name );
						break;
					default:
						TT_LOG_ERROR( "library/plugin", "plugin '%s' returned error for database.uhd.modify()", module->name );
				}
			}
		}
	}
	TT_LOG_ERROR( "library/plugin", "no plugin handled database.uhd.modify(), returning TT_ERR" );
	return TT_ERR;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_plugin__uhd_delete(const char* identifier, tt_status_t* status) {
	tt_module_t*	module;
	size_t		i;

	TT_TRACE( "library/plugin", "%s(identifier='%s')", __FUNCTION__, identifier );
	if( identifier == NULL || identifier[0] == '\0' || status == NULL ) {
		TT_LOG_ERROR( "library/plugin", "BUG: invalid invocation of API:database.uhd.delete()" );
		TT_DEBUG_SRC( "library/plugin", "invalid parameters" );
		return TT_ERR;
	}
	for( i=0; i<MAX_PLUGINS+1; i++ ) {
		module = g_modules[i];
		if( module != NULL && module->plugin != NULL ) {
			if( module->plugin->interface.api.database.uhd.delete != NULL ) {
				TT_DEBUG3( "library/plugin", "invoking database.uhd.delete('%s') on plugin '%s'", identifier, module->name );
				switch( module->plugin->interface.api.database.uhd.delete( identifier, status ) ) {
					case TT_OK:
						TT_DEBUG4( "library/plugin", "plugin '%s' successfully processed database.uhd.delete()", module->name );
						if( *status == TT_YES ) {
							if( libtokentube_runtime_broadcast( TT_EVENT__UHD_DELETED, identifier ) != TT_OK ) {
								TT_LOG_WARN( "library/plugin", "libtokentube_runtime_broadcast() failed in %s()", __FUNCTION__ );
							}
						}
						return TT_OK;
					case TT_IGN:
						TT_DEBUG4( "library/plugin", "plugin '%s' ignored database.uhd.delete()", module->name );
						break;
					default:
						TT_DEBUG4( "library/plugin", "plugin '%s' returned error for database.uhd.delete()", module->name );
				}
			}
		}
	}
	TT_LOG_ERROR( "library/plugin", "no plugin handled database.uhd.delete(), returning TT_ERR" );
	return TT_ERR;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_plugin__uhd_exists(const char* identifier, tt_status_t* status) {
	tt_module_t*	module;
	size_t		i;

	TT_TRACE( "library/plugin", "%s(identifier='%s')", __FUNCTION__, identifier );
	if( identifier == NULL || identifier[0] == '\0' || status == NULL ) {
		TT_LOG_ERROR( "library/plugin", "BUG: invalid invocation of API:database.uhd.exists()" );
		TT_DEBUG_SRC( "library/plugin", "invalid parameters" );
		return TT_ERR;
	}
	for( i=0; i<MAX_PLUGINS+1; i++ ) {
		module = g_modules[i];
		if( module != NULL && module->plugin != NULL ) {
			if( module->plugin->interface.api.database.uhd.exists != NULL ) {
				TT_DEBUG3( "library/plugin", "invoking database.uhd.exists('%s') on plugin '%s'", identifier, module->name );
				switch( module->plugin->interface.api.database.uhd.exists( identifier, status ) ) {
					case TT_OK:
						TT_DEBUG4( "library/plugin", "plugin '%s' successfully processed database.uhd.exists()", module->name );
						return TT_OK;
					case TT_IGN:
						TT_DEBUG4( "library/plugin", "plugin '%s' ignored database.uhd.exists()", module->name );
						break;
					default:
						TT_DEBUG4( "library/plugin", "plugin '%s' returned error for database.uhd.exists()", module->name );
				}
			}
		}
	}
	TT_LOG_ERROR( "library/plugin", "no plugin handled database.uhd.exists(), returning TT_ERR" );
	return TT_ERR;
}

