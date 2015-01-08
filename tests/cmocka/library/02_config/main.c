#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include <tokentube.h>


static void test_conf_00_file_not_found(void **state) {
	(void)	state; /* unused */
	assert_true( tt_initialize( TT_VERSION ) == TT_OK );
	assert_true( tt_configure( "00/tokentube.conf" ) == TT_ERR );
	tt_finalize();
}


static void test_conf_01_syntax_error(void **state) {
	(void)	state; /* unused */
	assert_true( tt_initialize( TT_VERSION ) == TT_OK );
	assert_true( tt_configure( "01/tokentube.conf" ) == TT_ERR );
	tt_finalize();
}


static void test_conf_02_include_fnf(void **state) {
	(void)	state; /* unused */
	assert_true( tt_initialize( TT_VERSION ) == TT_OK );
	assert_true( tt_configure( "02/tokentube.conf" ) == TT_ERR );
	tt_finalize();
}


static void test_conf_03_include_hash(void **state) {
	(void)	state; /* unused */
	assert_true( tt_initialize( TT_VERSION ) == TT_OK );
	assert_true( tt_configure( "03/tokentube.conf" ) == TT_ERR );
	tt_finalize();
}


static void test_conf_04_ok(void **state) {
	(void)	state; /* unused */
	assert_true( tt_initialize( TT_VERSION ) == TT_OK );
	assert_true( tt_configure( "04/tokentube.conf" ) == TT_OK );
	tt_finalize();
}


int main(void) {
	const UnitTest tests[] = {
		unit_test(test_conf_00_file_not_found),
		unit_test(test_conf_01_syntax_error),
		unit_test(test_conf_02_include_fnf),
		unit_test(test_conf_03_include_hash),
		unit_test(test_conf_04_ok)
	};
	return run_tests(tests);
}

