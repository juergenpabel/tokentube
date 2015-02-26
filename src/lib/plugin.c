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
tt_module_t* g_modules[MAX_PLUGINS+1] = { NULL };


__attribute__ ((visibility ("hidden")))
int libtokentube_plugin__initialize() {
	TT_DEBUG2( "library/plugin", "loading plugin 'default' (ID=%zd)...", MAX_PLUGINS );
	g_modules[MAX_PLUGINS] = (tt_module_t*)malloc( sizeof(tt_module_t) );
	if( g_modules[MAX_PLUGINS] == NULL ) {
		TT_LOG_FATAL( "library/plugin", "out-of-memory error while loading plugin 'default'" );
		TT_DEBUG1( "library/plugin", "malloc() failed for plugin 'default' in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	g_modules[MAX_PLUGINS]->name = "default";
	g_modules[MAX_PLUGINS]->elf = NULL;
	g_modules[MAX_PLUGINS]->plugin = malloc( sizeof(tt_plugin_t) );
	if( g_modules[MAX_PLUGINS]->plugin == NULL ) {
		TT_LOG_FATAL( "library/plugin", "out-of-memory error while loading plugin 'default'" );
		TT_DEBUG1( "library/plugin", "malloc() failed for plugin 'default' in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	memset( g_modules[MAX_PLUGINS]->plugin, '\0', sizeof(tt_plugin_t) );
	if( default__initialize( g_modules[MAX_PLUGINS]->plugin ) != TT_OK ) {
		TT_LOG_FATAL( "library/plugin", "initialization failed for plugin 'default'" );
		TT_DEBUG1( "library/plugin", "default__initialize() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	TT_DEBUG3( "library/plugin", "...plugin 'default' loaded (ID=%zd)", MAX_PLUGINS );
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_plugin__configure() {
	char	plugin[FILENAME_MAX+1] = {0};
	size_t	plugin_size = sizeof(plugin);
	char	library[FILENAME_MAX+1] = {0};
	size_t	library_size = sizeof(library);
	char	config[FILENAME_MAX+1] = {0};
	size_t	config_size = sizeof(config);
	size_t	i = 0;

	plugin_size = sizeof(plugin);
	if( libtokentube_conf_read_list( "plugins|enabled", 0, plugin, &plugin_size ) != TT_OK ) {
		TT_LOG_FATAL( "library/plugin", "failed to get enabled plugin from config ('plugins|enabled') at index=0" );
		return TT_ERR;
	}
	while( plugin_size>0 && i<MAX_PLUGINS ) {
		library_size = sizeof(library);
		if( libtokentube_conf_read_plugin_library( plugin, library, &library_size ) != TT_OK ) {
			TT_LOG_FATAL( "library/plugin", "failed to get plugin library from config ('plugin|%s|library')", plugin );
			TT_DEBUG1( "library/plugin", "libtokentube_conf_read_plugin_library() failed in %s()", __FUNCTION__ );
			return TT_ERR;
		}
		g_modules[i] = (tt_module_t*)malloc( sizeof(tt_module_t) );
		if( g_modules[i] == NULL ) {
			TT_LOG_FATAL( "library/plugin", "out-of-memory error while loading plugin '%s'", plugin );
			TT_DEBUG1( "library/plugin", "malloc() failed for plugin '%s'", plugin );
			return TT_ERR;
		}
		TT_DEBUG3( "library/plugin", "loading plugin '%s' (ID=%zd)...", plugin, i );
		g_modules[i]->name = strndup( plugin, 64 );
		if( g_modules[i]->name == NULL ) {
			TT_LOG_FATAL( "library/plugin", "out-of-memory error while loading plugin '%s'", plugin );
			TT_DEBUG1( "library/plugin", "malloc() failed for plugin '%s'", plugin );
			return TT_ERR;
		}
		g_modules[i]->plugin = NULL;
		g_modules[i]->elf = dlopen( library, RTLD_GLOBAL|RTLD_NOW );
		if( g_modules[i]->elf == NULL ) {
			TT_LOG_FATAL( "library/plugin", "plugin '%s' failed to load correctly (dlopen failed)", plugin );
			free( (void*)g_modules[i]->name );
			free( g_modules[i] );
			g_modules[i] = NULL;
			return TT_ERR;
		}
		if( g_modules[i]->plugin == NULL ) {
			TT_LOG_FATAL( "library/plugin", "plugin '%s' is not compatible, unloading", plugin );
			TT_DEBUG3( "library/plugin", "...unloading plugin '%s' (ID=%zd)", plugin, i );
			g_modules[i]->plugin = NULL;
			dlclose( g_modules[i]->elf );
			g_modules[i]->elf = NULL;
			return TT_ERR;
		}
		if( tt_discover( &g_modules[i]->plugin->library ) != TT_OK ) {
			TT_LOG_FATAL( "library/plugin", "tt_client_register() failed for plugin '%s'", plugin );
			TT_DEBUG3( "library/plugin", "finalizing plugin '%s' (ID=%zd)...", plugin, i );
			if( g_modules[i]->plugin->meta.finalize != NULL ) {
				g_modules[i]->plugin->meta.finalize();
			}
			g_modules[i]->plugin = NULL;
			return TT_ERR;
		}
		if( g_modules[i]->plugin->meta.configure != NULL ) {
			config_size = sizeof(config);
			if( libtokentube_conf_read_plugin_config( g_modules[i]->name, config, &config_size ) != TT_OK ) {
				TT_LOG_FATAL( "library/plugin", "failed to get config filename for plugin '%s'", plugin );
				TT_DEBUG3( "library/plugin", "finalizing plugin '%s' (ID=%zd)...", plugin, i );
				if( g_modules[i]->plugin->meta.finalize != NULL ) {
					g_modules[i]->plugin->meta.finalize();
				}
				g_modules[i]->plugin = NULL;
				return TT_ERR;
			}
			if( g_modules[i]->plugin->meta.configure( config ) != TT_OK ) {
				TT_LOG_FATAL( "library/plugin", "plugin '%s' failed to configure itself", plugin );
				TT_DEBUG3( "library/plugin", "finalizing plugin '%s' (ID=%zd)...", plugin, i );
				if( g_modules[i]->plugin->meta.finalize != NULL ) {
					g_modules[i]->plugin->meta.finalize();
				}
				g_modules[i]->plugin = NULL;
				return TT_ERR;
			}
		}
		TT_DEBUG3( "library/plugin", "...plugin '%s' loaded (ID=%zd)", plugin, i );
		i++;
		plugin_size = sizeof(plugin);
		if( libtokentube_conf_read_list( "plugins|enabled", i, plugin, &plugin_size ) != TT_OK ) {
			TT_LOG_FATAL( "library/plugin", "failed to get enabled plugin from config ('plugins|enabled') at index=%zd", i );
			return TT_ERR;
		}
	}
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_plugin__finalize() {
	tt_module_t*	module = NULL;
	size_t		i = 0;

	for( i=MAX_PLUGINS; i>0; i-- ) {
		if( g_modules[i-1] != NULL ) {
			module =  g_modules[i-1];
			TT_DEBUG3( "library/plugin", "unloading plugin '%s' (ID=%zd)", module->name, i-1 );
			if( module->plugin != NULL ) {
				if( module->plugin->meta.finalize != NULL ) {
					if( module->plugin->meta.finalize() != TT_OK ) {
						TT_LOG_ERROR( "library/plugin", "finalize() failed for plugin '%s'", module->name );
						module->elf = NULL; /* attempt to evade error on dlclose() */
					}
				}
				if( module->elf != NULL ) {
					dlclose( module->elf ); 
					module->elf = NULL;
				}
				free(  (void*)module->name );
				module->name = NULL;
			}
			free( module );
		}
	}
	if( g_modules[MAX_PLUGINS] != NULL ) {
		TT_DEBUG3( "library/plugin", "unloading plugin 'default'" );
		default__finalize();
		free( g_modules[MAX_PLUGINS]->plugin );
		free( g_modules[MAX_PLUGINS] );
	}
	memset( g_modules, '\0', sizeof(g_modules) );
	return TT_OK;
}


int tt_plugin_register(tt_plugin_t* plugin) {
	char	filter[FILENAME_MAX+1];
	size_t	filter_size;
	char	str_plugin[12] = {0};
	size_t	id = 0, offset = 0;

	TT_DEBUG2( "library/plugin", "%s(plugin='%p')", __FUNCTION__, plugin );
	if( plugin == NULL ) {
		TT_LOG_ERROR( "library/plugin", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( TT_VERSION.major > plugin->meta.version.major || TT_VERSION.major < plugin->meta.version.major ) {
		VERSION_TO_STR( plugin->meta.version, str_plugin );
		TT_LOG_ERROR( "library/plugin", "incompatible plugin has version='%s'", str_plugin );
		return TT_ERR;
	}
	for( id=0; id<MAX_PLUGINS; id++ ) {
		if( g_modules[id] != NULL ) {
			if( g_modules[id]->plugin == NULL ) {
				break;
			}
		}
	}
	if( id >= MAX_PLUGINS ) {
		TT_LOG_ERROR( "library/plugin", "tt_plugin_register() failed to identify plugin" );
		return TT_ERR;
	}

	if( plugin->meta.initialize != NULL ) {
		if( plugin->meta.initialize() != TT_OK ) {
			TT_LOG_ERROR( "library/plugin", "plugin '%s' failed to initialize itself", g_modules[id]->name );
			if( plugin->meta.finalize != NULL ) {
				plugin->meta.finalize();
			}
			return TT_ERR;
		}
	}
	filter_size = sizeof(filter);
	if( libtokentube_conf_read_plugin_filter_api( g_modules[id]->name, offset, filter, &filter_size ) != TT_OK ) {
		TT_LOG_WARN( "library/plugin", "failed to get api filter from config ('plugins|plugin|filter|api') at index=0" );
		filter_size = 0;
	}
	while( filter_size > 0 ) {
		if( strncasecmp( filter, "runtime", 8 ) == 0 ) {
			memset( &plugin->interface.api.runtime, '\0', sizeof(plugin->interface.api.runtime) );
		}
		if( strncasecmp( filter, "storage", 8 ) == 0 ) {
			memset( &plugin->interface.api.storage, '\0', sizeof(plugin->interface.api.storage) );
		}
		if( strncasecmp( filter, "pba", 4 ) == 0 ) {
			memset( &plugin->interface.api.pba, '\0', sizeof(plugin->interface.api.pba) );
		}
		if( strncasecmp( filter, "user", 5 ) == 0 ) {
			memset( &plugin->interface.api.user, '\0', sizeof(plugin->interface.api.user) );
		}
		if( strncasecmp( filter, "otp", 4 ) == 0 ) {
			memset( &plugin->interface.api.otp, '\0', sizeof(plugin->interface.api.otp) );
		}
		offset++;
		filter_size = sizeof(filter);
		if( libtokentube_conf_read_plugin_filter_api( g_modules[id]->name, offset, filter, &filter_size ) != TT_OK ) {
			TT_LOG_WARN( "library/plugin", "failed to get api filter from config ('plugins|plugin|filter|api') at index=0" );
			break;
		}
	}
	offset = 0;
	filter_size = sizeof(filter);
	if( libtokentube_conf_read_plugin_filter_event( g_modules[id]->name, offset, filter, &filter_size ) != TT_OK ) {
		TT_LOG_WARN( "library/plugin", "failed to get event filter from config ('plugins|plugin|filter|event') at index=0" );
		filter_size = 0;
	}
	while( filter_size > 0 ) {
		if( strncasecmp( filter, "user", 5 ) == 0 ) {
			memset( &plugin->interface.events.user, '\0', sizeof(plugin->interface.events.user) );
		}
		if( strncasecmp( filter, "otp", 4 ) == 0 ) {
			memset( &plugin->interface.events.otp, '\0', sizeof(plugin->interface.events.otp) );
		}
		offset++;
		filter_size = sizeof(filter);
		if( libtokentube_conf_read_plugin_filter_event( g_modules[id]->name, offset, filter, &filter_size ) != TT_OK ) {
			TT_LOG_WARN( "library/plugin", "failed to get event filter from config ('plugins|plugin|filter|event') at index=0" );
			break;
		}
	}
	g_modules[id]->plugin = plugin;
	TT_DEBUG2( "library/plugin", "plugin '%s' registered (ID=%zd)", g_modules[id]->name, id );
	return TT_OK;
}

