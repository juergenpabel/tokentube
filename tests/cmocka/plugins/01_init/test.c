#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <setjmp.h>
#include <cmocka.h>
#include <tokentube.h>


static void test_init(void **state) {
	tt_library_t	library;
	void*		null = NULL;
	void*		test = NULL;
	size_t		i;

	(void)state;

	/* plugin init test */
	assert_true( tt_initialize( TT_VERSION ) == TT_OK );
	assert_true( tt_configure( "etc/tokentube/tokentube.conf" ) == TT_OK );

	/* library API interface NULL-test */
	memset( &library, '\0', sizeof(library) );
	assert_true( tt_discover( &library ) == TT_OK );
	test = &library.api;
	for( i=0; i<sizeof(library.api)/sizeof(void*); i++ ) {
		assert_true( memcmp( test, &null, sizeof(void*) ) != 0 );
		test += sizeof(void*);
	}
	assert_true( tt_finalize() == TT_OK );
}


int main(void) {
	const UnitTest tests[] = {
		unit_test_setup_teardown(test_init, NULL, NULL)
	};
	return run_tests(tests);
}

