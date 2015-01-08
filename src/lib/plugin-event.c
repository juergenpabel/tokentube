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
int libtokentube_plugin__event_broadcast(tt_event_t event, const char* identifier) {
	void		(*event_func)(const char*) = NULL;
	tt_module_t*	module = NULL;
	size_t		i = 0;

	TT_TRACE( "library/plugin", "%s(event=%d,identifier='%s')", __FUNCTION__, (int)event, identifier );
	if( event == TT_EVENT__UNDEFINED || identifier == NULL || identifier[0] == '\0' ) {
		TT_LOG_ERROR( "library/plugin", "invalid parameters in %s()", __FUNCTION__ );
		return TT_ERR;
	}
 	TT_DEBUG3( "library/plugin", "broadcasting event=%d with identifier '%s'", (int)event, identifier );
	for( i=0; i<MAX_PLUGINS+1; i++ ) {
		if( g_modules[i] != NULL ) {
			module = g_modules[i];
			TT_DEBUG5( "library/plugin", "checking event=%d with identifier '%s' for plugin '%s'", (int)event, identifier, module->name );
			switch( event ) {
				case TT_EVENT__USER_CREATED:
					event_func = module->plugin->interface.events.user.created;
					break;
				case TT_EVENT__USER_VERIFIED:
					event_func = module->plugin->interface.events.user.verified;
					break;
				case TT_EVENT__USER_UPDATED:
					event_func = module->plugin->interface.events.user.updated;
					break;
				case TT_EVENT__USER_DELETED:
					event_func = module->plugin->interface.events.user.deleted;
					break;
				case TT_EVENT__OTP_CREATED:
					event_func = module->plugin->interface.events.otp.created;
					break;
				case TT_EVENT__OTP_APPLIED:
					event_func = module->plugin->interface.events.otp.applied;
					break;
				case TT_EVENT__OTP_DELETED:
					event_func = module->plugin->interface.events.otp.deleted;
					break;
				default:
					TT_LOG_ERROR( "library/plugin", "unsupported event=%d in %s()", (int)event, __FUNCTION__ );
					return TT_ERR;
			}
			if( event_func != NULL ) {
				TT_DEBUG4( "library/plugin", "sending event=%d with identifier '%s' to plugin '%s'", (int)event, identifier, module->name );
				event_func( identifier );
			} else {
				TT_DEBUG5( "library/plugin", "skipping event=%d with identifier '%s' for plugin '%s'", (int)event, identifier, module->name );
			}
		}
	}
	return TT_OK;
}

