#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include <tokentube.h>

int libtokentube_reset();


static void test_init_too_old_version(void **state) {
	tt_version_t	version;

	(void)	state; /* unused */

	version.major = 0;
	version.minor = 0;
	version.patch = 0;
	assert_true( tt_initialize( version ) == TT_ERR );	
	assert_true( tt_finalize() == TT_IGN );	
	libtokentube_reset();
}


static void test_init_current_version(void **state) {
	tt_version_t	version;

	(void)	state; /* unused */

	version.major = TT_VERSION_MAJOR;
	version.minor = 0;
	version.patch = 0;
	assert_true( tt_initialize( version ) == TT_OK );	
	assert_true( tt_finalize() == TT_OK );	

	version.major = TT_VERSION_MAJOR;
	version.minor = TT_VERSION_MINOR;
	version.patch = TT_VERSION_PATCH;
	assert_true( tt_initialize( version ) == TT_OK );	
	assert_true( tt_finalize() == TT_OK );	

	version.major = TT_VERSION_MAJOR;
	version.minor = 255;
	version.patch = 255;
	assert_true( tt_initialize( version ) == TT_OK );	
	assert_true( tt_finalize() == TT_OK );	
}


static void test_init_too_new_version(void **state) {
	tt_version_t	version;

	(void)	state; /* unused */

	version.major = 255;
	version.minor = 255;
	version.patch = 255;
	assert_true( tt_initialize( version ) == TT_ERR );	
	assert_true( tt_finalize() == TT_IGN );	
	libtokentube_reset();
}


int main(void) {
	const UnitTest tests[] = {
		unit_test(test_init_too_old_version),
		unit_test(test_init_current_version),
		unit_test(test_init_too_new_version)
	};
	return run_tests(tests);
}

