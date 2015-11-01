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
		TT_LOG_ERROR( "library/plugin", "BUG: invalid invocation of API:event.broadcast()" );
		TT_DEBUG_SRC( "library/plugin", "invalid parameters" );
		return TT_ERR;
	}
 	TT_DEBUG3( "library/plugin", "broadcasting event=%d with identifier '%s'", (int)event, identifier );
	for( i=0; i<MAX_PLUGINS+1; i++ ) {
		if( g_modules[i] != NULL ) {
			module = g_modules[i];
			TT_DEBUG5( "library/plugin", "evaluating broadcast of event=%d with identifier '%s' for plugin '%s'", (int)event, identifier, module->name );
			switch( event ) {
				case TT_EVENT__USER_CREATED:
					event_func = module->plugin->interface.events.user.created;
					break;
				case TT_EVENT__USER_MODIFIED:
					event_func = module->plugin->interface.events.user.modified;
					break;
				case TT_EVENT__USER_DELETED:
					event_func = module->plugin->interface.events.user.deleted;
					break;
				case TT_EVENT__OTP_CREATED:
					event_func = module->plugin->interface.events.otp.created;
					break;
				case TT_EVENT__OTP_MODIFIED:
					event_func = module->plugin->interface.events.otp.modified;
					break;
				case TT_EVENT__OTP_DELETED:
					event_func = module->plugin->interface.events.otp.deleted;
					break;
				case TT_EVENT__UHD_CREATED:
					event_func = module->plugin->interface.events.uhd.created;
					break;
				case TT_EVENT__UHD_MODIFIED:
					event_func = module->plugin->interface.events.uhd.modified;
					break;
				case TT_EVENT__UHD_DELETED:
					event_func = module->plugin->interface.events.uhd.deleted;
					break;
				case TT_EVENT__AUTH_USER:
					event_func = module->plugin->interface.events.auth.user;
					break;
				case TT_EVENT__AUTH_OTP:
					event_func = module->plugin->interface.events.auth.otp;
					break;
				default:
					TT_LOG_ERROR( "library/plugin", "BUG: invalid event submitted to API:event.broadcast()" );
					TT_DEBUG_SRC( "library/plugin", "unknown event" );
					return TT_ERR;
			}
			if( event_func != NULL ) {
				TT_DEBUG3( "library/plugin", "sending event=%d with identifier '%s' to plugin '%s'", (int)event, identifier, module->name );
				event_func( identifier );
			}
		}
	}
	return TT_OK;
}

