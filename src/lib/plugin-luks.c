#include <stdlib.h>
#include <stdio.h>
#include <libgen.h>
#include <string.h>
#include <unistd.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <sys/types.h>
#include "libtokentube.h"


__attribute__ ((visibility ("hidden")))
int libtokentube_plugin__luks_load(char* key, size_t* key_len) {
	tt_module_t*	module;
	size_t		i;

	TT_TRACE( "library/plugin", "%s(key=%p,key_len=%p)", __FUNCTION__, key, key_len );
	if( key == NULL || key_len == NULL || *key_len == 0 ) {
		TT_LOG_ERROR( "library/plugin", "invalid parameters in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	TT_DEBUG3( "library/plugin", "invoking 'luks_load' handlers" );
	for( i=0; i<MAX_PLUGINS+1; i++ ) {
		module = g_modules[i];
		if( module != NULL && module->plugin != NULL ) {
			TT_DEBUG5( "library/plugin", "checking 'luks_load' handler for plugin '%s'", module->name );
			if( module->plugin->interface.api.storage.luks_load != NULL ) {
				TT_DEBUG4( "library/plugin", "invoking 'luks_load' handler for plugin '%s'", module->name );
				switch( module->plugin->interface.api.storage.luks_load(key, key_len) ) {
					case TT_OK:
						TT_DEBUG4( "library/plugin", "plugin '%s' successfully handled 'luks_load'", module->name );
						return TT_OK;
					case TT_IGN:
						TT_DEBUG5( "library/plugin", "plugin '%s' ignored 'luks_load'", module->name );
						break;
					default:
						TT_LOG_ERROR( "library/plugin", "plugin '%s' returned error for 'luks_load'", module->name );
				}
			}
		}
	}
	TT_LOG_ERROR( "library/plugin", "no plugin handled 'luks_load', returning TT_ERR" );
	return TT_ERR;
}

