#include <stddef.h>
#include <tokentube.h>
#include <Python.h>


static tt_library_t*	g_library = NULL;


static PyObject* py_tt_initialize(PyObject* self) {
	if( g_library != NULL ) {
		PyErr_SetString(PyExc_TypeError, "already initialized" );
		return NULL;
	}
	g_library = malloc( sizeof(tt_library_t) );
	if( g_library == NULL ) {
		PyErr_SetString(PyExc_TypeError, "malloc failed" );
		return NULL;
	}
	memset( g_library, '\0', sizeof(tt_library_t) );
	if( tt_initialize( TT_VERSION ) != TT_OK ) {
		PyErr_SetString(PyExc_TypeError, "tt_initialize failed" );
		free( g_library );
		g_library = NULL;
		return NULL;
	}
	Py_RETURN_NONE;
}


static PyObject* py_tt_configure(PyObject* self, PyObject *args) {
	char*	filename = NULL;

	if( g_library == NULL ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube uninitialized, call tokentube.initialize() first" );
		return NULL;
	}
	if( !PyArg_ParseTuple( args, "|s", &filename ) ) {
		PyErr_SetString(PyExc_TypeError, "PyArg_ParseTuple failed" );
		return NULL;
	}
	if( tt_configure( filename ) != TT_OK ) {
		PyErr_SetString(PyExc_TypeError, "configuration failed" );
		free( g_library );
		g_library = NULL;
		return NULL;
	}
	if( tt_discover( g_library ) != TT_OK ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube.discovery failed" );
		free( g_library );
		g_library = NULL;
		return NULL;
	}
g_library->version = TT_VERSION; //TODO:delete
	Py_RETURN_NONE;
}


static PyObject* py_tt_finalize(PyObject* self) {
	if( g_library != NULL ) {
		free( g_library );
		g_library = NULL;
	}
	if( tt_finalize() != TT_OK ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube.finalize failed" );
		return NULL;
	}
	Py_RETURN_NONE;
}


static PyObject* py_tt_user_exists(PyObject* self, PyObject *args) {
	char*		username = NULL;
	tt_status_t	status = TT_STATUS__UNDEFINED;

	if( g_library == NULL || g_library->version.major != TT_VERSION_MAJOR ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube uninitialized, call tokentube.initialize() first" );
		return NULL;
	}
	if( !PyArg_ParseTuple( args, "s", &username ) ) {
		PyErr_SetString(PyExc_TypeError, "PyArg_ParseTuple failed" );
		return NULL;
	}
	if( g_library->api.user.exists( username, &status ) != TT_OK ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube.api.user.exists failed" );
		return NULL;
	}
	switch( status ) {
		case TT_STATUS__YES:
			Py_RETURN_TRUE;
			break;
		case TT_STATUS__NO:
			Py_RETURN_FALSE;
			break;
		default:
			return NULL;
	}
	return NULL;
}


static PyMethodDef g_tokentube_funcs[] = {
	{ "initialize", (PyCFunction)py_tt_initialize, METH_NOARGS, "initalization function" },
	{ "configure",  (PyCFunction)py_tt_configure, METH_VARARGS, "configuration function" },
	{ "finalize",   (PyCFunction)py_tt_finalize, METH_NOARGS, "finalizing function" },
	{ "user_exists", (PyCFunction)py_tt_user_exists, METH_VARARGS, "finalizing function" },
	{ NULL }
};


static struct PyModuleDef g_tokentube_def = {
	PyModuleDef_HEAD_INIT, "tokentube", "libtokentube extension", -1, g_tokentube_funcs, NULL, NULL, NULL, NULL
};


PyMODINIT_FUNC PyInit_tokentube(void) {
	return PyModule_Create( &g_tokentube_def );
}

