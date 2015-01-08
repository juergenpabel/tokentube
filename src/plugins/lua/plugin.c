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


static int initialize();
static int configure(const char* filename);
static int finalize();

int lua__get_script(cfg_t* cfg, const char* hook, char* out, size_t out_len);


static lua_State*	g_LUA = NULL;
static cfg_t*		g_cfg = NULL;

__attribute__ ((visibility ("hidden")))
tt_plugin_t	g_self;
TT_PLUGIN_REGISTER( g_self, initialize, configure, finalize )


static cfg_opt_t opt_exec_hooks_hook[] = {
	CFG_STR("script",  NULL, CFGF_NODEFAULT),
	CFG_END()
};

static cfg_opt_t opt_lua_hooks[] = {
	CFG_SEC("hook", opt_exec_hooks_hook, CFGF_MULTI|CFGF_TITLE),
	CFG_END()
};

static cfg_opt_t opt_lua[] = {
	CFG_SEC("hooks", opt_lua_hooks, CFGF_NONE),
	CFG_END()
};


static int lua__luks_load(char* key, size_t* key_len) {
	char	filename[1024] = {0};
	int	result = TT_ERR;

	if( key == NULL || key_len == NULL || *key_len == 0) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/lua", "invalid parameter in lua__luks_load()'" );
		return TT_ERR;
	}
	if( lua__get_script( g_cfg, "luks-load", filename, sizeof(filename) ) != TT_OK ) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/lua", "no script configured for 'luks_load'" );
		return TT_ERR;
	}

	g_self.library.api.runtime.debug( TT_DEBUG__VERBOSITY2, "plugin/lua", "invoking script '%s' for 'luks_load'", filename );
	if( luaL_dofile( g_LUA , filename ) == 0 ) {
		lua_getglobal( g_LUA, "result" );
		if( lua_isstring( g_LUA, -1 ) ) {
			if( lua_rawlen( g_LUA, -1 ) <= *key_len ) {
				*key_len = lua_rawlen( g_LUA, -1 );
				memcpy( key, lua_tostring( g_LUA, -1 ), *key_len );
				result = TT_OK;
			} else {
				g_self.library.api.runtime.log(TT_LOG__ERROR, "plugin/lua", "key_len too small (%zd bytes) for key returned by script (%d bytes)", *key_len, lua_rawlen(g_LUA, -1) );
			}
		} else {
			g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/lua", "script returned no data in global variable 'result'" );
		}
		lua_pop( g_LUA, 1 );
	} else {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/lua", "error in script '%s' for 'luks_load'", filename );
	}
	return result;
}


static int initialize() {
	g_cfg = cfg_init( opt_lua, CFGF_NONE );
	if( g_cfg == NULL ) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/lua", "cfg_init() returned NULL" );
		return TT_ERR;
	}
	g_LUA = luaL_newstate();
	if( g_LUA == NULL ) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/lua", "lua_open() returned NULL" );
		return TT_ERR;
	}
	luaL_openlibs( g_LUA );

	return TT_OK;
}


static int configure(const char* filename) {
	if( cfg_parse( g_cfg, filename ) != CFG_SUCCESS ) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/lua", "error loading configuration file '%s'", filename );
		return TT_ERR;
	}
	g_self.interface.api.storage.luks_load = lua__luks_load;
	return TT_OK;
}


static int finalize() {
	if( g_LUA != NULL ) {
		lua_close( g_LUA );
		g_LUA = NULL;
	}
	if( g_cfg != NULL ) {
		cfg_free( g_cfg );
		g_cfg = NULL;
	}
	return TT_OK;
}

