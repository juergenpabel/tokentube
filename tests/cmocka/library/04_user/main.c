#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <setjmp.h>
#include <cmocka.h>
#include <tokentube.h>


static void test_user_create(void **state) {
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

	/* positive tests: create user */
	status = TT_STATUS__UNDEFINED;
	assert_true( library->api.user.create( "user", "pass" ) == TT_OK );
	assert_true( library->api.user.exists( "user", &status ) == TT_OK );
	assert_true( status == TT_YES );
}


static void test_user_delete(void **state) {
	tt_status_t	status = TT_STATUS__UNDEFINED;
	tt_library_t*	library;

	library = (tt_library_t*)*state;

	/* negative tests: invalid paramters */
	assert_true( library->api.user.delete( NULL, NULL ) == TT_ERR );
	assert_true( library->api.user.delete( "", NULL ) == TT_ERR );
	assert_true( library->api.user.delete( NULL, &status ) == TT_ERR );

	/* positive tests: delete user */
	assert_true( library->api.user.create( "user", "pass" ) == TT_OK );
	assert_true( library->api.user.exists( "user", &status ) == TT_OK );
	assert_true( status == TT_YES );
	assert_true( library->api.user.delete( "user", &status ) == TT_OK );
	assert_true( status == TT_YES );
	assert_true( library->api.user.exists( "user", &status ) == TT_OK );
	assert_true( status == TT_NO );
}


static void test_user_verify(void **state) {
	tt_status_t	status = TT_STATUS__UNDEFINED;
	tt_library_t*	library;

	library = (tt_library_t*)*state;

	/* negative tests */
	assert_true( library->api.user.create( "user", "pass" ) == TT_OK );
	assert_true( library->api.user.execute_verify( "user", "fail", &status ) == TT_OK );
	assert_true( status == TT_NO );
	assert_true( library->api.user.delete( "user", &status ) == TT_OK );
	assert_true( status == TT_YES );

	/* positive tests */
	assert_true( library->api.user.create( "user", "pass" ) == TT_OK );
	assert_true( library->api.user.execute_verify( "user", "pass", &status ) == TT_OK );
	assert_true( status == TT_YES );
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
		unit_test_setup_teardown(test_user_create, self_setup, self_teardown),
		unit_test_setup_teardown(test_user_delete, self_setup, self_teardown),
		unit_test_setup_teardown(test_user_verify, self_setup, self_teardown),
	};
	return run_tests(tests);
}

