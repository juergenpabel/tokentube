#include <string.h>
#include <confuse.h>
#include <syslog.h>
#include <gcrypt.h>
#include <tokentube/plugin.h>
#include <Python.h>
#include "plugin.h"

extern tt_plugin_t	g_self;


__attribute__ ((visibility ("hidden")))
int python__exec_script(cfg_t* cfg, const char* hook, PyObject** result) {
	cfg_t*          section = NULL;
	char*           script = NULL;
	FILE*		fp = NULL;
	PyThreadState*	state = NULL;
	PyObject*	dict = NULL;

        section = cfg_getsec( cfg, "hooks" );
        if( section == NULL ) {
                g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/python", "configuration error (no hooks configured)" );
                return TT_ERR;
        }
        section = cfg_gettsec( section, "hook", hook );
        if( section == NULL ) {
                g_self.library.api.runtime.log( TT_LOG__WARN, "plugin/python", "no configuration provided for hook '%s'", hook );
                return TT_ERR;
        }
        script = cfg_getstr( section, "script" );
        if( script == NULL ) {
                g_self.library.api.runtime.log( TT_LOG__WARN, "plugin/python", "no script specified for hook '%s'", hook );
                return TT_ERR;
        }
	fp = fopen( script, "rb" );
	if( fp == NULL ) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/python", "file error in python__exec_script()'" );
		return TT_ERR;
	}
	
	state = PyThreadState_Swap( g_python );
	dict = PyModule_GetDict( PyImport_ImportModule("__main__") );
	PyRun_SimpleFile( fp, script );
	*result = PyDict_GetItemString( dict, "key" );
	PyThreadState_Swap( state );
	fclose( fp );
	return TT_OK;
}

