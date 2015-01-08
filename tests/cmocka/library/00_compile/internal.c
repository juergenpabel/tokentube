#include <stdio.h>
#include <stdarg.h>
#include <tokentube/defines.h>
#include <tokentube/enums.h>


int libtokentube_runtime_log( tt_loglevel_t level, const char* source, const char* message, ... ) {
        va_list	args;

        (void) source; /* unused */
        va_start( args, message );
        va_end( args );
	return level;
}

