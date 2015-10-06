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
int libtokentube_plugin__user_create(const char* username, const char* password, tt_status_t* status) {
	tt_module_t*	module;
	size_t		i;

	TT_TRACE( "library/plugin", "%s(username='%s' password='%s')", __FUNCTION__, username, password );
	if( username == NULL || username[0] == '\0' || password == NULL || password[0] == '\0' ) {
		TT_LOG_ERROR( "library/plugin", "invalid parameters in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	*status = TT_STATUS__UNDEFINED;
	TT_DEBUG3( "library/plugin", "invoking 'user_create' handlers" );
	for( i=0; i<MAX_PLUGINS+1; i++ ) {
		module = g_modules[i];
		if( module != NULL && module->plugin != NULL ) {
			TT_DEBUG5( "library/plugin", "checking 'user_create' for plugin '%s'", module->name );
			if( module->plugin->interface.api.database.user.create != NULL ) {
				TT_DEBUG4( "library/plugin", "invoking 'user_create' handler for plugin '%s'", module->name );
				switch( module->plugin->interface.api.database.user.create( username, password, status ) ) {
					case TT_OK:
						TT_DEBUG4( "library/plugin", "plugin '%s' successfully handled 'user_create'", module->name );
						if( *status == TT_STATUS__YES ) {
							if( libtokentube_runtime_broadcast( TT_EVENT__USER_CREATED, username ) != TT_OK ) {
								TT_LOG_WARN( "library/plugin", "libtokentube_runtime_broadcast() failed in %s()", __FUNCTION__ );
							}
						}
						return TT_OK;
					case TT_IGN:
						TT_DEBUG5( "library/plugin", "plugin '%s' ignored 'user_create'", module->name );
						break;
					default:
						TT_LOG_ERROR( "library/plugin", "plugin '%s' returned error for 'user_create'", module->name );
				}
			}
		}
	}
	TT_LOG_ERROR( "library/plugin", "no plugin handled 'user_create', returning TT_ERR" );
	return TT_ERR;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_plugin__user_modify(const char* username, const char* password, tt_modify_t action, void* data, tt_status_t* status) {
	tt_module_t*	module;
	size_t		i;

	TT_TRACE( "library/plugin", "%s(username='%s' password='%s', data='%s')", __FUNCTION__, username, password, data );
	if( username == NULL || username[0] == '\0' || password == NULL || password[0] == '\0' || data == NULL || status == NULL ) {
		TT_LOG_ERROR( "library/plugin", "invalid parameters in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	TT_DEBUG3( "library/plugin", "invoking 'user_modify' handlers" );
	*status = TT_STATUS__UNDEFINED;
	for( i=0; i<MAX_PLUGINS+1; i++ ) {
		module = g_modules[i];
		if( module != NULL && module->plugin != NULL ) {
			TT_DEBUG5( "library/plugin", "checking 'user_modify' for plugin '%s'", module->name );
			if( module->plugin->interface.api.database.user.modify != NULL ) {
				TT_DEBUG4( "library/plugin", "invoking 'user_modify' handler for plugin '%s'", module->name );
				switch( module->plugin->interface.api.database.user.modify( username, password, action, data, status ) ) {
					case TT_OK:
						TT_DEBUG4( "library/plugin", "plugin '%s' successfully handled 'user_modify'", module->name );
						if( *status == TT_YES ) {
							if( libtokentube_runtime_broadcast( TT_EVENT__USER_MODIFIED, username ) != TT_OK ) {
								TT_LOG_WARN( "library/plugin", "libtokentube_runtime_broadcast() failed in %s()", __FUNCTION__ );
							}
						}
						return TT_OK;
					case TT_IGN:
						TT_DEBUG5( "library/plugin", "plugin '%s' ignored 'user_modify'", module->name );
						break;
					default:
						TT_LOG_ERROR( "library/plugin", "plugin '%s' returned error for 'user_modify'", module->name );
						break;
				}
			}
		}
	}
	TT_LOG_ERROR( "library/plugin", "no plugin handled 'user_modify', returning TT_ERR" );
	return TT_ERR;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_plugin__user_delete(const char* username, tt_status_t* status) {
	tt_module_t*	module;
	size_t		i;

	TT_TRACE( "library/plugin", "%s(username='%s')", __FUNCTION__, username );
	if( username == NULL || username[0] == '\0' || status == NULL ) {
		TT_LOG_FATAL( "library/plugin", "invalid parameters in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	TT_DEBUG3( "library/plugin", "invoking 'user_delete' handlers" );
	for( i=0; i<MAX_PLUGINS+1; i++ ) {
		module = g_modules[i];
		if( module != NULL && module->plugin != NULL ) {
			TT_DEBUG5( "library/plugin", "checking 'user_delete' for plugin '%s'", module->name );
			if( module->plugin->interface.api.database.user.delete != NULL ) {
				TT_DEBUG4( "library/plugin", "invoking 'user_delete' handler for plugin '%s'", module->name );
				switch( module->plugin->interface.api.database.user.delete( username, status ) ) {
					case TT_OK:
						TT_DEBUG4( "library/plugin", "plugin '%s' successfully handled 'user_delete'", module->name );
						if( *status == TT_YES ) {
							if( libtokentube_runtime_broadcast( TT_EVENT__USER_DELETED, username ) != TT_OK ) {
								TT_LOG_WARN( "library/plugin", "libtokentube_runtime_broadcast() failed in %s()", __FUNCTION__ );
							}
						}
						return TT_OK;
					case TT_IGN:
						TT_DEBUG5( "library/plugin", "plugin '%s' ignored 'user_delete'", module->name );
						break;
					default:
						TT_LOG_ERROR( "library/plugin", "plugin '%s' returned error for 'user_delete'", module->name );
				}
			}
		}
	}
	TT_LOG_ERROR( "library/plugin", "no plugin handled 'user_delete', returning TT_ERR" );
	return TT_ERR;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_plugin__user_exists(const char* username, tt_status_t* status) {
	tt_module_t*	module;
	size_t		i;

	TT_TRACE( "library/plugin", "%s(username='%s')", __FUNCTION__, username );
	if( username == NULL || username[0] == '\0' || status == NULL ) {
		TT_LOG_FATAL( "library/plugin", "invalid parameters in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	TT_DEBUG3( "library/plugin", "invoking 'user_exists' handlers" );
	*status = TT_STATUS__UNDEFINED;
	for( i=0; i<MAX_PLUGINS+1; i++ ) {
		module = g_modules[i];
		if( module != NULL && module->plugin != NULL ) {
			TT_DEBUG5( "library/plugin", "checking 'user_exists' for plugin '%s'", module->name );
			if( module->plugin->interface.api.database.user.exists != NULL ) {
				TT_DEBUG4( "library/plugin", "invoking 'user_exists' handler for plugin '%s'", module->name );
				switch( module->plugin->interface.api.database.user.exists( username, status ) ) {
					case TT_OK:
						TT_DEBUG4( "library/plugin", "plugin '%s' successfully handled 'user_exists'", module->name );
						return TT_OK;
					case TT_IGN:
						TT_DEBUG4( "library/plugin", "plugin '%s' ignored 'user_exists'", module->name );
						break;
					default:
						TT_LOG_ERROR( "library/plugin", "plugin '%s' returned error for 'user_exists'", module->name );
				}
			}
		}
	}
	TT_LOG_ERROR( "library/plugin", "no plugin handled 'user_exists', returning TT_ERR" );
	return TT_ERR;
}

