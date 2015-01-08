#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <sys/types.h>

#define PAM_SM_SESSION	

#include <security/pam_modules.h>
#include <security/pam_ext.h>
#include <tokentube.h>


static int  pam_tokentube_initialize(pam_handle_t* pamh);
static void pam_tokentube_finalize(pam_handle_t* pamh, void* data, int error_status);

static tt_library_t	g_library;
static int		g_initialized = TT_NO;
static char*		g_pam_tokentube_termination_helper = "pam_tokentube_termination_helper";
static char*		g_pam_tokentube_user_recreate = "g_pam_tokentube_user_recreate";


static int pam_tokentube_initialize(pam_handle_t* pamh) {
	const char*	config = NULL;

	if( g_initialized == TT_YES ) {
		return TT_OK;
	}
	pam_set_data( pamh, g_pam_tokentube_termination_helper, g_pam_tokentube_termination_helper, pam_tokentube_finalize );
	if( tt_initialize( TT_VERSION ) != TT_OK ) {
		pam_syslog( pamh, LOG_ERR, "pam_tokentube: tt_initialize() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( pam_get_data( pamh, "config", (const void**)&config ) != PAM_SUCCESS ) {
		pam_syslog( pamh, LOG_ERR, "pam_tokentube: pam_get_data() returned nothing for 'user-reenrollment' in %s()", __FUNCTION__ );
		return PAM_IGNORE; /* avoid impact on authentication process by not returning PAM_SERVICE_ERR */
	}
	if( tt_configure( config ) != TT_OK ) {
		pam_syslog( pamh, LOG_ERR, "pam_tokentube: tt_configure() failed in %s()", __FUNCTION__ );
		tt_finalize();
		return TT_ERR;
	}
	if( tt_discover( &g_library ) != TT_OK ) {
		pam_syslog( pamh, LOG_ERR, "pam_tokentube: tt_discover() failed in %s()", __FUNCTION__ );
		tt_finalize();
		return TT_ERR;
	}
	g_initialized = TT_YES;
	return TT_OK;
}


static void pam_tokentube_finalize(pam_handle_t* pamh __attribute__((unused)), void* data __attribute__((unused)), int error_status) {
	if( g_initialized != TT_YES ) {
		return;
	}
//TODO	if( data != g_pam_tokentube_termination_helper ) {
//TODO		pam_syslog( pamh, LOG_ERR, "pam_tokentube: data!=g_pam_tokentube_termination_helper in pam_tokentube_finalize()" );
//TODO		return;
//TODO	}
	if( (error_status & PAM_DATA_REPLACE) == 0 ) {
		if( tt_finalize() != TT_OK ) {
			g_initialized = TT_STATUS__UNDEFINED;
			return;
		}
		g_initialized = TT_NO;
	}
}


PAM_EXTERN int pam_sm_chauthtok(pam_handle_t *pamh, int flags, int argc __attribute__((unused)), const char **argv __attribute__((unused))) {
	tt_status_t	status = TT_STATUS__UNDEFINED;
	const char*	username = NULL;
	const char*	old_password = NULL;
	const char*	new_password = NULL;
	
	if( flags & PAM_PRELIM_CHECK ) {
		pam_syslog( pamh, LOG_DEBUG, "pam_sm_chauthtok() - PRELIM" );
		return PAM_IGNORE;
	}
	if( pam_tokentube_initialize( pamh ) != TT_OK ) {
		pam_syslog( pamh, LOG_ERR, "pam_tokentube: pam_tokentube_initialize() returned error in %s()", __FUNCTION__ );
		return PAM_SERVICE_ERR;
	}
	if( pam_get_user( pamh, &username, NULL ) != PAM_SUCCESS || username == NULL || username[0] == '\0' ) {
		pam_syslog( pamh, LOG_ERR, "pam_tokentube: pam_get_user() returned error in %s()", __FUNCTION__ );
		g_library.api.runtime.log( TT_LOG__ERROR, "pam_tokentube", "pam_get_user() returned error in %s()", __FUNCTION__ );
		return PAM_SERVICE_ERR;
	}
	if( pam_get_item( pamh, PAM_OLDAUTHTOK, (const void**)&old_password ) != PAM_SUCCESS ) {
		pam_syslog( pamh, LOG_DEBUG, "pam_tokentube: pam_get_item() unsuccessful for PAM_OLDAUTHTOK in %s()", __FUNCTION__ );
		g_library.api.runtime.debug( TT_DEBUG__VERBOSITY1, "pam_tokentube", "pam_get_item() unsuccessful for PAM_OLDAUTHTOK in %s()", __FUNCTION__ );
	}
	if( pam_get_item( pamh, PAM_AUTHTOK, (const void**)&new_password ) != PAM_SUCCESS ) {
		pam_syslog( pamh, LOG_ERR, "pam_tokentube: pam_get_item() unsuccessful for PAM_AUTHTOK in %s()", __FUNCTION__ );
		g_library.api.runtime.log( TT_LOG__ERROR, "pam_tokentube", "pam_get_item() unsuccessful for PAM_AUTHTOK in %s()", __FUNCTION__ );
		return PAM_SERVICE_ERR;
	}
	if( g_library.api.user.exists( username, &status ) != TT_OK ) {
		pam_syslog( pamh, LOG_ERR, "pam_tokentube: API:user.exists() returned error for user '%s' in %s()", username, __FUNCTION__ );
		return PAM_SERVICE_ERR;
	}
	if( status == TT_STATUS__YES ) {
		if( old_password != NULL ) {
			if( g_library.api.user.update( username, old_password, new_password, &status ) != TT_OK ) {
				g_library.api.runtime.log( TT_LOG__ERROR, "pam_tokentube", "API:user.update() failed for user '%s' in %s()", username, __FUNCTION__ );
				return PAM_SERVICE_ERR;
			}
			if( status == TT_YES ) {
				g_library.api.runtime.debug( TT_DEBUG__VERBOSITY2, "pam_tokentube", "API:user.update() successful for user '%s' in %s()", username, __FUNCTION__ );
				return PAM_SUCCESS;
			}
			g_library.api.runtime.log( TT_LOG__WARN, "pam_tokentube", "API:user.update() unsuccessful for user '%s' in %s()", username, __FUNCTION__ );
		}
		if( g_library.api.user.delete( username, &status ) != TT_OK ) {
			g_library.api.runtime.log( TT_LOG__ERROR, "pam_tokentube", "API:user.delete() failed in %s()", __FUNCTION__ );
			return PAM_SERVICE_ERR;
		}
		if( g_library.api.user.create( username, new_password ) != TT_OK ) {
			g_library.api.runtime.log( TT_LOG__ERROR, "pam_tokentube", "API:user.create() failed in %s()", __FUNCTION__ );
			return PAM_SERVICE_ERR;
		}
		return PAM_SUCCESS;
	}
	if( g_library.api.user.execute_autoenrollment( username, new_password, &status ) != TT_OK ) {
		g_library.api.runtime.log( TT_LOG__ERROR, "pam_tokentube", "API:user.execute_autoenrollment() returned error for user '%s' in %s()", username, __FUNCTION__ );
		return PAM_IGNORE; /* avoid impact on authentication process by not returning PAM_SERVICE_ERR */
	}
	if( status == TT_YES ) {
		g_library.api.runtime.log( TT_LOG__INFO, "pam_tokentube", "user '%s' enrolled in %s()", username, __FUNCTION__ );
		return PAM_SUCCESS;
	}
	g_library.api.runtime.debug( TT_DEBUG__VERBOSITY3, "pam_tokentube", "API:user.execute_autoenrollment() did not enroll user '%s' in %s()", username, __FUNCTION__ );
	return PAM_IGNORE;
}


PAM_EXTERN int pam_sm_authenticate(pam_handle_t *pamh, int flags __attribute__((unused)), int argc __attribute__((unused)), const char **argv __attribute__((unused))) {
	tt_status_t	status = TT_STATUS__UNDEFINED;
	const char*	username = NULL;
	const char*	password = NULL;

	pam_set_data( pamh, g_pam_tokentube_user_recreate, NULL, NULL );
	if( pam_tokentube_initialize( pamh ) != TT_OK ) {
		pam_syslog( pamh, LOG_ERR, "pam_tokentube: pam_tokentube_initialize() returned error in %s()", __FUNCTION__ );
		return PAM_IGNORE; /* avoid impact on authentication process by not returning PAM_SERVICE_ERR */
	}
	if( pam_get_user( pamh, &username, NULL ) != PAM_SUCCESS || username == NULL || username[0] == '\0' ) {
		pam_syslog( pamh, LOG_ERR, "pam_tokentube: pam_get_user() returned error in %s()", __FUNCTION__ );
		g_library.api.runtime.log( TT_LOG__ERROR, "pam_tokentube", "pam_get_user() returned error in %s()", __FUNCTION__ );
		return PAM_IGNORE; /* avoid impact on authentication process by not returning PAM_SERVICE_ERR */
	}
	if( pam_get_item( pamh, PAM_AUTHTOK, (const void**)&password ) != PAM_SUCCESS ) {
		pam_syslog( pamh, LOG_ERR, "pam_tokentube: pam_get_item() unsuccessful for PAM_AUTHTOK for user '%s' in %s()", username, __FUNCTION__ );
		g_library.api.runtime.log( TT_LOG__ERROR, "pam_tokentube", "pam_get_item() unsuccessful for PAM_AUTHTOK for user '%s' in %s()", username, __FUNCTION__ );
		return PAM_IGNORE; /* avoid impact on authentication process by not returning PAM_SERVICE_ERR */
	}
	if( g_library.api.user.exists( username, &status ) != TT_OK ) {
		pam_syslog( pamh, LOG_ERR, "pam_tokentube: API:user.exists() returned error for user '%s' in %s()", username, __FUNCTION__ );
		return PAM_IGNORE; /* avoid impact on authentication process by not returning PAM_SERVICE_ERR */
	}
	switch( status ) {
		case TT_STATUS__YES:
			if( g_library.api.user.execute_verify( username, password, &status ) != TT_OK ) {
				pam_syslog( pamh, LOG_ERR, "pam_tokentube: API:user.execute_verify() returned error for user '%s' in %s()", username, __FUNCTION__ );
				return PAM_IGNORE; /* avoid impact on authentication process by not returning PAM_SERVICE_ERR */
			}
			if( status != TT_YES ) {
				g_library.api.runtime.debug( TT_DEBUG__VERBOSITY2, "pam_tokentube", "API:user.execute_verify() failed for user '%s' in %s()", username, __FUNCTION__ );
				if( pam_set_data( pamh, g_pam_tokentube_user_recreate, (char*)username, NULL ) != PAM_SUCCESS ) {
					pam_syslog( pamh, LOG_ERR, "pam_set_data() failed for user '%s' in %s()", username, __FUNCTION__ );
				}
				return PAM_AUTHTOK_ERR;
			}
			g_library.api.runtime.debug( TT_DEBUG__VERBOSITY2, "pam_tokentube", "API:user.execute_verify() successful for user '%s' in %s()", username, __FUNCTION__ );
			return PAM_SUCCESS;
		case TT_STATUS__NO:
			g_library.api.runtime.debug( TT_DEBUG__VERBOSITY2, "pam_tokentube", "API:user.exists() returned 'NO' for user '%s' in %s()", username, __FUNCTION__ );
			return PAM_IGNORE;
		default:
			pam_syslog( pamh, LOG_ERR, "pam_tokentube: API:user.exists() returned unknown status for user '%s' in %s()", username, __FUNCTION__ );
			g_library.api.runtime.log( TT_LOG__ERROR, "pam_tokentube", "API:user.exists() returned unknown status for user '%s' in %s()", username, __FUNCTION__ );
	}
	return PAM_IGNORE; /* avoid impact on authentication process by not returning PAM_SERVICE_ERR */
}


PAM_EXTERN int pam_sm_setcred(pam_handle_t *pamh __attribute__((unused)), int flags __attribute__((unused)), int argc __attribute__((unused)), const char **argv __attribute__((unused))) {
	return PAM_IGNORE;
}


PAM_EXTERN int pam_sm_open_session(pam_handle_t *pamh, int flags __attribute__((unused)), int argc __attribute__((unused)), const char **argv __attribute__((unused))) {
	tt_status_t	status = TT_STATUS__UNDEFINED;
	const char*	username = NULL;
	const char*	password = NULL;
	const char*	recreate = NULL;

	pam_syslog( pamh, LOG_ERR, "pam_tokentube: %s()", __FUNCTION__ );
	if( pam_get_user( pamh, &username, NULL ) != PAM_SUCCESS || username == NULL || username[0] == '\0' ) {
		pam_syslog( pamh, LOG_ERR, "pam_tokentube: pam_get_user() returned error in %s()", __FUNCTION__ );
		g_library.api.runtime.log( TT_LOG__ERROR, "pam_tokentube", "pam_get_user() returned error in %s()", __FUNCTION__ );
		return PAM_IGNORE; /* avoid impact on authentication process by not returning PAM_SERVICE_ERR */
	}
	if( pam_get_item( pamh, PAM_AUTHTOK, (const void**)&password ) != PAM_SUCCESS || password == NULL || password[0] == '\0' ) {
		pam_syslog( pamh, LOG_ERR, "pam_tokentube: pam_get_item() unsuccessful for PAM_AUTHTOK for user '%s' in %s()", username, __FUNCTION__ );
		g_library.api.runtime.log( TT_LOG__ERROR, "pam_tokentube", "pam_get_item() unsuccessful for PAM_AUTHTOK for user '%s' in %s()", username, __FUNCTION__ );
		return PAM_IGNORE; /* avoid impact on authentication process by not returning PAM_SERVICE_ERR */
	}
	if( g_library.api.user.exists( username, &status ) != TT_OK ) {
		pam_syslog( pamh, LOG_ERR, "pam_tokentube: API:user.exists() returned error for user '%s' in %s()", username, __FUNCTION__ );
		return PAM_IGNORE; /* avoid impact on authentication process by not returning PAM_SERVICE_ERR */
	}
	if( status == TT_STATUS__YES ) {
		g_library.api.runtime.debug( TT_DEBUG__VERBOSITY4, "pam_tokentube", "API:user.exists() successful for user '%s' in %s()", username, __FUNCTION__ );
		if( pam_get_data( pamh, g_pam_tokentube_user_recreate, (const void**)&recreate ) == PAM_SUCCESS && recreate != NULL ) {
			if( username == recreate || strncmp( username, recreate, TT_USERNAME_CHAR_MAX+1 ) == 0 ) {
				g_library.api.runtime.debug( TT_DEBUG__VERBOSITY3, "pam_tokentube", "recreating user '%s' in %s()", username, __FUNCTION__ );
				if( g_library.api.user.delete( username, &status ) != TT_OK || status != TT_STATUS__YES ) {
					g_library.api.runtime.log( TT_LOG__ERROR, "pam_tokentube", "API:user.delete() failed in %s()", __FUNCTION__ );
					return PAM_IGNORE; /* avoid impact on authentication process by not returning PAM_SERVICE_ERR */
				}
				if( g_library.api.user.create( username, password ) != TT_OK ) {
					g_library.api.runtime.log( TT_LOG__ERROR, "pam_tokentube", "API:user.create() failed in %s()", __FUNCTION__ );
					return PAM_IGNORE; /* avoid impact on authentication process by not returning PAM_SERVICE_ERR */
				}
				g_library.api.runtime.debug( TT_DEBUG__VERBOSITY2, "pam_tokentube", "recreated user '%s' in %s()", username, __FUNCTION__ );
				return PAM_SUCCESS;
			}
		}
		return PAM_IGNORE;
	}
	if( g_library.api.user.execute_autoenrollment( username, password, &status ) != TT_OK ) {
		g_library.api.runtime.log( TT_LOG__ERROR, "pam_tokentube", "API:user.execute_autoenrollment() returned error for user '%s' in %s()", username, __FUNCTION__ );
		return PAM_IGNORE; /* avoid impact on authentication process by not returning PAM_SERVICE_ERR */
	}
	if( status == TT_STATUS__YES ) {
		g_library.api.runtime.log( TT_LOG__INFO, "pam_tokentube", "user '%s' enrolled in %s()", username, __FUNCTION__ );
		return PAM_SUCCESS;
	}
	g_library.api.runtime.debug( TT_DEBUG__VERBOSITY2, "pam_tokentube", "API:user.execute_autoenrollment() did not enroll user '%s' in %s()", username, __FUNCTION__ );
	return PAM_IGNORE;
}


PAM_EXTERN int pam_sm_close_session(pam_handle_t *pamh, int flags __attribute__((unused)), int argc __attribute__((unused)), const char **argv __attribute__((unused))) {
	pam_set_data( pamh, g_pam_tokentube_user_recreate, NULL, NULL );
	return PAM_IGNORE;
}

