#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include <tokentube.h>
#include "runtime.h"


static void test_compile_defines(void **state) {
	(void)	state; /* unused */

	assert_true( TT_UNINITIALIZED < 0 );	
	assert_true( TT_UNIMPLEMENTED < 0 );	
	assert_true( TT_UNINITIALIZED != TT_UNIMPLEMENTED );	

	assert_true( TT_OK == 0 );	
	assert_true( TT_OK != TT_ERR );	
	assert_true( TT_OK != TT_IGN );	
	assert_true( TT_OK != TT_ERR );	
}



static void test_compile_enums(void **state) {
	(void)	state; /* unused */

	assert_true( TT_STATUS__UNDEFINED != TT_YES );	
	assert_true( TT_STATUS__UNDEFINED != TT_NO );	
	assert_true( TT_YES != TT_NO );	

	assert_true( TT_LOG__UNDEFINED != TT_LOG__FATAL );	
	assert_true( TT_LOG__FATAL != TT_LOG__ERROR );	
	assert_true( TT_LOG__ERROR != TT_LOG__WARN );	
	assert_true( TT_LOG__WARN != TT_LOG__INFO );	

	assert_true( TT_DEBUG__UNDEFINED != TT_DEBUG__VERBOSITY1 );
	assert_true( TT_DEBUG__VERBOSITY1 != TT_DEBUG__VERBOSITY2 );
	assert_true( TT_DEBUG__VERBOSITY2 != TT_DEBUG__VERBOSITY3 );
	assert_true( TT_DEBUG__VERBOSITY3 != TT_DEBUG__VERBOSITY4 );
	assert_true( TT_DEBUG__VERBOSITY4 != TT_DEBUG__VERBOSITY5 );

	assert_true( TT_RUNTIME__UNDEFINED != TT_RUNTIME__PBA );
	assert_true( TT_RUNTIME__PBA != TT_RUNTIME__STANDARD );

	assert_true( TT_FILE__UNDEFINED != TT_FILE__CONFIG_PBA );
	assert_true( TT_FILE__CONFIG_PBA != TT_FILE__CONFIG_STANDARD );
	assert_true( TT_FILE__CONFIG_STANDARD != TT_FILE__USER );
	assert_true( TT_FILE__USER != TT_FILE__OTP );

	assert_true( TT_EVENT__UNDEFINED != TT_EVENT__USER_CREATED );
	assert_true( TT_EVENT__USER_CREATED != TT_EVENT__AUTH_USER );
	assert_true( TT_EVENT__AUTH_USER != TT_EVENT__USER_MODIFIED );
	assert_true( TT_EVENT__USER_MODIFIED != TT_EVENT__USER_DELETED );
	assert_true( TT_EVENT__USER_DELETED != TT_EVENT__OTP_CREATED );
	assert_true( TT_EVENT__OTP_CREATED != TT_EVENT__AUTH_OTP );
	assert_true( TT_EVENT__AUTH_OTP != TT_EVENT__OTP_DELETED );
}


static void test_compile_version(void **state) {
	(void)	state; /* unused */

	assert_true( TT_VERSION.major == TT_VERSION_MAJOR );
	assert_true( TT_VERSION.minor == TT_VERSION_MINOR );
	assert_true( TT_VERSION.patch == TT_VERSION_PATCH );
}


static void test_compile_log(void **state) {
	(void)	state; /* unused */

	assert_true( TT_LOG_FATAL( "cmocka/00_compile", "TT_LOG_FATAL" ) == TT_LOG__FATAL );
	assert_true( TT_LOG_ERROR( "cmocka/00_compile", "TT_LOG_ERROR" ) == TT_LOG__ERROR );
	assert_true( TT_LOG_WARN( "cmocka/00_compile", "TT_LOG_WARN" ) == TT_LOG__WARN );
	assert_true( TT_LOG_INFO( "cmocka/00_compile", "TT_LOG_INFO" ) == TT_LOG__INFO );

	assert_true( TT_LOG( TT_LOG__FATAL, "cmocka/00_compile", "TT_LOG(TT_LOG__FATAL)" ) == TT_LOG__FATAL );
	assert_true( TT_LOG( TT_LOG__ERROR, "cmocka/00_compile", "TT_LOG(TT_LOG__ERROR)" ) == TT_LOG__ERROR );
	assert_true( TT_LOG( TT_LOG__WARN, "cmocka/00_compile", "TT_LOG(TT_LOG__WARN)" ) == TT_LOG__WARN );
	assert_true( TT_LOG( TT_LOG__INFO, "cmocka/00_compile", "TT_LOG(TT_LOG__INFO)" ) == TT_LOG__INFO );
}


int main(void) {
	const UnitTest tests[] = {
		unit_test(test_compile_defines),
		unit_test(test_compile_enums),
		unit_test(test_compile_version),
		unit_test(test_compile_log)
	};
	return run_tests(tests);
}

