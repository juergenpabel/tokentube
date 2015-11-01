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
int libtokentube_plugin__otp_create(const char* identifier, tt_status_t* status) {
	tt_module_t*	module;
	size_t		i;

	TT_TRACE( "library/plugin", "%s(identifier='%s')", __FUNCTION__, identifier );
	if( identifier == NULL || identifier[0] == '\0' || status == NULL ) {
		TT_LOG_ERROR( "library/plugin", "BUG: invalid invocation of API:database.otp.create()" );
		TT_DEBUG_SRC( "library/plugin", "invalid parameters" );
		return TT_ERR;
	}
	*status = TT_STATUS__UNDEFINED;
	for( i=0; i<MAX_PLUGINS+1; i++ ) {
		module = g_modules[i];
		if( module != NULL && module->plugin != NULL ) {
			if( module->plugin->interface.api.database.otp.create != NULL ) {
				TT_DEBUG3( "library/plugin", "invoking database.otp.create('%s') on plugin '%s'", identifier, module->name );
				switch( module->plugin->interface.api.database.otp.create( identifier, status ) ) {
					case TT_OK:
						TT_DEBUG4( "library/plugin", "plugin '%s' successfully processed database.otp.create()", module->name );
						if( *status == TT_STATUS__YES ) {
							if( libtokentube_runtime_broadcast( TT_EVENT__OTP_CREATED, identifier ) != TT_OK ) {
								TT_LOG_WARN( "library/plugin", "libtokentube_runtime_broadcast() failed in %s()", __FUNCTION__ );
							}
						}
						return TT_OK;
					case TT_IGN:
						TT_DEBUG4( "library/plugin", "plugin '%s' returned TT_IGN for database.otp.create()", module->name );
						break;
					default:
						TT_LOG_ERROR( "library/plugin", "plugin '%s' returned TT_ERR for database.otp.create()", module->name );
				}
			}
		}
	}
	TT_LOG_ERROR( "library/plugin", "no plugin handled database.otp.create(), returning TT_ERR" );
	return TT_ERR;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_plugin__otp_modify(const char* identifier, tt_modify_t action, void* data, tt_status_t* status) {
	tt_module_t*	module;
	size_t		i;

	TT_TRACE( "library/plugin", "%s(identifier='%s')", __FUNCTION__, identifier );
	if( identifier == NULL || identifier[0] == '\0' || data == NULL || status == NULL ) {
		TT_LOG_ERROR( "library/plugin", "BUG: invalid invocation of API:database.otp.modify()" );
		TT_DEBUG_SRC( "library/plugin", "invalid parameters" );
		return TT_ERR;
	}
	*status = TT_STATUS__UNDEFINED;
	for( i=0; i<MAX_PLUGINS+1; i++ ) {
		module = g_modules[i];
		if( module != NULL && module->plugin != NULL ) {
			if( module->plugin->interface.api.database.otp.modify != NULL ) {
				TT_DEBUG3( "library/plugin", "invoking database.otp.modify('%s') on plugin '%s'", identifier, module->name );
				switch( module->plugin->interface.api.database.otp.modify( identifier, action, data, status ) ) {
					case TT_OK:
						TT_DEBUG4( "library/plugin", "plugin '%s' successfully processed database.otp.modify()", module->name );
						if( *status == TT_STATUS__YES ) {
							if( libtokentube_runtime_broadcast( TT_EVENT__OTP_CREATED, identifier ) != TT_OK ) {
								TT_LOG_WARN( "library/plugin", "libtokentube_runtime_broadcast() failed in %s()", __FUNCTION__ );
							}
						}
						return TT_OK;
					case TT_IGN:
						TT_DEBUG4( "library/plugin", "plugin '%s' returned TT_IGN for database.otp.modify()", module->name );
						break;
					default:
						TT_LOG_ERROR( "library/plugin", "plugin '%s' returned TT_ERR for database.otp.modify()", module->name );
				}
			}
		}
	}
	TT_LOG_ERROR( "library/plugin", "no plugin handled database.otp.modify(), returning TT_ERR" );
	return TT_ERR;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_plugin__otp_delete(const char* identifier, tt_status_t* status) {
	tt_module_t*	module;
	size_t		i;

	TT_TRACE( "library/plugin", "%s(identifier='%s')", __FUNCTION__, identifier );
	if( identifier == NULL || identifier[0] == '\0' || status == NULL ) {
		TT_LOG_ERROR( "library/plugin", "BUG: invalid invocation of API:database.otp.modify()" );
		TT_DEBUG_SRC( "library/plugin", "invalid parameters" );
		return TT_ERR;
	}
	*status = TT_STATUS__UNDEFINED;
	for( i=0; i<MAX_PLUGINS+1; i++ ) {
		module = g_modules[i];
		if( module != NULL && module->plugin != NULL ) {
			if( module->plugin->interface.api.database.otp.delete != NULL ) {
				TT_DEBUG3( "library/plugin", "invoking database.otp.delete('%s') on plugin '%s'", identifier, module->name );
				switch( module->plugin->interface.api.database.otp.delete( identifier, status ) ) {
					case TT_OK:
						TT_DEBUG4( "library/plugin", "plugin '%s' successfully processed database.otp.delete()", module->name );
						if( *status == TT_YES ) {
							if( libtokentube_runtime_broadcast( TT_EVENT__OTP_DELETED, identifier ) != TT_OK ) {
								TT_LOG_WARN( "library/plugin", "libtokentube_runtime_broadcast() failed in %s()", __FUNCTION__ );
							}
						}
						return TT_OK;
					case TT_IGN:
						TT_DEBUG4( "library/plugin", "plugin '%s' returned TT_IGN for database.otp.delete()", module->name );
						break;
					default:
						TT_DEBUG4( "library/plugin", "plugin '%s' returned TT_ERR for database.otp.delete()", module->name );
				}
			}
		}
	}
	TT_LOG_ERROR( "library/plugin", "no plugin handled database.otp.delete(), returning TT_ERR" );
	return TT_ERR;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_plugin__otp_exists(const char* identifier, tt_status_t* status) {
	tt_module_t*	module;
	size_t		i;

	TT_TRACE( "library/plugin", "%s(identifier='%s')", __FUNCTION__, identifier );
	if( identifier == NULL || identifier[0] == '\0' || status == NULL ) {
		TT_LOG_ERROR( "library/plugin", "BUG: invalid invocation of API:database.otp.exists()" );
		TT_DEBUG_SRC( "library/plugin", "invalid parameters" );
		return TT_ERR;
	}
	*status = TT_STATUS__UNDEFINED;
	for( i=0; i<MAX_PLUGINS+1; i++ ) {
		module = g_modules[i];
		if( module != NULL && module->plugin != NULL ) {
			if( module->plugin->interface.api.database.otp.exists != NULL ) {
				TT_DEBUG3( "library/plugin", "invoking database.otp.exists('%s') on plugin '%s'", identifier, module->name );
				switch( module->plugin->interface.api.database.otp.exists( identifier, status ) ) {
					case TT_OK:
						TT_DEBUG4( "library/plugin", "plugin '%s' successfully processed database.otp.delete()", module->name );
						return TT_OK;
					case TT_IGN:
						TT_DEBUG4( "library/plugin", "plugin '%s' returned TT_IGN for database.otp.delete()", module->name );
						break;
					default:
						TT_DEBUG4( "library/plugin", "plugin '%s' returned TT_ERR for database.otp.delete()", module->name );
				}
			}
		}
	}
	TT_LOG_ERROR( "library/plugin", "no plugin handled database.otp.delete(), returning TT_ERR" );
	return TT_ERR;
}

