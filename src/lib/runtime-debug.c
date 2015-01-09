#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libtokentube.h"


__attribute__ ((visibility ("hidden")))
tt_debuglevel_t g_debug_level = TT_DEBUG__UNDEFINED;


__attribute__ ((visibility ("hidden")))
int libtokentube_debug_initialize() {
	const char*	env_debug_level = NULL;

	env_debug_level = getenv( "TT_DEBUG_LEVEL" );
	if( env_debug_level != NULL ) {
		g_debug_level = atoi( env_debug_level );
		if( g_debug_level > 9 ) {
			g_debug_level = 9;
		}
	}
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_debug_finalize() {
	g_debug_level = TT_DEBUG__UNDEFINED;
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_debug_configure() {
	int	conf_level = TT_UNINITIALIZED;

	if( g_debug_level == TT_DEBUG__UNDEFINED ) {
		if( libtokentube_conf_read_int( "runtime|debug|level", &conf_level ) != TT_OK ) {
			TT_LOG_INFO( "library/runtime", "configuration error in %s()", __FUNCTION__ );
			return TT_ERR;
		}
		if( conf_level < 0 ) {
			conf_level = 0;
		}
		if( conf_level > 9 ) {
			conf_level = 9;
		}
		g_debug_level = conf_level;
	}
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_runtime_vdebug( tt_debuglevel_t level, const char* source, const char* message, va_list args ) {
	if( g_debug_level != TT_DEBUG__UNDEFINED && level <= g_debug_level ) {
		fprintf( stderr, "DEBUG%d [%-20s] ", level, source );
		vfprintf( stderr, message, args );
		fprintf( stderr, "\n" );
		fflush( stderr );
	}
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_runtime_debug( tt_debuglevel_t level, const char* source, const char* message, ... ) {
	va_list	args;
	int	result;

	va_start( args, message );
	result = libtokentube_runtime_vdebug( level, source, message, args );
	va_end( args );
	return result;
}

