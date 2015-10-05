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


static int initialize();
static int configure(const char* filename);
static int finalize();


__attribute__ ((visibility ("hidden")))
lua_State*	g_LUA = NULL;
__attribute__ ((visibility ("hidden")))
cfg_t*		g_cfg = NULL;

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


static int initialize() {
	g_cfg = cfg_init( opt_lua, CFGF_NONE );
	if( g_cfg == NULL ) {
		g_self.library.api.runtime.system.log( TT_LOG__ERROR, "plugin/lua", "cfg_init() returned NULL" );
		return TT_ERR;
	}
	g_LUA = luaL_newstate();
	if( g_LUA == NULL ) {
		g_self.library.api.runtime.system.log( TT_LOG__ERROR, "plugin/lua", "lua_open() returned NULL" );
		return TT_ERR;
	}
	luaL_openlibs( g_LUA );
	return TT_OK;
}


static int configure(const char* filename) {
	if( cfg_parse( g_cfg, filename ) != CFG_SUCCESS ) {
		g_self.library.api.runtime.system.log( TT_LOG__ERROR, "plugin/lua", "error loading configuration file '%s'", filename );
		return TT_ERR;
	}
	g_self.interface.api.storage.load = lua__storage_load;
	g_self.interface.api.storage.save = lua__storage_save;
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

