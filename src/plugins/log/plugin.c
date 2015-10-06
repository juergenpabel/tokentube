#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <tokentube/plugin.h>


static int initialize();
static int finalize();


static tt_plugin_t	g_self;
TT_PLUGIN_REGISTER( g_self, initialize, NULL, finalize )


static void log__event__user_created(const char* identifier) {
	g_self.library.api.runtime.system.log( TT_LOG__INFO, "plugin/log", "event 'USER-CREATED' received for identifier '%s'", identifier );
}


static void log__event__user_modified(const char* identifier) {
	g_self.library.api.runtime.system.log( TT_LOG__INFO, "plugin/log", "event 'USER-MODIFIED' received for identifier '%s'", identifier );
}


static void log__event__user_deleted(const char* identifier) {
	g_self.library.api.runtime.system.log( TT_LOG__INFO, "plugin/log", "event 'USER-DELETED' received for identifier '%s'", identifier );
}


static void log__event__otp_created(const char* identifier) {
	g_self.library.api.runtime.system.log( TT_LOG__INFO, "plugin/log", "event 'OTP-CREATED' received for identifier '%s'", identifier );
}


static void log__event__otp_modified(const char* identifier) {
	g_self.library.api.runtime.system.log( TT_LOG__INFO, "plugin/log", "event 'OTP-MODIFIED' received for identifier '%s'", identifier );
}


static void log__event__otp_deleted(const char* identifier) {
	g_self.library.api.runtime.system.log( TT_LOG__INFO, "plugin/log", "event 'OTP-DELETED' received for identifier '%s'", identifier );
}


static void log__event__uhd_created(const char* identifier) {
	g_self.library.api.runtime.system.log( TT_LOG__INFO, "plugin/log", "event 'UHD-CREATED' received for identifier '%s'", identifier );
}


static void log__event__uhd_modified(const char* identifier) {
	g_self.library.api.runtime.system.log( TT_LOG__INFO, "plugin/log", "event 'UHD-MODIFIED' received for identifier '%s'", identifier );
}


static void log__event__uhd_deleted(const char* identifier) {
	g_self.library.api.runtime.system.log( TT_LOG__INFO, "plugin/log", "event 'UHD-DELETED' received for identifier '%s'", identifier );
}


static void log__event__auth_user(const char* identifier) {
	g_self.library.api.runtime.system.log( TT_LOG__INFO, "plugin/log", "event 'AUTH-USER' received for identifier '%s'", identifier );
}


static void log__event__auth_otp(const char* identifier) {
	g_self.library.api.runtime.system.log( TT_LOG__INFO, "plugin/log", "event 'AUTH-OTP' received for identifier '%s'", identifier );
}


int initialize() {
	g_self.interface.events.user.created = log__event__user_created;
	g_self.interface.events.user.modified = log__event__user_modified;
	g_self.interface.events.user.deleted = log__event__user_deleted;
	g_self.interface.events.otp.created = log__event__otp_created;
	g_self.interface.events.otp.modified = log__event__otp_modified;
	g_self.interface.events.otp.deleted = log__event__otp_deleted;
	g_self.interface.events.uhd.created = log__event__uhd_created;
	g_self.interface.events.uhd.modified = log__event__uhd_modified;
	g_self.interface.events.uhd.deleted = log__event__uhd_deleted;
	g_self.interface.events.auth.user = log__event__auth_user;
	g_self.interface.events.auth.otp = log__event__auth_otp;
	return TT_OK;
}


int finalize() {
	return TT_OK;
}

