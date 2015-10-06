#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <tokentube/plugin.h>


static int initialize();
static int finalize();


static tt_plugin_t	g_self;
TT_PLUGIN_REGISTER( g_self, initialize, NULL, finalize )


static void log__event__user_created(const char* identifier) {
	g_self.library.api.runtime.system.log( TT_LOG__INFO, "plugin/log", "event 'USER_CREATED' received for identifier '%s'", identifier );
}


static void log__event__user_updated(const char* identifier) {
	g_self.library.api.runtime.system.log( TT_LOG__INFO, "plugin/log", "event 'USER_UPDATED' received for identifier '%s'", identifier );
}


static void log__event__user_verified(const char* identifier) {
	g_self.library.api.runtime.system.log( TT_LOG__INFO, "plugin/log", "event 'USER_VERIFIED' received for identifier '%s'", identifier );
}


static void log__event__user_deleted(const char* identifier) {
	g_self.library.api.runtime.system.log( TT_LOG__INFO, "plugin/log", "event 'USER_DELETED' received for identifier '%s'", identifier );
}


static void log__event__otp_created(const char* identifier) {
	g_self.library.api.runtime.system.log( TT_LOG__INFO, "plugin/log", "event 'OTP_CREATED' received for identifier '%s'", identifier );
}


static void log__event__otp_applied(const char* identifier) {
	g_self.library.api.runtime.system.log( TT_LOG__INFO, "plugin/log", "event 'OTP_APPLIED' received for identifier '%s'", identifier );
}


static void log__event__otp_deleted(const char* identifier) {
	g_self.library.api.runtime.system.log( TT_LOG__INFO, "plugin/log", "event 'OTP_DELETED' received for identifier '%s'", identifier );
}


int initialize() {
	g_self.interface.events.user.created = log__event__user_created;
	g_self.interface.events.user.modified = log__event__user_updated;
	g_self.interface.events.user.deleted = log__event__user_deleted;
	g_self.interface.events.otp.created = log__event__otp_created;
	g_self.interface.events.otp.deleted = log__event__otp_deleted;
	g_self.interface.events.auth.user_verified = log__event__user_verified;
	g_self.interface.events.auth.otp_applied = log__event__otp_applied;
	return TT_OK;
}


int finalize() {
	return TT_OK;
}

