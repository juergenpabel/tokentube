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
int libtokentube_plugin__user_create(const char* username, const char* password) {
	tt_module_t*	module;
	size_t		i;

	TT_TRACE( "library/plugin", "%s(username='%s' password='%s')", __FUNCTION__, username, password );
	if( username == NULL || username[0] == '\0' || password == NULL || password[0] == '\0' ) {
		TT_LOG_ERROR( "library/plugin", "invalid parameters in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	TT_DEBUG3( "library/plugin", "invoking 'user_create' handlers" );
	for( i=0; i<MAX_PLUGINS+1; i++ ) {
		module = g_modules[i];
		if( module != NULL && module->plugin != NULL ) {
			TT_DEBUG5( "library/plugin", "checking 'user_create' for plugin '%s'", module->name );
			if( module->plugin->interface.api.user.create != NULL ) {
				TT_DEBUG4( "library/plugin", "invoking 'user_create' handler for plugin '%s'", module->name );
				switch( module->plugin->interface.api.user.create( username, password ) ) {
					case TT_OK:
						TT_DEBUG4( "library/plugin", "plugin '%s' successfully handled 'user_create'", module->name );
						if( libtokentube_runtime_broadcast( TT_EVENT__USER_CREATED, username ) != TT_OK ) {
							TT_LOG_WARN( "library/plugin", "libtokentube_runtime_broadcast() failed in %s()", __FUNCTION__ );
						}
						return TT_OK;
					case TT_IGN:
						TT_DEBUG5( "library/plugin", "plugin '%s' ignored 'user_update'", module->name );
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
int libtokentube_plugin__user_update(const char* username, const char* old_password, const char* new_password, tt_status_t* status) {
	tt_module_t*	module;
	size_t		i;

	TT_TRACE( "library/plugin", "%s(username='%s' old_password='%s', new_password='%s')", __FUNCTION__, username, old_password, new_password );
	if( username == NULL || username[0] == '\0' || old_password == NULL || old_password[0] == '\0' || new_password == NULL || new_password[0] == '\0' || status == NULL ) {
		TT_LOG_ERROR( "library/plugin", "invalid parameters in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	TT_DEBUG3( "library/plugin", "invoking 'user_update' handlers" );
	*status = TT_STATUS__UNDEFINED;
	for( i=0; i<MAX_PLUGINS+1; i++ ) {
		module = g_modules[i];
		if( module != NULL && module->plugin != NULL ) {
			TT_DEBUG5( "library/plugin", "checking 'user_update' for plugin '%s'", module->name );
			if( module->plugin->interface.api.user.update != NULL ) {
				TT_DEBUG4( "library/plugin", "invoking 'user_update' handler for plugin '%s'", module->name );
				switch( module->plugin->interface.api.user.update( username, old_password, new_password, status ) ) {
					case TT_OK:
						TT_DEBUG4( "library/plugin", "plugin '%s' successfully handled 'user_update'", module->name );
						if( *status == TT_YES ) {
							if( libtokentube_runtime_broadcast( TT_EVENT__USER_UPDATED, username ) != TT_OK ) {
								TT_LOG_WARN( "library/plugin", "libtokentube_runtime_broadcast() failed in %s()", __FUNCTION__ );
							}
						}
						return TT_OK;
					case TT_IGN:
						TT_DEBUG5( "library/plugin", "plugin '%s' ignored 'user_update'", module->name );
						break;
					default:
						TT_LOG_ERROR( "library/plugin", "plugin '%s' returned error for 'user_update'", module->name );
						break;
				}
			}
		}
	}
	TT_LOG_ERROR( "library/plugin", "no plugin handled 'user_update', returning TT_ERR" );
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
			if( module->plugin->interface.api.user.delete != NULL ) {
				TT_DEBUG4( "library/plugin", "invoking 'user_delete' handler for plugin '%s'", module->name );
				switch( module->plugin->interface.api.user.delete( username, status ) ) {
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
			if( module->plugin->interface.api.user.exists != NULL ) {
				TT_DEBUG4( "library/plugin", "invoking 'user_exists' handler for plugin '%s'", module->name );
				switch( module->plugin->interface.api.user.exists( username, status ) ) {
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



__attribute__ ((visibility ("hidden")))
int libtokentube_plugin__user_execute_load( const char* username, const char* password, char* key, size_t* key_size ) {
	tt_module_t*	module;
	size_t		i;

	TT_TRACE( "library/plugin", "%s(username='%s' password='%s')", __FUNCTION__, username, password );
	if( username == NULL || username[0] == '\0' || password == NULL || password[0] == '\0' || key == NULL || key_size == NULL || *key_size == 0) {
		TT_LOG_FATAL( "library/plugin", "invalid parameters in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	TT_DEBUG3( "library/plugin", "invoking 'user_execute_load' handlers" );
	for( i=0; i<MAX_PLUGINS+1; i++ ) {
		module = g_modules[i];
		if( module != NULL && module->plugin != NULL ) {
			TT_DEBUG5( "library/plugin", "checking 'user_execute_load' for plugin '%s'", module->name );
			if( module->plugin->interface.api.user.execute_load != NULL ) {
				TT_DEBUG4( "library/plugin", "invoking 'user_execute_load' handler for plugin '%s'", module->name );
				switch( module->plugin->interface.api.user.execute_load( username, password, key, key_size ) ) {
					case TT_OK:
						TT_DEBUG4( "library/plugin", "plugin '%s' successfully handled 'user_execute_load'", module->name );
						if( *key_size > 0 ) {
							if( libtokentube_runtime_broadcast( TT_EVENT__USER_VERIFIED, username ) != TT_OK ) {
								TT_LOG_WARN( "library/plugin", "libtokentube_runtime_broadcast() failed in %s()", __FUNCTION__ );
							}
						}
						return TT_OK;
					case TT_IGN:
						TT_DEBUG4( "library/plugin", "plugin '%s' ignored 'user_execute_load'", module->name );
						break;
					default:
						TT_LOG_ERROR( "library/plugin", "plugin '%s' returned error for 'user_execute_load'", module->name );
				}
			}
		}
	}
	TT_LOG_ERROR( "library/plugin", "no plugin handled 'user_execute_load', returning TT_ERR" );
	return TT_ERR;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_plugin__user_execute_verify( const char* username, const char* password, tt_status_t* status ) {
	tt_module_t*	module;
	size_t		i;

	TT_TRACE( "library/plugin", "%s(username='%s' password='%s')", __FUNCTION__, username, password );
	if( username == NULL || username[0] == '\0' || password == NULL || password[0] == '\0' || status == NULL ) {
		TT_LOG_FATAL( "library/plugin", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	TT_DEBUG3( "library/plugin", "invoking 'user_execute_verify' handlers" );
	*status = TT_STATUS__UNDEFINED;
	for( i=0; i<MAX_PLUGINS+1; i++ ) {
		module = g_modules[i];
		if( module != NULL && module->plugin != NULL ) {
			TT_DEBUG5( "library/plugin", "checking 'user_execute_verify' for plugin '%s'", module->name );
			if( module->plugin->interface.api.user.execute_verify != NULL ) {
				TT_DEBUG4( "library/plugin", "invoking 'user_execute_verify' handler for plugin '%s'", module->name );
				switch( module->plugin->interface.api.user.execute_verify( username, password, status ) ) {
					case TT_OK:
						TT_DEBUG4( "library/plugin", "plugin '%s' successfully handled 'user_execute_verify'", module->name );
						if( *status == TT_YES ) {
							if( libtokentube_runtime_broadcast( TT_EVENT__USER_VERIFIED, username ) != TT_OK ) {
								TT_LOG_WARN( "library/plugin", "libtokentube_runtime_broadcast() failed in %s()", __FUNCTION__ );
							}
						}
						return TT_OK;
					case TT_IGN:
						TT_DEBUG4( "library/plugin", "plugin '%s' ignored 'user_execute_verify'", module->name );
						break;
					default:
						TT_LOG_ERROR( "library/plugin", "plugin '%s' returned error for 'user_execute_verify'", module->name );
				}
			}
		}
	}
	TT_LOG_ERROR( "library/plugin", "no plugin handled 'user_execute_verify', returning TT_ERR" );
	return TT_ERR;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_plugin__user_execute_autoenrollment(const char* username, const char* password, tt_status_t* status) {
	tt_module_t*	module;
	size_t		i;

	TT_TRACE( "library/plugin", "%s(username='%s')", __FUNCTION__, username );
	if( username == NULL || username[0] == '\0' || password == NULL || password[0] == '\0' || status == NULL ) {
		TT_LOG_FATAL( "library/plugin", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	TT_DEBUG3( "library/plugin", "invoking 'user_execute_autoenrollment' handlers" );
	*status = TT_STATUS__UNDEFINED;
	for( i=0; i<MAX_PLUGINS+1; i++ ) {
		module = g_modules[i];
		if( module != NULL && module->plugin != NULL ) {
			TT_DEBUG5( "library/plugin", "checking 'user_autoenrollment' for plugin '%s'", module->name );
			if( module->plugin->interface.api.user.execute_autoenrollment != NULL ) {
				TT_DEBUG4( "library/plugin", "invoking 'user_execute_autoenrollment' handler for plugin '%s'", module->name );
				switch( module->plugin->interface.api.user.execute_autoenrollment( username, password, status ) ) {
					case TT_OK:
						TT_DEBUG4( "library/plugin", "plugin '%s' successfully handled 'user_execute_autoenrollment'", module->name );
						return TT_OK;
					case TT_IGN:
						TT_DEBUG4( "library/plugin", "plugin '%s' ignored 'user_execute_autoenrollment'", module->name );
						break;
					default:
						TT_LOG_ERROR( "library/plugin", "plugin '%s' returned error for 'user_execute_autoenrollment'", module->name );
				}
			}
		}
	}
	TT_LOG_ERROR( "library/plugin", "no plugin handled 'user_execute_autoenrollment', returning TT_ERR" );
	return TT_ERR;
}

