#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <string.h>
#include <confuse.h>
#include <lua5.2/lua.h>
#include <lua5.2/lualib.h>
#include <lua5.2/luaconf.h>
#include <lua5.2/lauxlib.h>
#include <tokentube/plugin.h>
#include "plugin.h"


static char* g_type[] = { NULL, "config-pba", "config-standard", "key", "user", "otp", "helpdesk" };


__attribute__ ((visibility ("hidden")))
int lua__storage_load(tt_file_t file, const char* identifier, char* data, size_t* data_size) {
	char	filename[FILENAME_MAX+1] = {0};

	if( file == TT_FILE__UNDEFINED || identifier == NULL || data == NULL || data_size == NULL || *data_size == 0) {
		g_self.library.api.runtime.system.log( TT_LOG__ERROR, "plugin/lua", "invalid parameter in %s()'", __FUNCTION__ );
		return TT_ERR;
	}
	if( lua__get_script( g_cfg, "storage-load", filename, sizeof(filename) ) != TT_OK ) {
		g_self.library.api.runtime.system.log( TT_LOG__ERROR, "plugin/lua", "no script configured for 'storage-load'" );
		return TT_ERR;
	}

	g_self.library.api.runtime.system.debug( TT_DEBUG__VERBOSITY2, "plugin/lua", "invoking script '%s' for 'storage-load' with type='%s' and identifier='%s'", filename, g_type[file], identifier );
	lua_pushstring( g_LUA, g_type[file] );
	lua_setglobal( g_LUA, "type" );
	lua_pushstring( g_LUA, identifier );
	lua_setglobal( g_LUA, "identifier" );
	if( luaL_dofile( g_LUA , filename ) != 0 ) {
		g_self.library.api.runtime.system.log( TT_LOG__ERROR, "plugin/lua", "error in script '%s' for 'storage-load'", filename );
		return TT_ERR;
	}
	lua_getglobal( g_LUA, "data" );
	if( lua_isstring( g_LUA, -1 ) == 0 ) {
		g_self.library.api.runtime.system.log( TT_LOG__ERROR, "plugin/lua", "script returned no data in global variable 'data'" );
		lua_pop( g_LUA, 1 );
		return TT_ERR;
	}
	if( lua_rawlen( g_LUA, -1 ) > *data_size ) {
		g_self.library.api.runtime.system.log(TT_LOG__ERROR, "plugin/lua", "data_size too small (%zd bytes) for data returned by script (%d bytes)", *data_size, lua_rawlen(g_LUA, -1) );
		lua_pop( g_LUA, 1 );
		return TT_ERR;
	}
	*data_size = lua_rawlen( g_LUA, -1 );
	memcpy( data, lua_tostring( g_LUA, -1 ), *data_size );
	lua_pop( g_LUA, 1 );
	g_self.library.api.runtime.system.debug( TT_DEBUG__VERBOSITY4, "plugin/lua", "script '%s' loaded %zd bytes for 'storage-load'", filename, *data_size );
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int lua__storage_save(tt_file_t file, const char* identifier, const char* data, size_t data_size) {
	char	filename[FILENAME_MAX+1] = {0};

	if( file == TT_FILE__UNDEFINED || identifier == NULL || data == NULL || data_size == 0 ) {
		g_self.library.api.runtime.system.log( TT_LOG__ERROR, "plugin/lua", "invalid parameter in %s()'", __FUNCTION__ );
		return TT_ERR;
	}
	if( lua__get_script( g_cfg, "storage-save", filename, sizeof(filename) ) != TT_OK ) {
		g_self.library.api.runtime.system.log( TT_LOG__ERROR, "plugin/lua", "no script configured for 'storage-save'" );
		return TT_ERR;
	}

	g_self.library.api.runtime.system.debug( TT_DEBUG__VERBOSITY2, "plugin/lua", "invoking script '%s' for 'storage-save' with type='%s' and identifier='%s'", filename, g_type[file], identifier );
	lua_pushstring( g_LUA, g_type[file] );
	lua_setglobal( g_LUA, "type" );
	lua_pushstring( g_LUA, identifier );
	lua_setglobal( g_LUA, "identifier" );
	lua_pushlstring( g_LUA, data, data_size );
	lua_setglobal( g_LUA, "data" );
	if( luaL_dofile( g_LUA , filename ) != 0 ) {
		g_self.library.api.runtime.system.log( TT_LOG__ERROR, "plugin/lua", "error in script '%s' for 'storage-save'", filename );
		return TT_ERR;
	}
	g_self.library.api.runtime.system.debug( TT_DEBUG__VERBOSITY4, "plugin/lua", "script '%s' saved %zd bytes for 'storage-save'", filename, data_size );
	return TT_OK;
}

