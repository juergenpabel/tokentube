#include <stddef.h>
#include <tokentube.h>
#include <Python.h>
#include "binding.h"


__attribute__ ((visibility ("hidden")))
PyObject* py_tt_runtime_log(PyObject* self __attribute__((unused)), PyObject *args) {
	int		py_level = TT_LOG__UNDEFINED;
	char*		py_source = NULL;
	char*		py_message = NULL;

	if( g_library == NULL || g_library->version.major == 0 ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube uninitialized, call tokentube.initialize() first" );
		return NULL;
	}
	if( !PyArg_ParseTuple( args, "iss", &py_level, &py_source, &py_message ) ) {
		PyErr_SetString(PyExc_TypeError, "PyArg_ParseTuple failed" );
		return NULL;
	}
	if( g_library->api.runtime.system.log( py_level, py_source, py_message ) != TT_OK ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube:runtime.log() failed" );
		return NULL;
	}
	Py_RETURN_TRUE;
}


__attribute__ ((visibility ("hidden")))
PyObject* py_tt_runtime_debug(PyObject* self __attribute__((unused)), PyObject *args) {
	int		py_level = TT_DEBUG__UNDEFINED;
	char*		py_source = NULL;
	char*		py_message = NULL;

	if( g_library == NULL || g_library->version.major == 0 ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube uninitialized, call tokentube.initialize() first" );
		return NULL;
	}
	if( !PyArg_ParseTuple( args, "iss", &py_level, &py_source, &py_message ) ) {
		PyErr_SetString(PyExc_TypeError, "PyArg_ParseTuple failed" );
		return NULL;
	}
	if( g_library->api.runtime.system.debug( py_level, py_source, py_message ) != TT_OK ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube:runtime.debug() failed" );
		return NULL;
	}
	Py_RETURN_TRUE;
}


__attribute__ ((visibility ("hidden")))
PyObject* py_tt_runtime_install_pba(PyObject* self __attribute__((unused)), PyObject *args) {
	char*		py_type = NULL;
	char*		py_path = NULL;

	if( g_library == NULL || g_library->version.major == 0 ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube uninitialized, call tokentube.initialize() first" );
		return NULL;
	}
	if( !PyArg_ParseTuple( args, "ss", &py_type, &py_path ) ) {
		PyErr_SetString(PyExc_TypeError, "PyArg_ParseTuple failed" );
		return NULL;
	}
	if( g_library->api.runtime.system.install_pba( py_type, py_path ) != TT_OK ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube.api.pba.system.install failed" );
		return NULL;
	}
	Py_RETURN_TRUE;
}

