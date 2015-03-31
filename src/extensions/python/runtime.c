#include <stddef.h>
#include <tokentube.h>
#include <Python.h>
#include "extension.h"


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
	if( g_library->api.runtime.install_pba( py_type, py_path ) != TT_OK ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube.api.pba.install failed" );
		return NULL;
	}
	Py_RETURN_TRUE;
}

