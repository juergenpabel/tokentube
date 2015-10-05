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
int libtokentube_plugin__otp_create(const char* identifier) {
	tt_module_t*	module;
	size_t		i;

	TT_TRACE( "library/plugin", "%s(identifier='%s')", __FUNCTION__, identifier );
	if( identifier == NULL || identifier[0] == '\0' ) {
		TT_LOG_ERROR( "library/plugin", "invalid parameters in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	TT_DEBUG3( "library/plugin", "invoking 'otp_create' handlers" );
	for( i=0; i<MAX_PLUGINS+1; i++ ) {
		module = g_modules[i];
		if( module != NULL && module->plugin != NULL ) {
			TT_DEBUG5( "library/plugin", "processing 'otp_create' for plugin '%s'", module->name );
			if( module->plugin->interface.api.database.otp.create != NULL ) {
				TT_DEBUG4( "library/plugin", "invoking 'otp_create' for plugin '%s'", module->name );
				switch( module->plugin->interface.api.database.otp.create( identifier ) ) {
					case TT_OK:
						TT_DEBUG4( "library/plugin", "plugin '%s' successfully handled 'otp_create'", module->name );
						if( libtokentube_runtime_broadcast( TT_EVENT__OTP_CREATED, identifier ) != TT_OK ) {
							TT_LOG_WARN( "library/plugin", "libtokentube_runtime_broadcast() failed in %s()", __FUNCTION__ );
						}
						return TT_OK;
					case TT_IGN:
						TT_DEBUG5( "library/plugin", "plugin '%s' ignored 'otp_create'", module->name );
						break;
					default:
						TT_LOG_ERROR( "library/plugin", "plugin '%s' returned error for 'otp_create'", module->name );
				}
			}
		}
	}
	TT_LOG_ERROR( "library/plugin", "no plugin handled 'otp_create', returning TT_ERR" );
	return TT_ERR;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_plugin__otp_update(const char* identifier, const char* key, size_t key_size, const char* new_key, size_t new_key_size, tt_status_t* status) {
	tt_module_t*	module;
	size_t		i;

	TT_TRACE( "library/plugin", "%s(identifier='%s')", __FUNCTION__, identifier );
	if( identifier == NULL || identifier[0] == '\0' ) {
		TT_LOG_ERROR( "library/plugin", "invalid parameters in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	TT_DEBUG3( "library/plugin", "invoking 'otp_update' handlers" );
	for( i=0; i<MAX_PLUGINS+1; i++ ) {
		module = g_modules[i];
		if( module != NULL && module->plugin != NULL ) {
			TT_DEBUG5( "library/plugin", "processing 'otp_update' for plugin '%s'", module->name );
			if( module->plugin->interface.api.database.otp.update != NULL ) {
				TT_DEBUG4( "library/plugin", "invoking 'otp_update' for plugin '%s'", module->name );
				switch( module->plugin->interface.api.database.otp.update( identifier, key, key_size, new_key, new_key_size, status ) ) {
					case TT_OK:
						TT_DEBUG4( "library/plugin", "plugin '%s' successfully handled 'otp_update'", module->name );
						if( libtokentube_runtime_broadcast( TT_EVENT__OTP_CREATED, identifier ) != TT_OK ) {
							TT_LOG_WARN( "library/plugin", "libtokentube_runtime_broadcast() failed in %s()", __FUNCTION__ );
						}
						return TT_OK;
					case TT_IGN:
						TT_DEBUG5( "library/plugin", "plugin '%s' ignored 'otp_update'", module->name );
						break;
					default:
						TT_LOG_ERROR( "library/plugin", "plugin '%s' returned error for 'otp_update'", module->name );
				}
			}
		}
	}
	TT_LOG_ERROR( "library/plugin", "no plugin handled 'otp_update', returning TT_ERR" );
	return TT_ERR;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_plugin__otp_delete(const char* identifier, tt_status_t* status) {
	tt_module_t*	module;
	size_t		i;

	TT_TRACE( "library/plugin", "%s(identifier='%s')", __FUNCTION__, identifier );
	if( identifier == NULL || identifier[0] == '\0' || status == NULL ) {
		TT_LOG_ERROR( "library/plugin", "invalid parameters in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	TT_DEBUG3( "library/plugin", "invoking 'otp_delete' handlers" );
	for( i=0; i<MAX_PLUGINS+1; i++ ) {
		module = g_modules[i];
		if( module != NULL && module->plugin != NULL ) {
			TT_DEBUG5( "library/plugin", "checking 'otp_delete' for plugin '%s'", module->name );
			if( module->plugin->interface.api.database.otp.delete != NULL ) {
				switch( module->plugin->interface.api.database.otp.delete( identifier, status ) ) {
					case TT_OK:
						TT_DEBUG4( "library/plugin", "plugin '%s' successfully handled 'otp_delete'", module->name );
						if( *status == TT_YES ) {
							if( libtokentube_runtime_broadcast( TT_EVENT__OTP_DELETED, identifier ) != TT_OK ) {
								TT_LOG_WARN( "library/plugin", "libtokentube_runtime_broadcast() failed in %s()", __FUNCTION__ );
							}
						}
						return TT_OK;
					case TT_IGN:
						TT_DEBUG4( "library/plugin", "plugin '%s' ignored 'otp_delete'", module->name );
						break;
					default:
						TT_DEBUG4( "library/plugin", "plugin '%s' returned error for 'otp_delete'", module->name );
				}
			}
		}
	}
	TT_LOG_ERROR( "library/plugin", "no plugin handled 'otp_delete', returning TT_ERR" );
	return TT_ERR;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_plugin__otp_exists(const char* identifier, tt_status_t* status) {
	tt_module_t*	module;
	size_t		i;

	TT_TRACE( "library/plugin", "%s(identifier='%s')", __FUNCTION__, identifier );
	if( identifier == NULL || identifier[0] == '\0' || status == NULL ) {
		TT_LOG_ERROR( "library/plugin", "invalid parameters in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	TT_DEBUG3( "library/plugin", "invoking 'otp_exists' handlers" );
	for( i=0; i<MAX_PLUGINS+1; i++ ) {
		module = g_modules[i];
		if( module != NULL && module->plugin != NULL ) {
			TT_DEBUG5( "library/plugin", "checking 'otp_exists' for plugin '%s'", module->name );
			if( module->plugin->interface.api.database.otp.exists != NULL ) {
				switch( module->plugin->interface.api.database.otp.exists( identifier, status ) ) {
					case TT_OK:
						TT_DEBUG4( "library/plugin", "plugin '%s' successfully handled 'otp_exists'", module->name );
						return TT_OK;
					case TT_IGN:
						TT_DEBUG4( "library/plugin", "plugin '%s' ignored 'otp_exists'", module->name );
						break;
					default:
						TT_DEBUG4( "library/plugin", "plugin '%s' returned error for 'otp_exists'", module->name );
				}
			}
		}
	}
	TT_LOG_ERROR( "library/plugin", "no plugin handled 'otp_exists', returning TT_ERR" );
	return TT_ERR;
}

