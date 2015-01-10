#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.60)
AC_INIT(tokentube, 1.0.0, tokentube-devel@sourceforge.net)
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])
# AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MAKE_SET

AM_MAINTAINER_MODE([disable])

# Checks for libraries.
AM_PROG_LIBTOOL

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

CFLAGS="-g -O0 -Wall -Wextra -Werror -Wno-missing-braces -Wno-missing-field-initializers"
AC_CONFIG_FILES([Makefile src/lib/Makefile src/pam/Makefile src/plugins/Makefile src/plugins/exec/Makefile src/plugins/log/Makefile src/plugins/syslog/Makefile src/plugins/test/Makefile src/plugins/helpdesk/Makefile src/plugins/lua/Makefile src/bin/cli/Makefile src/bin/pba/Makefile src/bin/ssod/Makefile src/bin/sso-greeter-lightdm/Makefile tests/cmocka/Makefile tests/cmocka/library/Makefile tests/cmocka/library/00_compile/Makefile tests/cmocka/library/01_init/Makefile tests/cmocka/library/02_config/Makefile tests/cmocka/library/03_testvectors/Makefile tests/cmocka/library/04_user/Makefile tests/cmocka/library/05_otp/Makefile tests/cmocka/plugins/Makefile tests/cmocka/plugins/00_compile/Makefile tests/cmocka/plugins/01_init/Makefile tests/cmocka/plugins/02_helpdesk/Makefile tests/cmocka/pam/Makefile tests/cmocka/pam/libpam/Makefile tests/cmocka/pam/00_compile/Makefile tests/cmocka/pam/01_init/Makefile tests/cmocka/pam/02_sm_authenticate/Makefile tests/cmocka/pam/03_sm_chauthtok/Makefile])
AC_OUTPUT
