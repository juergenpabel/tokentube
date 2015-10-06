#include <stdlib.h>
#include "libtokentube.h"


static char* g_event_names[] = {
	NULL,
	"USER-CREATED",
	"USER-MODIFIED",
	"USER-DELETED",
	"OTP-CREATED",
	"OTP-MODIFIED",
	"OTP-DELETED",
	"UHD-CREATED",
	"UHD-MODIFIED",
	"UHD-DELETED",
	"AUTH-USER",
	"AUTH-OTP",
	NULL
};


__attribute__ ((visibility ("hidden")))
int libtokentube_event_initialize() {
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_event_configure() {
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_event_finalize() {
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_runtime_broadcast(tt_event_t event, const char* identifier) {
	char*	event_name = NULL;
	int	result = TT_ERR;

	TT_TRACE( "library/runtime", "%s(event=%d, identifier='%s')", __FUNCTION__, event, identifier );
	switch( event ) {
		case TT_EVENT__USER_CREATED:
		case TT_EVENT__USER_MODIFIED:
		case TT_EVENT__USER_DELETED:
		case TT_EVENT__OTP_CREATED:
		case TT_EVENT__OTP_MODIFIED:
		case TT_EVENT__OTP_DELETED:
		case TT_EVENT__UHD_CREATED:
		case TT_EVENT__UHD_MODIFIED:
		case TT_EVENT__UHD_DELETED:
		case TT_EVENT__AUTH_USER:
		case TT_EVENT__AUTH_OTP:
			event_name = g_event_names[event];
			break;
		default:
			TT_LOG_ERROR( "library/runtime", "unsupported event=%d in %s()", (int)event, __FUNCTION__ );
			return TT_ERR;
	}
	TT_DEBUG2( "library/runtime", "broadcasting event '%s' for identifier '%s'...", event_name, identifier );
	result = libtokentube_plugin__event_broadcast( event, identifier );
	TT_DEBUG2( "library/runtime", "...broadcasted event '%s' for identifier '%s'", event_name, identifier );
	return result;
}

