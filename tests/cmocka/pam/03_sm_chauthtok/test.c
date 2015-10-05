#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <setjmp.h>
#include <cmocka.h>
#include <tokentube.h>

#define PAM_SM_SESSION

#include <security/pam_modules.h>
#include <security/pam_ext.h>


static void test_pam_sm_chauthtok(void **state) {
	(void)state; /* TODO */

}


static void self_setup(void **state) {
	(void)state; /* unused */
	tt_library_t	library;

	assert_true( tt_initialize( TT_VERSION ) == TT_OK );
	assert_true( tt_configure( "etc/tokentube/tokentube.conf" ) == TT_OK );
	assert_true( tt_discover( &library ) == TT_OK );
	assert_true( library.api.database.user.create( "user", "pass" ) == TT_OK );
	tt_finalize();
}


static void self_teardown(void **state) {
	(void)state; /* unused */
	tt_library_t	library;
	tt_status_t	status;

	pam_sm_close_session( NULL, 0, 0, NULL );
	assert_true( tt_initialize( TT_VERSION ) == TT_OK );
	assert_true( tt_configure( "etc/tokentube/tokentube.conf" ) == TT_OK );
	assert_true( tt_discover( &library ) == TT_OK );
	assert_true( library.api.database.user.delete( "user", &status ) == TT_OK );
	tt_finalize();
}


int main(void) {
	const UnitTest tests[] = {
		unit_test_setup_teardown(test_pam_sm_chauthtok, self_setup, self_teardown),
	};
	return run_tests(tests);
}

