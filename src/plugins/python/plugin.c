#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <string.h>
#include <confuse.h>
#include <Python.h>
#include <tokentube/plugin.h>
#include "plugin.h"


static int initialize();
static int configure(const char* filename);
static int finalize();


__attribute__ ((visibility ("hidden")))
cfg_t*           g_cfg = NULL;
__attribute__ ((visibility ("hidden")))
PyThreadState*	g_python = NULL;

__attribute__ ((visibility ("hidden")))
tt_plugin_t	g_self;
TT_PLUGIN_REGISTER( g_self, initialize, configure, finalize )


static cfg_opt_t opt_python_hooks_hook[] = {
	CFG_STR("script",  NULL, CFGF_NODEFAULT),
	CFG_END()
};

static cfg_opt_t opt_python_events_event[] = {
	CFG_STR("script",  NULL, CFGF_NODEFAULT),
	CFG_END()
};

static cfg_opt_t opt_python_events[] = {
	CFG_SEC("event", opt_python_events_event, CFGF_MULTI|CFGF_TITLE),
	CFG_END()
};

static cfg_opt_t opt_python_hooks[] = {
	CFG_SEC("hook", opt_python_hooks_hook, CFGF_MULTI|CFGF_TITLE),
	CFG_END()
};

static cfg_opt_t opt_python[] = {
	CFG_SEC("events", opt_python_events, CFGF_NONE),
	CFG_SEC("hooks", opt_python_hooks, CFGF_NONE),
	CFG_END()
};


static int initialize() {
	if( !Py_IsInitialized() ) {
		Py_InitializeEx( 0 );
	}
	g_python = Py_NewInterpreter();
	if( g_python == NULL ) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/python", "Py_NewInterpreter() returned NULL" );
		return TT_ERR;
	}
	g_cfg = cfg_init( opt_python, CFGF_NONE );
	if( g_cfg == NULL ) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/python", "cfg_init() returned NULL" );
		return TT_ERR;
	}
	return TT_OK;
}


static int configure(const char* filename) {
	if( cfg_parse( g_cfg, filename ) != CFG_SUCCESS ) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/python", "error loading configuration file '%s'", filename );
		return TT_ERR;
	}
	g_self.interface.api.storage.load = python__storage_load;
	g_self.interface.api.storage.save = python__storage_save;
	return TT_OK;
}


static int finalize() {
	if( PyThreadState_GET() == g_python ) {
		Py_EndInterpreter( g_python );
	}
	if( g_cfg != NULL ) {
		cfg_free( g_cfg );
		g_cfg = NULL;
	}
	return TT_OK;
}

