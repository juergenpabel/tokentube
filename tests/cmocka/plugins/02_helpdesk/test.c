#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <setjmp.h>
#include <cmocka.h>
#include <tokentube.h>


static void test_otp_create(void **state) {
	tt_status_t	status;
	tt_library_t*   library;

	library = (tt_library_t*)*state;
	assert_true( library != NULL );

	/* negative tests: invalid parameters */
	status = TT_STATUS__UNDEFINED;

	assert_true( library->api.otp.create( NULL ) == TT_ERR );
	assert_true( library->api.otp.exists( NULL, &status ) == TT_ERR );
	assert_true( status == TT_STATUS__UNDEFINED );

	/* positive tests: create otp */
	status = TT_STATUS__UNDEFINED;
	assert_true( library->api.otp.create( "otp" ) == TT_OK );
	assert_true( library->api.otp.exists( "otp", &status ) == TT_OK );
	assert_true( status == TT_YES );
}


static void test_otp_delete(void **state) {
	tt_status_t	status = TT_STATUS__UNDEFINED;
	tt_library_t*   library;

	library = (tt_library_t*)*state;

	/* negative tests: invalid paramters */
	assert_true( library->api.otp.delete( NULL, NULL ) == TT_ERR );
	assert_true( library->api.otp.delete( "", NULL ) == TT_ERR );
	assert_true( library->api.otp.delete( NULL, &status ) == TT_ERR );

	/* positive tests: delete otp */
	assert_true( library->api.otp.create( "otp" ) == TT_OK );
	assert_true( library->api.otp.exists( "otp", &status ) == TT_OK );
	assert_true( status == TT_YES );
	assert_true( library->api.otp.delete( "otp", &status ) == TT_OK );
	assert_true( status == TT_YES );
	assert_true( library->api.otp.exists( "otp", &status ) == TT_OK );
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

	assert_true( library->api.otp.create( "otp" ) == TT_OK );
	assert_true( library->api.helpdesk.challenge( "otp", challenge, &challenge_size ) == TT_OK );
	assert_int_equal( challenge_size, 24 );

	assert_true( library->api.helpdesk.response( "otp", "63va-e94r-cyce-ew4q-c04u", response, &response_size ) == TT_OK );
	assert_string_equal( response, "k5r7-ukpe-sq04-zd67-xvv8" );

	assert_true( library->api.helpdesk.apply( "otp", "63va-e94r-cyce-ew4q-c04u", "k5r7-ukpe-sq04-zd67-xvv8", key, &key_size ) == TT_OK );
	assert_int_equal( key_size, 32 );
	assert_true( memcmp( key, null, 32 ) == 0 );

	assert_true( library->api.otp.delete( "otp", &status ) == TT_OK );
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
		assert_true( library->api.otp.delete( "otp", &status ) == TT_OK );
		*state = NULL;
	}
	tt_finalize();
}


int main(void) {
	const UnitTest tests[] = {
		unit_test_setup_teardown(test_otp_create, self_setup, self_teardown),
		unit_test_setup_teardown(test_otp_delete, self_setup, self_teardown),
		unit_test_setup_teardown(test_helpdesk, self_setup, self_teardown),
	};
	return run_tests(tests);
}

