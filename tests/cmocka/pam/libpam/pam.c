#include <stdio.h>
#include <string.h>

#define PAM_SM_SESSION

#include <security/pam_modules.h>
#include <security/pam_ext.h>


static void (*g_cleanup)(pam_handle_t *pamh, void *data, int error_status) = NULL;
static void*  g_cleanup_data = NULL;

static const char*	g_username = NULL;
static const char*	g_old_authtok = NULL;
static const char*	g_authtok = NULL;
static const char*	g_rekeying = NULL;


int pam_get_user(pam_handle_t* pamh __attribute__((unused)), const char** user, const char* prompt __attribute__((unused))) {
	if( user == NULL ) {
		return PAM_SERVICE_ERR;
	}
	*user = g_username;
	return PAM_SUCCESS;
}


int pam_get_item(const pam_handle_t* pamh __attribute__((unused)), int item_type, const void** item) {
	switch( item_type ) {
		case PAM_USER:
			*item = g_username;
			break;
		case PAM_OLDAUTHTOK:
			*item = g_old_authtok;
			break;
		case PAM_AUTHTOK:
			*item = g_authtok;
			break;
		default:
			return PAM_SERVICE_ERR;
	}
	return PAM_SUCCESS;
}


int pam_set_data(pam_handle_t *pamh __attribute__((unused)), const char *module_data_name, void *data, void (*cleanup)(pam_handle_t *pamh, void *data, int error_status)) {
	if( module_data_name != NULL && data != NULL ) {
		if( strncasecmp( module_data_name, "pam_tokentube_termination_helper", 33 ) == 0 ) {
			g_cleanup = cleanup;
			g_cleanup_data = data;
		}
	}
	if( module_data_name != NULL && data != NULL ) {
		if( strncasecmp( module_data_name, "user-reenrollment", 18 ) == 0 ) {
			g_rekeying = data;
		}
	}
	return PAM_SUCCESS;
}


int pam_get_data(const pam_handle_t* pamh __attribute__((unused)), const char* module_data_name __attribute__((unused)), const void** data __attribute__((unused))) {
	if( module_data_name != NULL && data != NULL ) {
		if( strncasecmp( module_data_name, "config", 7 ) == 0 ) {
			*data = "etc/tokentube/tokentube.conf";
		}
	}
	if( module_data_name != NULL && data != NULL ) {
		if( strncasecmp( module_data_name, "user-reenrollment", 18 ) == 0 ) {
			*data = g_rekeying;
		}
	}
	if( module_data_name != NULL && data != NULL ) {
		if( strncasecmp( module_data_name, "pam_tokentube_termination_helper", 33 ) == 0 ) {
			*data = g_cleanup_data;
		}
	}
	return PAM_SUCCESS;
}


void pam_syslog(const pam_handle_t *pamh __attribute__((unused)), int priority __attribute__((unused)), const char *fmt __attribute__((unused)), ...) {
        va_list args;

        va_start( args, fmt );
        va_end( args );
}


int pam_sm_setcred(pam_handle_t *pamh __attribute__((unused)), int flags, int argc __attribute__((unused)), const char **argv) {
	switch( flags ) {
		case PAM_NO_MODULE_DATA:
			g_rekeying = *argv;
			break;
		case PAM_USER:
			g_username = *argv;
			break;
		case PAM_AUTHTOK:
			g_authtok = *argv;
			break;
		case PAM_OLDAUTHTOK:
			g_old_authtok = *argv;
			break;
		default:
			return PAM_SERVICE_ERR;
	}
	return PAM_SUCCESS;
}

int pam_sm_close_session(pam_handle_t *pamh, int flags __attribute__((unused)), int argc __attribute__((unused)), const char **argv __attribute__((unused))) {
	if( g_cleanup != NULL ) {
		g_cleanup( pamh, g_cleanup_data, 0 );
	}
	return PAM_SUCCESS;
}

