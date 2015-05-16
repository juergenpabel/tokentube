#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include <tokentube.h>


static void test_testvectors_hex(void **state) {
	tt_library_t*	library;
	char		output[1024] = {0};
	size_t		output_size = sizeof(output);

	library = (tt_library_t*)*state;
	assert_non_null( library );

	output_size = sizeof(output);
	assert_true( library->api.util.hex_encode( "", 0, output, &output_size ) == TT_OK );
	assert_int_equal( output_size, 0 );
	assert_string_equal( output, "" );

	output_size = sizeof(output);
	assert_true( library->api.util.hex_encode( "test", 4, output, &output_size ) == TT_OK );
	assert_int_equal( output_size, 8 );
	assert_string_equal( output, "74657374" );
}


static void test_testvectors_crc16(void **state) {
	tt_library_t*	library;
	unsigned short	result;

	library = (tt_library_t*)*state;
	assert_non_null( library );

	assert_true( library->api.util.crc16( "", 0, &result ) == TT_OK );
	assert_int_equal( result, 0x0000 );

	assert_true( library->api.util.crc16( "test", 4, &result ) == TT_OK );
	assert_int_equal( result, 0xf82e );

}


static void test_testvectors_base32(void **state) {
	tt_library_t*	library;
	char		output[1024] = {0};
	size_t		output_size = sizeof(output);

	library = (tt_library_t*)*state;
	assert_non_null( library );

	output_size = sizeof(output);
	assert_true( library->api.util.base32_encode( "", 0, output, &output_size ) == TT_OK );
	assert_int_equal( output_size, 0 );
	assert_string_equal( output, "" );

	output_size = sizeof(output);
	assert_true( library->api.util.base32_encode( "test", 4, output, &output_size ) == TT_OK );
	assert_int_equal( output_size, 8 );
	assert_string_equal( output, "ruvtrq4h" );

	output_size = sizeof(output);
	assert_true( library->api.util.base32_decode( "ruvtrq4h", 8, output, &output_size ) == TT_OK );
	assert_int_equal( output_size, 4 );
	output[4] = '\0';
	assert_string_equal( output, "test" );

	output_size = sizeof(output);
	assert_true( library->api.util.base32_decode( "zpcp2e37qua92y9u860e", 20, output, &output_size ) == TT_OK );
	assert_int_equal( output_size, 10 );
}


static void test_testvectors_base64(void **state) {
	tt_library_t*	library;
	char		output[1024] = {0};
	size_t		output_size = sizeof(output);

	library = (tt_library_t*)*state;
	assert_non_null( library );

	output_size = sizeof(output);
	assert_true( library->api.util.base64_encode( "", 0, output, &output_size ) == TT_OK );
	assert_int_equal( output_size, 0 );
	assert_string_equal( output, "" );

	output_size = sizeof(output);
	assert_true( library->api.util.base64_encode( "test", 4, output, &output_size ) == TT_OK );
	assert_int_equal( output_size, 8 );
	assert_string_equal( output, "dGVzdA==" );
}


static void self_setup(void **state) {
	tt_library_t*	library;

	(void) state; /* unused */

	library = malloc( sizeof(tt_library_t) );
	assert_non_null( library );

	assert_true( tt_initialize( TT_VERSION ) == TT_OK );
	assert_true( tt_discover( library ) == TT_OK );

	*state = library;
}


static void self_teardown(void **state) {
	tt_finalize();
	assert_non_null( *state );
	free( *state );
	*state = NULL;
}


int main(void) {
	const UnitTest tests[] = {
		unit_test_setup_teardown(test_testvectors_hex, self_setup, self_teardown),
		unit_test_setup_teardown(test_testvectors_crc16, self_setup, self_teardown),
		unit_test_setup_teardown(test_testvectors_base32, self_setup, self_teardown),
		unit_test_setup_teardown(test_testvectors_base64, self_setup, self_teardown),
	};
	return run_tests(tests);
}

