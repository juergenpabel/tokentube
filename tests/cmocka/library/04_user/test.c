#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <setjmp.h>
#include <cmocka.h>
#include <tokentube.h>


static void test_user_create_success(void **state) {
	tt_status_t	status;
	tt_library_t*	library;

	library = (tt_library_t*)*state;

	/* positive tests: create user */
	status = TT_STATUS__UNDEFINED;
	assert_true( library->api.user.create( "user", "pass" ) == TT_OK );
	assert_true( library->api.user.exists( "user", &status ) == TT_OK );
	assert_true( status == TT_YES );
	status = TT_STATUS__UNDEFINED;
	assert_true( library->api.user.execute_verify( "user", "pass", &status ) == TT_OK );
	assert_true( status == TT_YES );
}


static void test_user_create_failure(void **state) {
	tt_status_t	status;
	tt_library_t*	library;

	library = (tt_library_t*)*state;

	/* negative tests: invalid parameters */
	status = TT_STATUS__UNDEFINED;
	assert_true( library->api.user.create( NULL, "pass" ) == TT_ERR );
	assert_true( library->api.user.exists( NULL, &status ) == TT_ERR );
	assert_true( status == TT_STATUS__UNDEFINED );

	status = TT_STATUS__UNDEFINED;
	assert_true( library->api.user.create( "user", NULL ) == TT_ERR );
	assert_true( library->api.user.exists( "user", &status ) == TT_OK );
	assert_true( status == TT_NO );

	status = TT_STATUS__UNDEFINED;
	assert_true( library->api.user.create( "", "pass" ) == TT_ERR );
	assert_true( library->api.user.exists( "", &status ) == TT_ERR );
	assert_true( status == TT_STATUS__UNDEFINED );

	status = TT_STATUS__UNDEFINED;
	assert_true( library->api.user.create( "user", "" ) == TT_ERR );
	assert_true( library->api.user.exists( "user", &status ) == TT_OK );
	assert_true( status == TT_NO );
}


static void test_user_update_success(void **state) {
	tt_status_t	status;
	tt_library_t*	library;

	library = (tt_library_t*)*state;

	/* positive tests: update user */
	status = TT_STATUS__UNDEFINED;
	assert_true( library->api.user.create( "user", "pass" ) == TT_OK );
	assert_true( library->api.user.update( "user", "pass", "pass2", &status ) == TT_OK );
	status = TT_STATUS__UNDEFINED;
	assert_true( library->api.user.execute_verify( "user", "pass2", &status ) == TT_OK );
	assert_true( status == TT_YES );
}


static void test_user_update_failure(void **state) {
	tt_status_t	status;
	tt_library_t*	library;

	library = (tt_library_t*)*state;

	/* negative tests: invalid parameters */
	status = TT_STATUS__UNDEFINED;
	assert_true( library->api.user.create( "user", "pass" ) == TT_OK );
	assert_true( library->api.user.update( "user", NULL, NULL, &status ) == TT_ERR );
	assert_true( library->api.user.update( "user", "pass", NULL, &status ) == TT_ERR );
	assert_true( library->api.user.update( "user", NULL, "pass2", &status ) == TT_ERR );
	assert_true( library->api.user.update( "user", "", "", &status ) == TT_ERR );
	assert_true( library->api.user.update( "user", "pass", "", &status ) == TT_ERR );
	assert_true( library->api.user.update( "user", "", "pass2", &status ) == TT_ERR );
}


static void test_user_delete_success(void **state) {
	tt_status_t	status = TT_STATUS__UNDEFINED;
	tt_library_t*	library;

	library = (tt_library_t*)*state;

	/* positive tests: delete user */
	assert_true( library->api.user.create( "user", "pass" ) == TT_OK );
	assert_true( library->api.user.exists( "user", &status ) == TT_OK );
	assert_true( status == TT_YES );
	assert_true( library->api.user.delete( "user", &status ) == TT_OK );
	assert_true( status == TT_YES );
	assert_true( library->api.user.exists( "user", &status ) == TT_OK );
	assert_true( status == TT_NO );
}


static void test_user_delete_failure(void **state) {
	tt_status_t	status = TT_STATUS__UNDEFINED;
	tt_library_t*	library;

	library = (tt_library_t*)*state;

	/* negative tests: invalid paramters */
	assert_true( library->api.user.delete( NULL, NULL ) == TT_ERR );
	assert_true( library->api.user.delete( "", NULL ) == TT_ERR );
	assert_true( library->api.user.delete( NULL, &status ) == TT_ERR );
}


static void test_user_verify_success(void **state) {
	tt_status_t	status = TT_STATUS__UNDEFINED;
	tt_library_t*	library;

	library = (tt_library_t*)*state;

	/* positive tests */
	assert_true( library->api.user.create( "user", "pass" ) == TT_OK );
	assert_true( library->api.user.execute_verify( "user", "pass", &status ) == TT_OK );
	assert_true( status == TT_YES );
}


static void test_user_verify_failure(void **state) {
	tt_status_t	status = TT_STATUS__UNDEFINED;
	tt_library_t*	library;

	library = (tt_library_t*)*state;

	/* negative tests */
	assert_true( library->api.user.create( "user", "pass" ) == TT_OK );
	assert_true( library->api.user.execute_verify( "user", "fail", &status ) == TT_OK );
	assert_true( status == TT_NO );
	assert_true( library->api.user.delete( "user", &status ) == TT_OK );
	assert_true( status == TT_YES );
}


static void test_user_reconf_success(void **state) {
	tt_status_t	status = TT_STATUS__UNDEFINED;
	tt_library_t*	library;

	library = (tt_library_t*)*state;

	/* positive tests */
	assert_true( library->api.user.create( "user", "pass" ) == TT_OK );
	assert_true( library->api.user.execute_verify( "user", "pass", &status ) == TT_OK );
	assert_true( status == TT_YES );

	assert_true( tt_finalize() == TT_OK );
	assert_true( tt_initialize( TT_VERSION ) == TT_OK );
	assert_true( tt_configure( "etc/tokentube/tokentube-reconf.conf" ) == TT_OK );
	assert_true( tt_discover( library ) == TT_OK );

	assert_true( library->api.user.execute_verify( "user", "pass", &status ) == TT_OK );
	assert_true( status == TT_YES );
	assert_true( library->api.user.update( "user", "pass", "pass2", &status ) == TT_OK );
	assert_true( status == TT_YES );
	assert_true( library->api.user.execute_verify( "user", "pass2", &status ) == TT_OK );
}


static void self_setup(void **state) {
	tt_status_t	status = TT_STATUS__UNDEFINED;
	tt_library_t*	library = NULL;

	library = malloc( sizeof(tt_library_t) );
	assert_true( library != NULL );
	memset( library, '\0', sizeof(tt_library_t) );
	assert_true( tt_initialize( TT_VERSION ) == TT_OK );
	assert_true( tt_configure( "etc/tokentube/tokentube.conf" ) == TT_OK );
	assert_true( tt_discover( library ) == TT_OK );
	assert_true( library->api.user.delete( "user", &status ) == TT_OK );
	*state = library;
}


static void self_teardown(void **state) {
	tt_status_t	status = TT_STATUS__UNDEFINED;
	tt_library_t*	library;

	library = (tt_library_t*)*state;

	assert_true( library->api.user.delete( "user", &status ) == TT_OK );
	tt_finalize();
}


int main(void) {
	const UnitTest tests[] = {
		unit_test_setup_teardown(test_user_create_success, self_setup, self_teardown),
		unit_test_setup_teardown(test_user_create_failure, self_setup, self_teardown),
		unit_test_setup_teardown(test_user_update_success, self_setup, self_teardown),
		unit_test_setup_teardown(test_user_update_failure, self_setup, self_teardown),
		unit_test_setup_teardown(test_user_delete_success, self_setup, self_teardown),
		unit_test_setup_teardown(test_user_delete_failure, self_setup, self_teardown),
		unit_test_setup_teardown(test_user_verify_success, self_setup, self_teardown),
		unit_test_setup_teardown(test_user_verify_failure, self_setup, self_teardown),
		unit_test_setup_teardown(test_user_reconf_success, self_setup, self_teardown),
	};
	return run_tests(tests);
}

