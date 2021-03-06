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
int libtokentube_plugin__file_load(tt_file_t type, const char* identifier, char* buffer, size_t* buffer_len) {
	tt_module_t*	module;
	size_t		i;

	TT_TRACE( "library/plugin", "%s(type=%d,identifier='%s')", __FUNCTION__, (int)type, identifier );
	if( type == TT_FILE__UNDEFINED || identifier == NULL || identifier[0] == '\0' || buffer == NULL || buffer_len == NULL || *buffer_len == 0 ) {
		TT_LOG_ERROR( "library/plugin", "invalid parameters in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	TT_DEBUG3( "library/plugin", "invoking 'file_load' handlers" );
	for( i=0; i<MAX_PLUGINS+1; i++ ) {
		module = g_modules[i];
		if( module != NULL && module->plugin != NULL ) {
			TT_DEBUG5( "library/plugin", "checking 'file_load' for plugin '%s'", module->name );
			if( module->plugin->interface.api.storage.file_load != NULL ) {
				TT_DEBUG4( "library/plugin", "invoking 'file_load' for plugin '%s'", module->name );
				switch( module->plugin->interface.api.storage.file_load(type, identifier, buffer, buffer_len) ) {
					case TT_OK:
						TT_DEBUG4( "library/plugin", "plugin '%s' successfully handled 'file_load'", module->name );
						return TT_OK;
					case TT_IGN:
						TT_DEBUG5( "library/plugin", "plugin '%s' ignored 'file_load'", module->name );
						break;
					default:
						TT_LOG_ERROR( "library/plugin", "plugin '%s' returned error for 'file_load'", module->name );
				}
			}
		}
	}
	TT_LOG_ERROR( "library/plugin", "no plugin handled 'file_load', returning TT_ERR" );
	return TT_ERR;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_plugin__file_save(tt_file_t type, const char* identifier, char* buffer, size_t buffer_len) {
	tt_module_t* module;
	size_t i;

	TT_TRACE( "library/plugin", "%s(type=%d,identifier='%s')", __FUNCTION__, (int)type, identifier );
	if( type == TT_FILE__UNDEFINED || identifier == NULL || identifier[0] == '\0' || buffer == NULL || buffer_len == 0 ) {
		TT_LOG_ERROR( "library/plugin", "invalid parameters in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	TT_DEBUG3( "library/plugin", "invoking 'file_save' handlers" );
	for( i=0; i<MAX_PLUGINS+1; i++ ) {
		module = g_modules[i];
		if( module != NULL && module->plugin != NULL ) {
			TT_DEBUG5( "library/plugin", "checking 'file_save' for plugin '%s'", module->name );
			if( module->plugin->interface.api.storage.file_save != NULL ) {
				switch( module->plugin->interface.api.storage.file_save(type, identifier, buffer, buffer_len) ) {
					case TT_OK:
						TT_DEBUG4( "library/plugin", "plugin '%s' successfully handled 'file_save'", module->name );
						return TT_OK;
					case TT_IGN:
						TT_DEBUG5( "library/plugin", "plugin '%s' ignored 'file_save'", module->name );
						break;
					default:
						TT_LOG_ERROR( "library/plugin", "plugin '%s' returned error for 'file_save'", module->name );
				}
			}
		}
	}
	TT_LOG_ERROR( "library/plugin", "no plugin handled 'file_save', returning TT_ERR" );
	return TT_ERR;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_plugin__file_exists(tt_file_t type, const char* identifier, tt_status_t* status) {
	tt_module_t* module;
	size_t	i;

	TT_TRACE( "library/plugin", "%s(type=%d,identifier='%s')", __FUNCTION__, (int)type, identifier );
	if( type == TT_FILE__UNDEFINED || identifier == NULL || identifier[0] == '\0' || status == NULL ) {
		TT_LOG_ERROR( "library/plugin", "invalid parameters in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	TT_DEBUG3( "library/plugin", "invoking 'file_exists' handlers" );
	*status = TT_STATUS__UNDEFINED;
	for( i=0; i<MAX_PLUGINS+1; i++ ) {
		module = g_modules[i];
		if( module != NULL && module->plugin != NULL ) {
			TT_DEBUG5( "library/plugin", "checking 'file_exists' for plugin '%s'", module->name );
			if( module->plugin->interface.api.storage.file_exists != NULL ) {
				switch( module->plugin->interface.api.storage.file_exists(type, identifier, status) ) {
					case TT_OK:
						if( *status == TT_YES ) {
							TT_DEBUG4( "library/plugin", "plugin '%s' return TT_YES for 'file_exists'", module->name );
							return TT_OK;
						} else {
							TT_DEBUG4( "library/plugin", "plugin '%s' return TT_NO for 'file_exists'", module->name );
						}
						break;
					case TT_IGN:
						TT_DEBUG5( "library/plugin", "plugin '%s' ignored 'file_exists'", module->name );
						break;
					default:
						TT_LOG_ERROR( "library/plugin", "plugin '%s' returned error for 'file_exists'", module->name );
				}
			}
		}
	}
	if( *status == TT_NO ) {
		TT_DEBUG3( "library/plugin", "returning TT_NO for 'file_exists'" );
		return TT_OK;
	}
	TT_LOG_ERROR( "library/plugin", "no plugin handled 'file_exists', returning TT_ERR" );
	return TT_ERR;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_plugin__file_delete(tt_file_t type, const char* identifier, tt_status_t* status) {
	tt_module_t*	module;
	size_t		i;

	TT_TRACE( "library/plugin", "%s(type=%d,identifier='%s')", __FUNCTION__, (int)type, identifier );
	if( type == TT_FILE__UNDEFINED || identifier == NULL || identifier[0] == '\0' || status == NULL ) {
		TT_LOG_ERROR( "library/plugin", "invalid parameters in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	TT_DEBUG3( "library/plugin", "invoking 'file_delete' handlers" );
	*status = TT_STATUS__UNDEFINED;
	for( i=0; i<MAX_PLUGINS+1; i++ ) {
		module = g_modules[i];
		if( module != NULL && module->plugin != NULL ) {
			TT_DEBUG5( "library/plugin", "checking 'file_exists' for plugin '%s'", module->name );
			if( module->plugin->interface.api.storage.file_delete != NULL ) {
				switch( module->plugin->interface.api.storage.file_delete(type, identifier, status) ) {
					case TT_OK:
						if( *status == TT_YES ) {
							TT_DEBUG4( "library/plugin", "plugin '%s' successfully handled 'file_delete'", module->name );
							return TT_OK;
						} else {
							TT_DEBUG4( "library/plugin", "plugin '%s' return TT_NO for 'file_delete'", module->name );
						}
						break;
					case TT_IGN:
						TT_DEBUG5( "library/plugin", "plugin '%s' ignored 'file_delete'", module->name );
						break;
					default:
						TT_LOG_ERROR( "library/plugin", "plugin '%s' returned error for 'file_delete'", module->name );
				}
			}
		}
	}
	if( *status == TT_NO ) {
		TT_DEBUG3( "library/plugin", "returning TT_NO for 'file_delete'", module->name );
		return TT_OK;
	}
	TT_LOG_ERROR( "library/plugin", "no plugin handled 'file_delete', returning TT_ERR" );
	return TT_ERR;
}

