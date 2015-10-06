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
int libtokentube_plugin__auth_user_loadkey( const char* username, const char* password, const char* identifier, char* key, size_t* key_size ) {
	tt_module_t*	module;
	size_t		i;

	TT_TRACE( "library/plugin", "%s(username='%s' password='%s')", __FUNCTION__, username, password );
	if( username == NULL || username[0] == '\0' || password == NULL || password[0] == '\0' || key == NULL || key_size == NULL || *key_size == 0) {
		TT_LOG_FATAL( "library/plugin", "invalid parameters in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	TT_DEBUG3( "library/plugin", "invoking 'auth_user_loadkey' handlers" );
	for( i=0; i<MAX_PLUGINS+1; i++ ) {
		module = g_modules[i];
		if( module != NULL && module->plugin != NULL ) {
			TT_DEBUG5( "library/plugin", "checking 'auth_user_loadkey' for plugin '%s'", module->name );
			if( module->plugin->interface.api.auth.user.loadkey != NULL ) {
				TT_DEBUG4( "library/plugin", "invoking 'auth_user_loadkey' handler for plugin '%s'", module->name );
				switch( module->plugin->interface.api.auth.user.loadkey( username, password, identifier, key, key_size ) ) {
					case TT_OK:
						TT_DEBUG4( "library/plugin", "plugin '%s' successfully handled 'auth_user_loadkey'", module->name );
						if( libtokentube_runtime_broadcast( TT_EVENT__AUTH_USER, identifier ) != TT_OK ) {
							TT_LOG_WARN( "library/plugin", "libtokentube_runtime_broadcast() failed in %s()", __FUNCTION__ );
						}
						return TT_OK;
					case TT_IGN:
						TT_DEBUG4( "library/plugin", "plugin '%s' ignored 'auth_user_loadkey'", module->name );
						break;
					default:
						TT_LOG_ERROR( "library/plugin", "plugin '%s' returned error for 'auth_user_loadkey'", module->name );
				}
			}
		}
	}
	TT_LOG_ERROR( "library/plugin", "no plugin handled 'auth_user_loadkey', returning TT_ERR" );
	return TT_ERR;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_plugin__auth_user_verify( const char* username, const char* password, tt_status_t* status ) {
	tt_module_t*	module;
	size_t		i;

	TT_TRACE( "library/plugin", "%s(username='%s' password='%s')", __FUNCTION__, username, password );
	if( username == NULL || username[0] == '\0' || password == NULL || password[0] == '\0' || status == NULL ) {
		TT_LOG_FATAL( "library/plugin", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	TT_DEBUG3( "library/plugin", "invoking 'auth_user_verify' handlers" );
	*status = TT_STATUS__UNDEFINED;
	for( i=0; i<MAX_PLUGINS+1; i++ ) {
		module = g_modules[i];
		if( module != NULL && module->plugin != NULL ) {
			TT_DEBUG5( "library/plugin", "checking 'auth_user_verify' for plugin '%s'", module->name );
			if( module->plugin->interface.api.auth.user.verify != NULL ) {
				TT_DEBUG4( "library/plugin", "invoking 'auth_user_verify' handler for plugin '%s'", module->name );
				switch( module->plugin->interface.api.auth.user.verify( username, password, status ) ) {
					case TT_OK:
						TT_DEBUG4( "library/plugin", "plugin '%s' successfully handled 'auth_user_verify'", module->name );
						if( *status == TT_YES ) {
							if( libtokentube_runtime_broadcast( TT_EVENT__AUTH_USER, username ) != TT_OK ) {
								TT_LOG_WARN( "library/plugin", "libtokentube_runtime_broadcast() failed in %s()", __FUNCTION__ );
							}
						}
						return TT_OK;
					case TT_IGN:
						TT_DEBUG4( "library/plugin", "plugin '%s' ignored 'auth_user_verify'", module->name );
						break;
					default:
						TT_LOG_ERROR( "library/plugin", "plugin '%s' returned error for 'auth_user_verify'", module->name );
				}
			}
		}
	}
	TT_LOG_ERROR( "library/plugin", "no plugin handled 'auth_user_verify', returning TT_ERR" );
	return TT_ERR;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_plugin__auth_user_autoenrollment(const char* username, const char* password, tt_status_t* status) {
	tt_module_t*	module;
	size_t		i;

	TT_TRACE( "library/plugin", "%s(username='%s')", __FUNCTION__, username );
	if( username == NULL || username[0] == '\0' || password == NULL || password[0] == '\0' || status == NULL ) {
		TT_LOG_FATAL( "library/plugin", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	TT_DEBUG3( "library/plugin", "invoking 'auth_user_autoenrollment' handlers" );
	*status = TT_STATUS__UNDEFINED;
	for( i=0; i<MAX_PLUGINS+1; i++ ) {
		module = g_modules[i];
		if( module != NULL && module->plugin != NULL ) {
			TT_DEBUG5( "library/plugin", "checking 'user_autoenrollment' for plugin '%s'", module->name );
			if( module->plugin->interface.api.auth.user.autoenrollment != NULL ) {
				TT_DEBUG4( "library/plugin", "invoking 'auth_user_autoenrollment' handler for plugin '%s'", module->name );
				switch( module->plugin->interface.api.auth.user.autoenrollment( username, password, status ) ) {
					case TT_OK:
						TT_DEBUG4( "library/plugin", "plugin '%s' successfully handled 'auth_user_autoenrollment'", module->name );
						return TT_OK;
					case TT_IGN:
						TT_DEBUG4( "library/plugin", "plugin '%s' ignored 'auth_user_autoenrollment'", module->name );
						break;
					default:
						TT_LOG_ERROR( "library/plugin", "plugin '%s' returned error for 'auth_user_autoenrollment'", module->name );
				}
			}
		}
	}
	TT_LOG_ERROR( "library/plugin", "no plugin handled 'auth_user_autoenrollment', returning TT_ERR" );
	return TT_ERR;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_plugin__auth_otp_challenge(const char* identifier, char* challenge, size_t* challenge_size) {
	tt_module_t* module;
	size_t i;

	TT_TRACE( "library/plugin", "%s(identifier='%s')", __FUNCTION__, identifier );
	if( identifier == NULL || identifier[0] == '\0' || challenge == NULL || challenge_size == NULL || *challenge_size == 0 ) {
		TT_LOG_ERROR( "library/plugin", "invalid parameters in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	TT_DEBUG3( "library/plugin", "invoking 'auth_otp_challenge' handlers" );
	for( i=0; i<MAX_PLUGINS+1; i++ ) {
		module = g_modules[i];
		if( module != NULL && module->plugin != NULL ) {
			TT_DEBUG5( "library/plugin", "checking 'auth_otp_challenge' for plugin '%s'", module->name );
			if( module->plugin->interface.api.auth.otp.challenge != NULL ) {
				switch( module->plugin->interface.api.auth.otp.challenge( identifier, challenge, challenge_size ) ) {
					case TT_OK:
						TT_DEBUG4( "library/plugin", "plugin '%s' successfully handled 'auth_otp_challenge'", module->name );
						return TT_OK;
					case TT_IGN:
						TT_DEBUG4( "library/plugin", "plugin '%s' ignored 'auth_otp_challenge'", module->name );
						break;
					default:
						TT_DEBUG4( "library/plugin", "plugin '%s' returned error for 'auth_otp_challenge'", module->name );
				}
			}
		}
	}
	TT_LOG_ERROR( "library/plugin", "no plugin handled 'auth_otp_challenge', returning TT_ERR" );
	return TT_ERR;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_plugin__auth_otp_response( const char* identifier, const char* challenge, char* response, size_t* response_size ) {
	tt_module_t*	module;
	size_t		i;

	TT_TRACE( "library/plugin", "%s(identifier='%s',challenge='%s')", __FUNCTION__, identifier, challenge );
	if( identifier == NULL || identifier[0] == '\0' || challenge == NULL || response == NULL || response_size == NULL || *response_size == 0 ) {
		TT_LOG_ERROR( "library/plugin", "invalid parameters in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	TT_DEBUG3( "library/plugin", "invoking 'auth_otp_response' handlers" );
	for( i=0; i<MAX_PLUGINS+1; i++ ) {
		module = g_modules[i];
		if( module != NULL && module->plugin != NULL ) {
			TT_DEBUG5( "library/plugin", "checking 'auth_otp_response' for plugin '%s'", module->name );
			if( module->plugin->interface.api.auth.otp.response != NULL ) {
				switch( module->plugin->interface.api.auth.otp.response( identifier, challenge, response, response_size ) ) {
					case TT_OK:
						TT_DEBUG4( "library/plugin", "plugin '%s' successfully handled 'auth_otp_response'", module->name );
						return TT_OK;
					case TT_IGN:
						TT_DEBUG4( "library/plugin", "plugin '%s' ignored 'auth_otp_response'", module->name );
						break;
					default:
						TT_DEBUG4( "library/plugin", "plugin '%s' returned error for 'auth_otp_response'", module->name );
				}
			}
		}
	}
	TT_LOG_ERROR( "library/plugin", "no plugin handled 'auth_otp_response', returning TT_ERR" );
	return TT_ERR;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_plugin__auth_otp_loadkey( const char* identifier, const char* challenge, const char* response, char* key, size_t* key_size ) {
	tt_module_t*	module;
	size_t		i;

	TT_TRACE( "library/plugin", "%s(identifier='%s',challenge='%s',response='%s')", __FUNCTION__, identifier, challenge, response );
	if( identifier == NULL || identifier[0] == '\0' || challenge == NULL || response == NULL || key == NULL || key_size == NULL || *key_size == 0 ) {
		TT_LOG_ERROR( "library/plugin", "invalid parameters in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	TT_DEBUG3( "library/plugin", "invoking 'auth_otp_loadkey' handlers" );
	for( i=0; i<MAX_PLUGINS+1; i++ ) {
		module = g_modules[i];
		if( module != NULL && module->plugin != NULL ) {
			TT_DEBUG5( "library/plugin", "checking 'auth_otp_loadkey' for plugin '%s'", module->name );
			if( module->plugin->interface.api.auth.otp.loadkey != NULL ) {
				switch( module->plugin->interface.api.auth.otp.loadkey( identifier, challenge, response, key, key_size ) ) {
					case TT_OK:
						TT_DEBUG4( "library/plugin", "plugin '%s' successfully handled 'auth_otp_loadkey'", module->name );
						if( *key_size > 0 ) {
							if( libtokentube_runtime_broadcast( TT_EVENT__AUTH_OTP, identifier ) != TT_OK ) {
								TT_LOG_WARN( "library/plugin", "libtokentube_runtime_broadcast() failed in %s()", __FUNCTION__ );
							}
						}
						return TT_OK;
					case TT_IGN:
						TT_DEBUG4( "library/plugin", "plugin '%s' ignored 'auth_otp_loadkey'", module->name );
						break;
					default:
						TT_DEBUG4( "library/plugin", "plugin '%s' returned error for 'auth_otp_loadkey'", module->name );
				}
			}
		}
	}
	TT_LOG_ERROR( "library/plugin", "no plugin handled 'auth_otp_loadkey', returning TT_ERR" );
	return TT_ERR;
}

