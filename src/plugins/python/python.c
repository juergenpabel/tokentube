#include <string.h>
#include <confuse.h>
#include <syslog.h>
#include <gcrypt.h>
#include <tokentube/plugin.h>
#include <Python.h>
#include "plugin.h"

extern tt_plugin_t	g_self;


__attribute__ ((visibility ("hidden")))
int python__exec_script(cfg_t* cfg, const char* action, const char* type, const char* identifier, PyObject** in, PyObject** out) {
	cfg_t*          section = NULL;
	char*           script = NULL;
	size_t          size = 0;
	wchar_t*	argv[3] = { NULL, NULL, NULL };
	FILE*		fp = NULL;
	PyThreadState*	state = NULL;
	PyObject*	dict = NULL;

	g_self.library.api.runtime.debug( TT_DEBUG__TRACE, "plugin/python", "%s(cfg=%p,action=%s,type=%s,identifier=%p,in=%p,out=%p)", __FUNCTION__, cfg, action, type, identifier, in, out );
	section = cfg_gettsec( cfg, "hooks|hook", action );
	if( section == NULL ) {
		g_self.library.api.runtime.log( TT_LOG__WARN, "plugin/python", "no configuration provided for 'hooks|hook %s'", action );
		return TT_ERR;
	}
        script = cfg_getstr( section, "script" );
        if( script == NULL ) {
                g_self.library.api.runtime.log( TT_LOG__WARN, "plugin/python", "no script specified for section 'hooks|hook %s'", action );
                return TT_ERR;
        }
	fp = fopen( script, "rb" );
	if( fp == NULL ) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/python", "file error in %s()'", __FUNCTION__ );
		return TT_ERR;
	}
	g_self.library.api.runtime.debug( TT_DEBUG__VERBOSITY3, "plugin/python", "successfully opened script file '%s' in %s()", script, __FUNCTION__ );
	
	state = PyThreadState_Swap( g_python );
	size = strlen( script );
	argv[0] = _Py_char2wchar( script, &size );
	size = strlen( type );
	argv[1] = _Py_char2wchar( type, &size );
	size = strlen( identifier );
	argv[2] = _Py_char2wchar( identifier, &size );
	PySys_SetArgv( 3, argv );
	dict = PyModule_GetDict( PyImport_ImportModule("__main__") );
	if( in != NULL ) {
		PyDict_SetItemString( dict, "data", *in );
	}
	g_self.library.api.runtime.debug( TT_DEBUG__VERBOSITY5, "plugin/python", "executing script file '%s' in %s()...", script, __FUNCTION__ );
	PyRun_SimpleFile( fp, script );
	fclose( fp );
	g_self.library.api.runtime.debug( TT_DEBUG__VERBOSITY5, "plugin/python", "...executed script file '%s' in %s()", script, __FUNCTION__ );
	if( PyErr_Occurred() != NULL ) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/python", "script '%s' returned error/exception'", script );
		PyThreadState_Swap( state );
		return TT_ERR;
	}
	if( out != NULL ) {
		*out = PyDict_GetItemString( dict, "data" );
	}
	PyThreadState_Swap( state );
	return TT_OK;
}

