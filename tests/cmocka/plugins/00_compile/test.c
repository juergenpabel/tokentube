#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <setjmp.h>
#include <dlfcn.h>
#include <cmocka.h>
#include <tokentube/plugin.h>


static int g_plugins_loaded = 0;


int tt_plugin_register(tt_plugin_t* plugin) {
	if( plugin != NULL ) {
		g_plugins_loaded++;
	}
	return TT_OK;
}


static void test_plugins(void **state) {
	assert_true( *state == NULL );
	assert_true( dlopen( "lib/tokentube/plugins.d/libtokentube-plugin-test.so", RTLD_GLOBAL|RTLD_NOW ) != NULL );
	assert_true( dlopen( "lib/tokentube/plugins.d/libtokentube-plugin-helpdesk.so", RTLD_GLOBAL|RTLD_NOW ) != NULL );
	assert_true( dlopen( "lib/tokentube/plugins.d/libtokentube-plugin-exec.so", RTLD_GLOBAL|RTLD_NOW ) != NULL );
	assert_true( dlopen( "lib/tokentube/plugins.d/libtokentube-plugin-log.so", RTLD_GLOBAL|RTLD_NOW ) != NULL );
	assert_true( dlopen( "lib/tokentube/plugins.d/libtokentube-plugin-syslog.so", RTLD_GLOBAL|RTLD_NOW ) != NULL );
	assert_true( dlopen( "lib/tokentube/plugins.d/libtokentube-plugin-lua.so", RTLD_GLOBAL|RTLD_NOW ) != NULL );
	assert_true( g_plugins_loaded == 6 );
}


int main(void) {
	const UnitTest tests[] = {
		unit_test_setup_teardown(test_plugins, NULL, NULL),
	};
	return run_tests(tests);
}

