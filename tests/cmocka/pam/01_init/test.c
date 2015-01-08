#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <setjmp.h>
#include <cmocka.h>
#include <tokentube.h>

#define PAM_SM_SESSION

#include <security/pam_modules.h>
#include <security/pam_ext.h>


static void test_pam_init(void **state) {
	(void)state; /* unused */
}


static void self_setup(void **state) {
	(void)state; /* unused */
}


static void self_teardown(void **state) {
	(void)state; /* unused */
}


int main(void) {
	const UnitTest tests[] = {
		unit_test_setup_teardown(test_pam_init, self_setup, self_teardown),
	};
	return run_tests(tests);
}

