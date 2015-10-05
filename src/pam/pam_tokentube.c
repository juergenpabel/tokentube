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
		pam_syslog( pamh, LOG_ERR, "pam_tokentube: failed to initialize libtokentube" );
		return TT_ERR;
	}
	if( pam_get_data( pamh, "config", (const void**)&config ) == PAM_SUCCESS ) {
		pam_syslog( pamh, LOG_INFO, "pam_tokentube: using '%s' as tokentube config", config );
	}
	if( tt_configure( config ) != TT_OK ) {
		pam_syslog( pamh, LOG_ERR, "pam_tokentube: failed to configure libtokentube" );
		tt_finalize();
		return TT_ERR;
	}
	if( tt_discover( &g_library ) != TT_OK ) {
		pam_syslog( pamh, LOG_ERR, "pam_tokentube: failed to discover libtokentube" );
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
	if( data != g_pam_tokentube_termination_helper && strncmp( data, g_pam_tokentube_termination_helper, 32 ) != 0 ) {
		pam_syslog( pamh, LOG_ERR, "pam_tokentube: internal error in pam_tokentube_finalize()" );
		return;
	}
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
		pam_syslog( pamh, LOG_ERR, "pam_tokentube: initialization failure" );
		return PAM_SERVICE_ERR;
	}
	if( pam_get_user( pamh, &username, NULL ) != PAM_SUCCESS || username == NULL || username[0] == '\0' ) {
		pam_syslog( pamh, LOG_ERR, "pam_tokentube: failed to get username from PAM master for chauthtok" );
		g_library.api.runtime.system.debug( TT_DEBUG__VERBOSITY1, "pam/passwd", "failed to get username from PAM master" );
		return PAM_SERVICE_ERR;
	}
	if( pam_get_item( pamh, PAM_OLDAUTHTOK, (const void**)&old_password ) != PAM_SUCCESS ) {
		pam_syslog( pamh, LOG_DEBUG, "pam_tokentube: failed to get old password from PAM master for chauthtok" );
		g_library.api.runtime.system.debug( TT_DEBUG__VERBOSITY1, "pam/passwd", "pam_get_item() failed for PAM_OLDAUTHTOK in %s()", __FUNCTION__ );
	}
	if( pam_get_item( pamh, PAM_AUTHTOK, (const void**)&new_password ) != PAM_SUCCESS ) {
		pam_syslog( pamh, LOG_ERR, "pam_tokentube: failed to get new password from PAM master for chauthtok" );
		g_library.api.runtime.system.debug( TT_DEBUG__VERBOSITY1, "pam/passwd", "pam_get_item() failed for PAM_AUTHTOK in %s()", __FUNCTION__ );
		return PAM_SERVICE_ERR;
	}
	if( g_library.api.database.user.exists( username, &status ) != TT_OK ) {
		pam_syslog( pamh, LOG_ERR, "pam_tokentube: API:user.exists() returned error for user '%s' for chauthtok", username );
		g_library.api.runtime.system.debug( TT_DEBUG__VERBOSITY1, "pam/passwd", "API:user.exists() failed for user '%s' in %s()", username, __FUNCTION__ );
		return PAM_SERVICE_ERR;
	}
	if( status == TT_STATUS__YES ) {
		if( old_password != NULL ) {
			if( g_library.api.database.user.update( username, old_password, new_password, &status ) != TT_OK ) {
				g_library.api.runtime.system.log( TT_LOG__ERROR, "pam/passwd", "API:user.update() failed for user '%s'", username );
				return PAM_SERVICE_ERR;
			}
			if( status == TT_YES ) {
				g_library.api.runtime.system.debug( TT_DEBUG__VERBOSITY3, "pam/passwd", "API:user.update() successful for user '%s' in %s()", username, __FUNCTION__ );
				return PAM_SUCCESS;
			}
			g_library.api.runtime.system.log( TT_LOG__WARN, "pam/passwd", "API:user.update() unsuccessful for user '%s'", username );
		}
		if( g_library.api.database.user.delete( username, &status ) != TT_OK ) {
			g_library.api.runtime.system.log( TT_LOG__ERROR, "pam/passwd", "API:user.delete() failed for user '%s'", username);
			return PAM_SERVICE_ERR;
		}
		if( g_library.api.database.user.create( username, new_password ) != TT_OK ) {
			g_library.api.runtime.system.log( TT_LOG__ERROR, "pam/passwd", "API:user.create() failed for user '%s'", username);
			return PAM_SERVICE_ERR;
		}
		return PAM_SUCCESS;
	}
	status = TT_STATUS__UNDEFINED;
	if( g_library.api.auth.user.autoenrollment( username, new_password, &status ) != TT_OK ) {
		g_library.api.runtime.system.log( TT_LOG__WARN, "pam/passwd", "API:auth.user.autoenrollment() failed for user '%s'", username );
		return PAM_IGNORE; /* avoid impact on authentication process by not returning PAM_SERVICE_ERR */
	}
	if( status == TT_YES ) {
		pam_syslog( pamh, LOG_INFO, "pam_tokentube: user '%s' enrolled", username );
		g_library.api.runtime.system.log( TT_LOG__INFO, "pam/passwd", "user '%s' enrolled", username );
		return PAM_SUCCESS;
	}
	g_library.api.runtime.system.debug( TT_DEBUG__VERBOSITY3, "pam/passwd", "user '%s' not enrolled", username );
	return PAM_IGNORE;
}


PAM_EXTERN int pam_sm_authenticate(pam_handle_t *pamh, int flags __attribute__((unused)), int argc __attribute__((unused)), const char **argv __attribute__((unused))) {
	tt_status_t	status = TT_STATUS__UNDEFINED;
	const char*	username = NULL;
	const char*	password = NULL;

	pam_set_data( pamh, g_pam_tokentube_user_recreate, NULL, NULL );
	if( pam_tokentube_initialize( pamh ) != TT_OK ) {
		pam_syslog( pamh, LOG_ERR, "pam_tokentube: initialization failure" );
		return PAM_IGNORE; /* avoid impact on authentication process by not returning PAM_SERVICE_ERR */
	}
	if( pam_get_user( pamh, &username, NULL ) != PAM_SUCCESS || username == NULL || username[0] == '\0' ) {
		pam_syslog( pamh, LOG_ERR, "pam_tokentube: failed to get username from PAM master for authenticate" );
		g_library.api.runtime.system.debug( TT_DEBUG__VERBOSITY1, "pam/auth", "failed to get username from PAM master" );
		return PAM_SERVICE_ERR;
	}
	if( pam_get_item( pamh, PAM_AUTHTOK, (const void**)&password ) != PAM_SUCCESS ) {
		pam_syslog( pamh, LOG_ERR, "pam_tokentube: failed to get password from PAM master for authenticate" );
		g_library.api.runtime.system.debug( TT_DEBUG__VERBOSITY1, "pam/auth", "pam_get_item() failed for PAM_AUTHTOK in %s()", __FUNCTION__ );
		return PAM_SERVICE_ERR;
	}
	if( g_library.api.database.user.exists( username, &status ) != TT_OK ) {
		pam_syslog( pamh, LOG_ERR, "pam_tokentube: API:user.exists() returned error for user '%s' for authenticate", username );
		g_library.api.runtime.system.debug( TT_DEBUG__VERBOSITY1, "pam/auth", "API:user.exists() failed for user '%s' in %s()", username, __FUNCTION__ );
		return PAM_IGNORE; /* avoid impact on authentication process by not returning PAM_SERVICE_ERR */
	}
	switch( status ) {
		case TT_STATUS__YES:
			if( g_library.api.auth.user.verify( username, password, &status ) != TT_OK ) {
				pam_syslog( pamh, LOG_ERR, "pam_tokentube: API:auth.user.verify() returned error for user '%s' for authenticate", username );
				g_library.api.runtime.system.debug( TT_DEBUG__VERBOSITY1, "pam/auth", "API:auth.user.verify() failed for user '%s' in %s()", username, __FUNCTION__ );
				return PAM_IGNORE; /* avoid impact on authentication process by not returning PAM_SERVICE_ERR */
			}
			if( status != TT_YES ) {
				g_library.api.runtime.system.debug( TT_DEBUG__VERBOSITY2, "pam/auth", "API:auth.user.verify() failed for user '%s' in %s()", username, __FUNCTION__ );
				if( pam_set_data( pamh, g_pam_tokentube_user_recreate, (char*)username, NULL ) != PAM_SUCCESS ) {
					pam_syslog( pamh, LOG_ERR, "pam_set_data() failed for user '%s' in %s()", username, __FUNCTION__ );
				}
				return PAM_AUTHTOK_ERR;
			}
			g_library.api.runtime.system.debug( TT_DEBUG__VERBOSITY2, "pam/auth", "API:auth.user.verify() successful for user '%s' in %s()", username, __FUNCTION__ );
			return PAM_SUCCESS;
		case TT_STATUS__NO:
			g_library.api.runtime.system.debug( TT_DEBUG__VERBOSITY2, "pam/auth", "API:user.exists() returned 'NO' for user '%s' in %s()", username, __FUNCTION__ );
			return PAM_IGNORE;
		default:
			pam_syslog( pamh, LOG_ERR, "pam_tokentube: API:user.exists() returned invalid status for user '%s' in %s()", username, __FUNCTION__ );
			g_library.api.runtime.system.log( TT_LOG__ERROR, "pam/auth", "API:user.exists() returned invalid status for user '%s'", username );
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
		pam_syslog( pamh, LOG_ERR, "pam_tokentube: failed to get username from PAM master for session" );
		g_library.api.runtime.system.debug( TT_DEBUG__VERBOSITY1, "pam/session", "pam_get_user() returned error in %s()", __FUNCTION__ );
		return PAM_SERVICE_ERR;
	}
	if( pam_get_item( pamh, PAM_AUTHTOK, (const void**)&password ) != PAM_SUCCESS || password == NULL || password[0] == '\0' ) {
		pam_syslog( pamh, LOG_ERR, "pam_tokentube: failed to get password from PAM master for session" );
		g_library.api.runtime.system.debug( TT_DEBUG__VERBOSITY1, "pam/session", "pam_get_item() failed for PAM_AUTHTOK in %s()", __FUNCTION__ );
		return PAM_SERVICE_ERR;
	}
	if( g_library.api.database.user.exists( username, &status ) != TT_OK ) {
		pam_syslog( pamh, LOG_ERR, "pam_tokentube: API:user.exists() returned error for user '%s' for session", username );
		g_library.api.runtime.system.debug( TT_DEBUG__VERBOSITY1, "pam/session", "API:user.exists() failed for user '%s' in %s()", username, __FUNCTION__ );
		return PAM_IGNORE; /* avoid impact on authentication process by not returning PAM_SERVICE_ERR */
	}
	if( status == TT_STATUS__YES ) {
		g_library.api.runtime.system.debug( TT_DEBUG__VERBOSITY4, "pam/session", "API:user.exists() successful for user '%s' in %s()", username, __FUNCTION__ );
		if( pam_get_data( pamh, g_pam_tokentube_user_recreate, (const void**)&recreate ) == PAM_SUCCESS && recreate != NULL ) {
			if( username == recreate || strncmp( username, recreate, TT_USERNAME_CHAR_MAX+1 ) == 0 ) {
				g_library.api.runtime.system.debug( TT_DEBUG__VERBOSITY3, "pam/session", "recreating user '%s' in %s()", username, __FUNCTION__ );
				if( g_library.api.database.user.delete( username, &status ) != TT_OK || status != TT_STATUS__YES ) {
					g_library.api.runtime.system.log( TT_LOG__ERROR, "pam/session", "API:user.delete() failed for user '%s'", username );
					return PAM_IGNORE; /* avoid impact on authentication process by not returning PAM_SERVICE_ERR */
				}
				if( g_library.api.database.user.create( username, password ) != TT_OK ) {
					g_library.api.runtime.system.log( TT_LOG__ERROR, "pam/session", "API:user.create() failed for user '%s'", username );
					return PAM_IGNORE; /* avoid impact on authentication process by not returning PAM_SERVICE_ERR */
				}
				g_library.api.runtime.system.debug( TT_DEBUG__VERBOSITY2, "pam/session", "recreated user '%s' in %s()", username, __FUNCTION__ );
				return PAM_SUCCESS;
			}
		}
		return PAM_IGNORE;
	}
	if( g_library.api.auth.user.autoenrollment( username, password, &status ) != TT_OK ) {
		g_library.api.runtime.system.log( TT_LOG__ERROR, "pam/session", "API:auth.user.autoenrollment() failed for user '%s'", username );
		return PAM_IGNORE; /* avoid impact on authentication process by not returning PAM_SERVICE_ERR */
	}
	if( status == TT_STATUS__YES ) {
		g_library.api.runtime.system.log( TT_LOG__INFO, "pam/session", "user '%s' enrolled", username );
		return PAM_SUCCESS;
	}
	g_library.api.runtime.system.debug( TT_DEBUG__VERBOSITY2, "pam/session", "API:auth.user.autoenrollment() didn't enroll user '%s'", username );
	return PAM_IGNORE;
}


PAM_EXTERN int pam_sm_close_session(pam_handle_t *pamh, int flags __attribute__((unused)), int argc __attribute__((unused)), const char **argv __attribute__((unused))) {
	pam_set_data( pamh, g_pam_tokentube_user_recreate, NULL, NULL );
	return PAM_IGNORE;
}

