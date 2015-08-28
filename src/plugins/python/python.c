#include <string.h>
#include <confuse.h>
#include <syslog.h>
#include <gcrypt.h>
#include <tokentube/plugin.h>
#include <Python.h>
#include "plugin.h"

extern tt_plugin_t	g_self;


__attribute__ ((visibility ("hidden")))
int python__exec_script(cfg_t* cfg, const char* action, const char* name, const char* identifier, PyObject** result) {
	cfg_t*          section = NULL;
	char*           script = NULL;
	size_t          size = 0;
	wchar_t*	argv[2] = { NULL, NULL };
	FILE*		fp = NULL;
	PyThreadState*	state = NULL;
	PyObject*	dict = NULL;

	g_self.library.api.runtime.debug( TT_DEBUG__TRACE, "plugin/python", "%s(cfg=%p,action=%p,name=%p,identifier=%p,result=%p)", __FUNCTION__, cfg, action, name, identifier, result );
	section = cfg_gettsec( cfg, action, name );
	if( section == NULL ) {
		g_self.library.api.runtime.log( TT_LOG__WARN, "plugin/python", "no configuration provided for action '%s %s'", action, name );
		return TT_ERR;
	}
        script = cfg_getstr( section, "script" );
        if( script == NULL ) {
                g_self.library.api.runtime.log( TT_LOG__WARN, "plugin/python", "no script specified for section '%s %s'", action, name );
                return TT_ERR;
        }
	fp = fopen( script, "rb" );
	if( fp == NULL ) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/python", "file error in python__exec_script()'" );
		return TT_ERR;
	}
	g_self.library.api.runtime.debug( TT_DEBUG__VERBOSITY3, "plugin/python", "successfully opened script file '%s' in %s()", script, __FUNCTION__ );
	
	state = PyThreadState_Swap( g_python );
	size = strlen( action );
	argv[0] = _Py_char2wchar( action, &size );
	size = strlen( identifier );
	argv[1] = _Py_char2wchar( identifier, &size );
	PySys_SetArgv( 2, argv );
	dict = PyModule_GetDict( PyImport_ImportModule("__main__") );
	g_self.library.api.runtime.debug( TT_DEBUG__VERBOSITY5, "plugin/python", "executing script file '%s' in %s()...", script, __FUNCTION__ );
	PyRun_SimpleFile( fp, script );
	g_self.library.api.runtime.debug( TT_DEBUG__VERBOSITY5, "plugin/python", "...executed script file '%s' in %s()", script, __FUNCTION__ );
	*result = PyDict_GetItemString( dict, "data" );
	PyThreadState_Swap( state );
	fclose( fp );
	return TT_OK;
}

