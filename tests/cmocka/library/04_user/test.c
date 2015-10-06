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
	assert_true( library->api.database.user.create( "user", "pass", &status ) == TT_OK );
	assert_true( library->api.database.user.exists( "user", &status ) == TT_OK );
	assert_true( status == TT_YES );
	status = TT_STATUS__UNDEFINED;
	assert_true( library->api.auth.user.verify( "user", "pass", &status ) == TT_OK );
	assert_true( status == TT_YES );
}


static void test_user_create_failure(void **state) {
	tt_status_t	status;
	tt_library_t*	library;

	library = (tt_library_t*)*state;

	/* negative tests: invalid parameters */
	status = TT_STATUS__UNDEFINED;
	assert_true( library->api.database.user.create( NULL, "pass", &status ) == TT_ERR );
	assert_true( library->api.database.user.exists( NULL, &status ) == TT_ERR );
	assert_true( status == TT_STATUS__UNDEFINED );

	status = TT_STATUS__UNDEFINED;
	assert_true( library->api.database.user.create( "user", NULL, &status ) == TT_ERR );
	assert_true( library->api.database.user.exists( "user", &status ) == TT_OK );
	assert_true( status == TT_NO );

	status = TT_STATUS__UNDEFINED;
	assert_true( library->api.database.user.create( "", "pass", &status ) == TT_ERR );
	assert_true( library->api.database.user.exists( "", &status ) == TT_ERR );
	assert_true( status == TT_STATUS__UNDEFINED );

	status = TT_STATUS__UNDEFINED;
	assert_true( library->api.database.user.create( "user", "", &status ) == TT_ERR );
	assert_true( library->api.database.user.exists( "user", &status ) == TT_OK );
	assert_true( status == TT_NO );
}


static void test_user_modify_success(void **state) {
	tt_status_t	status;
	tt_library_t*	library;

	library = (tt_library_t*)*state;

	/* positive tests: modify user */
	status = TT_STATUS__UNDEFINED;
	assert_true( library->api.database.user.create( "user", "pass", &status ) == TT_OK );
	assert_true( library->api.database.user.modify( "user", "pass", TT_MODIFY__USER_PASSWORD, "pass2", &status ) == TT_OK );
	status = TT_STATUS__UNDEFINED;
	assert_true( library->api.auth.user.verify( "user", "pass2", &status ) == TT_OK );
	assert_true( status == TT_YES );
}


static void test_user_modify_failure(void **state) {
	tt_status_t	status;
	tt_library_t*	library;

	library = (tt_library_t*)*state;

	/* negative tests: invalid parameters */
	status = TT_STATUS__UNDEFINED;
	assert_true( library->api.database.user.create( "user", "pass", &status ) == TT_OK );
	assert_true( library->api.database.user.modify( "user", NULL, TT_MODIFY__USER_PASSWORD, NULL, &status ) == TT_ERR );
	assert_true( library->api.database.user.modify( "user", "pass", TT_MODIFY__USER_PASSWORD, NULL, &status ) == TT_ERR );
	assert_true( library->api.database.user.modify( "user", NULL, TT_MODIFY__USER_PASSWORD, "pass2", &status ) == TT_ERR );
	assert_true( library->api.database.user.modify( "user", "", TT_MODIFY__USER_PASSWORD, "", &status ) == TT_ERR );
	assert_true( library->api.database.user.modify( "user", "pass", TT_MODIFY__USER_PASSWORD, "", &status ) == TT_ERR );
	assert_true( library->api.database.user.modify( "user", "", TT_MODIFY__USER_PASSWORD, "pass2", &status ) == TT_ERR );
}


static void test_user_delete_success(void **state) {
	tt_status_t	status = TT_STATUS__UNDEFINED;
	tt_library_t*	library;

	library = (tt_library_t*)*state;

	/* positive tests: delete user */
	assert_true( library->api.database.user.create( "user", "pass", &status ) == TT_OK );
	assert_true( library->api.database.user.exists( "user", &status ) == TT_OK );
	assert_true( status == TT_YES );
	assert_true( library->api.database.user.delete( "user", &status ) == TT_OK );
	assert_true( status == TT_YES );
	assert_true( library->api.database.user.exists( "user", &status ) == TT_OK );
	assert_true( status == TT_NO );
}


static void test_user_delete_failure(void **state) {
	tt_status_t	status = TT_STATUS__UNDEFINED;
	tt_library_t*	library;

	library = (tt_library_t*)*state;

	/* negative tests: invalid paramters */
	assert_true( library->api.database.user.delete( NULL, NULL ) == TT_ERR );
	assert_true( library->api.database.user.delete( "", NULL ) == TT_ERR );
	assert_true( library->api.database.user.delete( NULL, &status ) == TT_ERR );
}


static void test_user_verify_success(void **state) {
	tt_status_t	status = TT_STATUS__UNDEFINED;
	tt_library_t*	library;

	library = (tt_library_t*)*state;

	/* positive tests */
	assert_true( library->api.database.user.create( "user", "pass", &status ) == TT_OK );
	assert_true( library->api.auth.user.verify( "user", "pass", &status ) == TT_OK );
	assert_true( status == TT_YES );
}


static void test_user_verify_failure(void **state) {
	tt_status_t	status = TT_STATUS__UNDEFINED;
	tt_library_t*	library;

	library = (tt_library_t*)*state;

	/* negative tests */
	assert_true( library->api.database.user.create( "user", "pass", &status ) == TT_OK );
	assert_true( library->api.auth.user.verify( "user", "fail", &status ) == TT_OK );
	assert_true( status == TT_NO );
	assert_true( library->api.database.user.delete( "user", &status ) == TT_OK );
	assert_true( status == TT_YES );
}


static void test_user_keys_success(void **state) {
	tt_status_t	status = TT_STATUS__UNDEFINED;
	tt_library_t*	library;

	library = (tt_library_t*)*state;

	/* positive tests */
	assert_true( library->api.database.user.create( "user", "pass", &status ) == TT_OK );
	assert_true( library->api.database.user.modify( "user", "pass", TT_MODIFY__USER_KEY_ADD, "test", &status ) == TT_OK );
	assert_true( status == TT_YES );
	assert_true( library->api.database.user.modify( "user", "pass", TT_MODIFY__USER_KEY_ADD, "test", &status ) == TT_OK );
	assert_true( status == TT_NO );
	assert_true( library->api.database.user.modify( "user", "pass", TT_MODIFY__USER_KEY_DEL, "test", &status ) == TT_OK );
	assert_true( status == TT_YES );
	assert_true( library->api.database.user.modify( "user", "pass", TT_MODIFY__USER_KEY_DEL, "test", &status ) == TT_OK );
	assert_true( status == TT_NO );

	assert_true( library->api.database.user.delete( "user", &status ) == TT_OK );
	assert_true( status == TT_YES );
}


static void test_user_keys_failure(void **state) {
	tt_status_t	status = TT_STATUS__UNDEFINED;
	tt_library_t*	library;

	library = (tt_library_t*)*state;

	/* negative tests */
	assert_true( library->api.database.user.create( "user", "pass", &status ) == TT_OK );
	assert_true( library->api.database.user.modify( "user", "pass", TT_MODIFY__USER_KEY_DEL, "test", &status ) == TT_OK );
	assert_true( status == TT_NO );
	assert_true( library->api.database.user.modify( "user", "fail", TT_MODIFY__USER_KEY_ADD, "test", &status ) == TT_ERR );
	assert_true( library->api.database.user.modify( "user", "pass", TT_MODIFY__USER_KEY_ADD, "fail", &status ) == TT_ERR );
	assert_true( library->api.database.user.modify( "fail", "pass", TT_MODIFY__USER_KEY_ADD, "test", &status ) == TT_ERR );

	assert_true( library->api.database.user.modify( "fail", "pass", TT_MODIFY__USER_KEY_DEL, "test", &status ) == TT_ERR );
	assert_true( library->api.database.user.modify( "user", "fail", TT_MODIFY__USER_KEY_DEL, "test", &status ) == TT_ERR );
	assert_true( library->api.database.user.modify( "user", "pass", TT_MODIFY__USER_KEY_DEL, "fail", &status ) == TT_OK );
	assert_true( status == TT_NO );

	assert_true( library->api.database.user.delete( "user", &status ) == TT_OK );
	assert_true( status == TT_YES );
}


static void test_user_reconf_success(void **state) {
	tt_status_t	status = TT_STATUS__UNDEFINED;
	tt_library_t*	library;

	library = (tt_library_t*)*state;

	/* positive tests */
	assert_true( library->api.database.user.create( "user", "pass", &status ) == TT_OK );
	assert_true( library->api.auth.user.verify( "user", "pass", &status ) == TT_OK );
	assert_true( status == TT_YES );

	assert_true( tt_finalize() == TT_OK );
	assert_true( tt_initialize( TT_VERSION ) == TT_OK );
	assert_true( tt_configure( "etc/tokentube/tokentube-reconf.conf" ) == TT_OK );
	assert_true( tt_discover( library ) == TT_OK );

	assert_true( library->api.auth.user.verify( "user", "pass", &status ) == TT_OK );
	assert_true( status == TT_YES );
	assert_true( library->api.database.user.modify( "user", "pass", TT_MODIFY__USER_PASSWORD, "pass2", &status ) == TT_OK );
	assert_true( status == TT_YES );
	assert_true( library->api.auth.user.verify( "user", "pass2", &status ) == TT_OK );
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
	assert_true( library->api.database.user.delete( "user", &status ) == TT_OK );
	*state = library;
}


static void self_teardown(void **state) {
	tt_status_t	status = TT_STATUS__UNDEFINED;
	tt_library_t*	library;

	library = (tt_library_t*)*state;

	assert_true( library->api.database.user.delete( "user", &status ) == TT_OK );
	tt_finalize();
}


int main(void) {
	const UnitTest tests[] = {
		unit_test_setup_teardown(test_user_create_success, self_setup, self_teardown),
		unit_test_setup_teardown(test_user_create_failure, self_setup, self_teardown),
		unit_test_setup_teardown(test_user_modify_success, self_setup, self_teardown),
		unit_test_setup_teardown(test_user_modify_failure, self_setup, self_teardown),
		unit_test_setup_teardown(test_user_delete_success, self_setup, self_teardown),
		unit_test_setup_teardown(test_user_delete_failure, self_setup, self_teardown),
		unit_test_setup_teardown(test_user_verify_success, self_setup, self_teardown),
		unit_test_setup_teardown(test_user_verify_failure, self_setup, self_teardown),
		unit_test_setup_teardown(test_user_keys_success, self_setup, self_teardown),
		unit_test_setup_teardown(test_user_keys_failure, self_setup, self_teardown),
		unit_test_setup_teardown(test_user_reconf_success, self_setup, self_teardown),
	};
	return run_tests(tests);
}

