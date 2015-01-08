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


int tt_initialize(tt_version_t version) {
	int result = TT_OK;

	if( libtokentube_debug_initialize() != TT_OK ) {
		fprintf( stderr, "TOKENTUBE - ABORTING BECAUSE INTERNAL ERROR\n" );
		return TT_ERR;
	}
	TT_DEBUG1( "library/init", "initializing... (client version='%d.%d.%d')", version.major, version.minor, version.patch );
	if( version.major > TT_VERSION.major ) {
		fprintf( stderr, "TOKENTUBE - ABORTING BECAUSE LIBRARY(libtokentube.so) TOO OLD FOR BINARY(%s)\n", __progname );
		result = TT_ERR;
	}
	if( version.major < TT_VERSION.major ) {
		fprintf( stderr, "TOKENTUBE - ABORTING BECAUSE BINARY(%s) TOO OLD FOR LIBRARY(libtokentube.so)\n", __progname );
		result = TT_ERR;
	}
	if( result == TT_OK ) {
		TT_DEBUG2( "library/init", "initializing module 'log'..." );
		result = libtokentube_log_initialize();
		TT_DEBUG2( "library/init", "...initialized module 'log'" );
	}
	if( result == TT_OK ) {
		TT_DEBUG2( "library/init", "initializing module 'runtime'..." );
		result = libtokentube_runtime_initialize();
		TT_DEBUG2( "library/init", "...initialized module 'runtime'" );
	}
	if( result == TT_OK ) {
		TT_DEBUG2( "library/init", "initializing module 'crypto'..." );
		result = libtokentube_crypto_initialize();
		TT_DEBUG2( "library/init", "...initialized module 'crypto'" );
	}
	if( result == TT_OK ) {
		TT_DEBUG2( "library/init", "initializing module 'event'..." );
		result = libtokentube_event_initialize();
		TT_DEBUG2( "library/init", "...initialized module 'event'" );
	}
	if( result == TT_OK ) {
		TT_DEBUG2( "library/init", "initializing module 'library'..." );
		result = libtokentube_library_initialize();
		TT_DEBUG2( "library/init", "...initialized module 'library'" );
	}
	if( result == TT_OK ) {
		TT_DEBUG2( "library/init", "initializing module 'plugin'..." );
		result = libtokentube_plugin__initialize();
		TT_DEBUG2( "library/init", "...initialized module 'plugin'" );
	}
	if( result != TT_OK ) {
		TT_LOG_FATAL( "library/init", "initialization failed, ABORTING" );
	}
	return result;
}


int tt_configure(const char* filename) {
	int	result = TT_ERR;

	TT_DEBUG1( "library/init", "configuring... (filename='%s')", filename );
	result = libtokentube_conf_configure( filename );
	if( result == TT_OK ) {
		TT_DEBUG2( "library/init", "configuring module 'debug'..." );
		result = libtokentube_debug_configure();
		TT_DEBUG2( "library/init", "...configured module 'debug'" );
	}
	if( result == TT_OK ) {
		TT_DEBUG2( "library/init", "configuring module 'log'..." );
		result = libtokentube_log_configure();
		TT_DEBUG2( "library/init", "...configured module 'log'" );
	}
	if( result == TT_OK ) {
		TT_DEBUG2( "library/init", "configuring module 'runtime'..." );
		result = libtokentube_runtime_configure();
		TT_DEBUG2( "library/init", "...configured module 'runtime'" );
	}
	if( result == TT_OK ) {
		TT_DEBUG2( "library/init", "configuring module 'crypto'..." );
		result = libtokentube_crypto_configure();
		TT_DEBUG2( "library/init", "...configured module 'crypto'" );
	}
	if( result == TT_OK ) {
		TT_DEBUG2( "library/init", "configuring module 'event'..." );
		result = libtokentube_event_configure();
		TT_DEBUG2( "library/init", "...configured module 'event'" );
	}
	if( result == TT_OK ) {
		TT_DEBUG2( "library/init", "configuring module 'library'..." );
		result = libtokentube_library_configure();
		TT_DEBUG2( "library/init", "...configured module 'library'" );
	}
	if( result == TT_OK ) {
		TT_DEBUG2( "library/init", "configuring module 'plugin'..." );
		result = libtokentube_plugin__configure();
		TT_DEBUG2( "library/init", "...configured module 'plugin'" );
	}
	if( result == TT_OK ) {
		TT_DEBUG2( "library/init", "configuration completed" );
	} else {
		TT_LOG_FATAL( "library/init", "configuration failed, ABORTING" );
	}
	return result;
}


int tt_finalize() {
	int result = TT_OK;

	TT_DEBUG1( "library/init", "finalizing..." );
	if( result == TT_OK ) {
		TT_DEBUG2( "library/init", "finalizing module 'plugin'..." );
		result = libtokentube_plugin__finalize();
		TT_DEBUG2( "library/init", "...finalized module 'plugin'" );
	}
	if( result == TT_OK ) {
		TT_DEBUG2( "library/init", "finalizing module 'library'..." );
		result = libtokentube_library_finalize();
		TT_DEBUG2( "library/init", "...finalized module 'library'" );
	}
	if( result == TT_OK ) {
		TT_DEBUG2( "library/init", "finalizing module 'event'..." );
		result = libtokentube_event_finalize();
		TT_DEBUG2( "library/init", "...finalized module 'event'" );
	}
	if( result == TT_OK ) {
		TT_DEBUG2( "library/init", "finalizing module 'crypto'..." );
		result = libtokentube_crypto_finalize();
		TT_DEBUG2( "library/init", "...finalized module 'crypto'" );
	}
	if( result == TT_OK ) {
		TT_DEBUG2( "library/init", "finalizing module 'conf'..." );
		result = libtokentube_conf_finalize();
		TT_DEBUG2( "library/init", "...finalized module 'conf'" );
	}
	if( result == TT_OK ) {
		TT_DEBUG2( "library/init", "finalizing module 'log'..." );
		result = libtokentube_log_finalize();
		TT_DEBUG2( "library/init", "...finalized module 'log'" );
	}
	if( result == TT_OK ) {
		result = libtokentube_debug_finalize();
	}
	return result;
}

