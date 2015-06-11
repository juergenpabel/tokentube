#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "libtokentube.h"


int libtokentube_init_initialize();
int libtokentube_conf_configure(const char* filename);

int libtokentube_debug_initialize();
int libtokentube_log_initialize();
int libtokentube_runtime_initialize();
int libtokentube_plugin__initialize();
int libtokentube_event_initialize();
int libtokentube_crypto_initialize();
int libtokentube_library_initialize();

int libtokentube_debug_configure();
int libtokentube_log_configure();
int libtokentube_runtime_configure();
int libtokentube_plugin__configure();
int libtokentube_event_configure();
int libtokentube_crypto_configure();
int libtokentube_library_configure();

int libtokentube_library_finalize();
int libtokentube_crypto_finalize();
int libtokentube_event_finalize();
int libtokentube_plugin__finalize();
int libtokentube_runtime_finalize();
int libtokentube_conf_finalize();
int libtokentube_log_finalize();
int libtokentube_debug_finalize();

extern char* __progname;


typedef enum {
	TT_STATE_UNDEFINED,
	TT_STATE_UNINITIALIZED,
	TT_STATE_INITIALIZED,
	TT_STATE_CONFIGURED
} tt_state;

static tt_state	g_state = TT_STATE_UNINITIALIZED;


int tt_initialize(tt_version_t version) {
	int result = TT_OK;

	if( g_state == TT_STATE_INITIALIZED ) {
		return TT_OK;
	}
	if( g_state != TT_STATE_UNINITIALIZED ) {
		fprintf( stderr, "TOKENTUBE - ABORTING in %s() BECAUSE INTERNAL ERROR\n", __FUNCTION__ );
		return TT_ERR;
	}
	g_state = TT_STATE_UNDEFINED;

	if( libtokentube_debug_initialize() != TT_OK ) {
		fprintf( stderr, "TOKENTUBE - ABORTING in %s() BECAUSE INTERNAL ERROR\n", __FUNCTION__ );
		return TT_ERR;
	}
	TT_DEBUG1( "library/core", "initializing... (client version='%d.%d.%d')", version.major, version.minor, version.patch );
	if( version.major > TT_VERSION.major ) {
		fprintf( stderr, "TOKENTUBE - ABORTING BECAUSE LIBRARY(libtokentube.so) TOO OLD FOR BINARY(%s)\n", __progname );
		result = TT_ERR;
	}
	if( version.major < TT_VERSION.major ) {
		fprintf( stderr, "TOKENTUBE - ABORTING BECAUSE BINARY(%s) TOO OLD FOR LIBRARY(libtokentube.so)\n", __progname );
		result = TT_ERR;
	}
	if( result == TT_OK ) {
		TT_DEBUG2( "library/core", "initializing module 'log'..." );
		result = libtokentube_log_initialize();
		TT_DEBUG2( "library/core", "...initialized module 'log'" );
	}
	if( result == TT_OK ) {
		TT_DEBUG2( "library/core", "initializing module 'runtime'..." );
		result = libtokentube_runtime_initialize();
		TT_DEBUG2( "library/core", "...initialized module 'runtime'" );
	}
	if( result == TT_OK ) {
		TT_DEBUG2( "library/core", "initializing module 'crypto'..." );
		result = libtokentube_crypto_initialize();
		TT_DEBUG2( "library/core", "...initialized module 'crypto'" );
	}
	if( result == TT_OK ) {
		TT_DEBUG2( "library/core", "initializing module 'event'..." );
		result = libtokentube_event_initialize();
		TT_DEBUG2( "library/core", "...initialized module 'event'" );
	}
	if( result == TT_OK ) {
		TT_DEBUG2( "library/core", "initializing module 'library'..." );
		result = libtokentube_library_initialize();
		TT_DEBUG2( "library/core", "...initialized module 'library'" );
	}
	if( result == TT_OK ) {
		TT_DEBUG2( "library/core", "initializing module 'plugin'..." );
		result = libtokentube_plugin__initialize();
		TT_DEBUG2( "library/core", "...initialized module 'plugin'" );
	}
	if( result != TT_OK ) {
		TT_LOG_FATAL( "library/core", "initialization failed, ABORTING" );
	}
	if( result == TT_OK ) {
		TT_DEBUG2( "library/core", "initialization completed" );
		g_state = TT_STATE_INITIALIZED;
	}
	if( g_state == TT_STATE_UNDEFINED ) {
		g_state = TT_STATE_UNINITIALIZED;
		return TT_ERR;
	}
	return result;
}


int tt_configure(const char* filename) {
	int	result = TT_ERR;

	if( g_state == TT_STATE_CONFIGURED ) {
		return TT_OK;
	}
	TT_DEBUG1( "library/core", "configuring... (filename='%s')", filename );
	if( g_state != TT_STATE_INITIALIZED ) {
		fprintf( stderr, "TOKENTUBE - ABORTING in %s() BECAUSE INTERNAL ERROR\n", __FUNCTION__ );
		return TT_ERR;
	}
	g_state = TT_STATE_UNDEFINED;

	result = libtokentube_conf_configure( filename );
	if( result == TT_IGN ) {
		g_state = TT_STATE_INITIALIZED;
		return TT_IGN;
	}
	if( result == TT_OK ) {
		TT_DEBUG2( "library/core", "configuring module 'debug'..." );
		result = libtokentube_debug_configure();
		TT_DEBUG2( "library/core", "...configured module 'debug'" );
	}
	if( result == TT_OK ) {
		TT_DEBUG2( "library/core", "configuring module 'log'..." );
		result = libtokentube_log_configure();
		TT_DEBUG2( "library/core", "...configured module 'log'" );
	}
	if( result == TT_OK ) {
		TT_DEBUG2( "library/core", "configuring module 'runtime'..." );
		result = libtokentube_runtime_configure();
		TT_DEBUG2( "library/core", "...configured module 'runtime'" );
	}
	if( result == TT_OK ) {
		TT_DEBUG2( "library/core", "configuring module 'crypto'..." );
		result = libtokentube_crypto_configure();
		TT_DEBUG2( "library/core", "...configured module 'crypto'" );
	}
	if( result == TT_OK ) {
		TT_DEBUG2( "library/core", "configuring module 'event'..." );
		result = libtokentube_event_configure();
		TT_DEBUG2( "library/core", "...configured module 'event'" );
	}
	if( result == TT_OK ) {
		TT_DEBUG2( "library/core", "configuring module 'library'..." );
		result = libtokentube_library_configure();
		TT_DEBUG2( "library/core", "...configured module 'library'" );
	}
	if( result == TT_OK ) {
		TT_DEBUG2( "library/core", "configuring module 'plugin'..." );
		result = libtokentube_plugin__configure();
		TT_DEBUG2( "library/core", "...configured module 'plugin'" );
	}
	if( result != TT_OK ) {
		TT_LOG_FATAL( "library/core", "configuration failed, ABORTING" );
	}
	if( result == TT_OK ) {
		TT_DEBUG2( "library/core", "configuration completed" );
		g_state = TT_STATE_CONFIGURED;
	}
	return result;
}


int tt_finalize() {
	int result = TT_OK;

	if( g_state == TT_STATE_UNINITIALIZED ) {
		return TT_IGN;
	}
	TT_DEBUG1( "library/core", "finalizing..." );
	if( g_state != TT_STATE_INITIALIZED && g_state != TT_STATE_CONFIGURED ) {
		fprintf( stderr, "TOKENTUBE - FAILING QUIETLY IN %s() BECAUSE OF INTERNAL ERROR\n", __FUNCTION__ );
		return TT_IGN;
	}
	g_state = TT_STATE_UNDEFINED;
	if( result == TT_OK ) {
		TT_DEBUG2( "library/core", "finalizing module 'plugin'..." );
		result = libtokentube_plugin__finalize();
		TT_DEBUG2( "library/core", "...finalized module 'plugin'" );
	}
	if( result == TT_OK ) {
		TT_DEBUG2( "library/core", "finalizing module 'library'..." );
		result = libtokentube_library_finalize();
		TT_DEBUG2( "library/core", "...finalized module 'library'" );
	}
	if( result == TT_OK ) {
		TT_DEBUG2( "library/core", "finalizing module 'event'..." );
		result = libtokentube_event_finalize();
		TT_DEBUG2( "library/core", "...finalized module 'event'" );
	}
	if( result == TT_OK ) {
		TT_DEBUG2( "library/core", "finalizing module 'crypto'..." );
		result = libtokentube_crypto_finalize();
		TT_DEBUG2( "library/core", "...finalized module 'crypto'" );
	}
	if( result == TT_OK ) {
		TT_DEBUG2( "library/core", "finalizing module 'conf'..." );
		result = libtokentube_conf_finalize();
		TT_DEBUG2( "library/core", "...finalized module 'conf'" );
	}
	if( result == TT_OK ) {
		TT_DEBUG2( "library/core", "finalizing module 'log'..." );
		result = libtokentube_log_finalize();
		TT_DEBUG2( "library/core", "...finalized module 'log'" );
	}
	if( result == TT_OK ) {
		result = libtokentube_debug_finalize();
		g_state = TT_STATE_UNINITIALIZED;
	}
	return result;
}


__attribute__ ((visibility ("hidden")))
int tt_reset() {
	g_state = TT_STATE_UNINITIALIZED;
	return TT_OK;
}

