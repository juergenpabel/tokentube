#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <setjmp.h>
#include <cmocka.h>
#include <tokentube.h>


static void test_otp_create_failure(void **state) {
	tt_status_t	status;
	tt_library_t*   library;

	library = (tt_library_t*)*state;
	assert_true( library != NULL );
	status = TT_STATUS__UNDEFINED;
	assert_true( library->api.database.otp.create( NULL, &status ) == TT_ERR );
	assert_true( library->api.database.otp.exists( NULL, &status ) == TT_ERR );
	assert_true( status == TT_STATUS__UNDEFINED );
}


static void test_otp_create_success(void **state) {
	tt_status_t	status;
	tt_library_t*   library;

	library = (tt_library_t*)*state;
	assert_true( library != NULL );
	status = TT_STATUS__UNDEFINED;
	assert_true( library->api.database.otp.create( "test", &status ) == TT_OK );
	assert_true( library->api.database.otp.exists( "test", &status ) == TT_OK );
	assert_true( status == TT_YES );
}


static void test_otp_delete_failure(void **state) {
	tt_status_t	status = TT_STATUS__UNDEFINED;
	tt_library_t*   library;

	library = (tt_library_t*)*state;
	assert_true( library->api.database.otp.delete( NULL, NULL ) == TT_ERR );
	assert_true( library->api.database.otp.delete( "", NULL ) == TT_ERR );
	assert_true( library->api.database.otp.delete( NULL, &status ) == TT_ERR );
}


static void test_otp_delete_success(void **state) {
	tt_status_t	status = TT_STATUS__UNDEFINED;
	tt_library_t*   library;

	library = (tt_library_t*)*state;
	assert_true( library->api.database.otp.create( "test", &status ) == TT_OK );
	assert_true( library->api.database.otp.exists( "test", &status ) == TT_OK );
	assert_true( status == TT_YES );
	assert_true( library->api.database.otp.delete( "test", &status ) == TT_OK );
	assert_true( status == TT_YES );
	assert_true( library->api.database.otp.exists( "test", &status ) == TT_OK );
	assert_true( status == TT_NO );
}


static void test_helpdesk(void **state) {
	tt_status_t	status = TT_STATUS__UNDEFINED;
	char		challenge[TT_OTP_TEXT_MAX+1] = {0};
	size_t		challenge_size = sizeof(challenge);
	char		response[TT_OTP_TEXT_MAX+1] = {0};
	size_t		response_size = sizeof(response);
	char		key[TT_OTP_BITS_MAX/8] = {0};
	size_t		key_size = sizeof(key);
	char		null[TT_OTP_BITS_MAX/8] = {0};
	tt_library_t*   library;

	library = (tt_library_t*)*state;

	assert_true( library->api.database.otp.create( "test", &status ) == TT_OK );
	assert_true( library->api.auth.otp.challenge( "test", challenge, &challenge_size ) == TT_OK );
	assert_int_equal( challenge_size, 20 );

	assert_true( library->api.auth.otp.response( "test", "hxstyrzhpq65p89y9uwv", response, &response_size ) == TT_OK );
	assert_string_equal( response, "fynraw6vcrgg229h86rx" );

	assert_true( library->api.auth.otp.loadkey( "test", "hxstyrzhpq65p89y9uwv", "fynraw6vcrgg229h86rx", key, &key_size ) == TT_OK );
	assert_int_equal( key_size, 32 );
	assert_true( memcmp( key, null, 32 ) == 0 );

	assert_true( library->api.database.otp.delete( "test", &status ) == TT_OK );
	assert_true( status == TT_YES );
}


static void self_setup(void **state) {
	tt_library_t*   library = NULL;

	library = malloc( sizeof(tt_library_t) );
	assert_true( library != NULL );
	memset( library, '\0', sizeof(tt_library_t) );
	assert_true( tt_initialize( TT_VERSION ) == TT_OK );
	assert_true( tt_configure( "etc/tokentube/tokentube.conf" ) == TT_OK );
	assert_true( tt_discover( library ) == TT_OK );
	*state = library;
}


static void self_teardown(void **state) {
	tt_status_t	status = TT_STATUS__UNDEFINED;
	tt_library_t*   library;

	library = (tt_library_t*)*state;
	if( library != NULL ) {
		assert_true( library->api.database.otp.delete( "test", &status ) == TT_OK );
		*state = NULL;
	}
	tt_finalize();
}


int main(void) {
	const UnitTest tests[] = {
		unit_test_setup_teardown(test_otp_create_success, self_setup, self_teardown),
		unit_test_setup_teardown(test_otp_create_failure, self_setup, self_teardown),
		unit_test_setup_teardown(test_otp_delete_success, self_setup, self_teardown),
		unit_test_setup_teardown(test_otp_delete_failure, self_setup, self_teardown),
		unit_test_setup_teardown(test_helpdesk, self_setup, self_teardown),
	};
	return run_tests(tests);
}

