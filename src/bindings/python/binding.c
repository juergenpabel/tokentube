#include <stddef.h>
#include <tokentube.h>
#include <Python.h>
#include "binding.h"


static void PyExit_tokentube(void);


__attribute__ ((visibility ("hidden")))
tt_library_t*	g_library = NULL;


static PyObject* py_tt_initialize(PyObject* self __attribute__((unused)), PyObject *args __attribute__((unused))) {
	memset( g_library, '\0', sizeof(tt_library_t) );
	if( tt_initialize( TT_VERSION ) == TT_ERR ) {
		return NULL;
	}
	Py_RETURN_NONE;
}


static PyObject* py_tt_configure(PyObject* self __attribute__((unused)), PyObject *args) {
	char*	filename = NULL;

	if( !PyArg_ParseTuple( args, "|s", &filename ) ) {
		PyErr_SetString(PyExc_TypeError, "PyArg_ParseTuple failed" );
		return NULL;
	}
	if( tt_configure( filename ) == TT_ERR ) {
		PyErr_SetString(PyExc_TypeError, "configuration failed" );
		return NULL;
	}
	if( tt_discover( g_library ) == TT_ERR ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube.discovery failed" );
		return NULL;
	}
	if( g_library->version.major != TT_VERSION_MAJOR ) {
		memset( g_library, '\0', sizeof(tt_library_t) );
		PyErr_SetString(PyExc_TypeError, "incompatible version of libtokentube" );
		return NULL;
	}
	Py_RETURN_NONE;
}


static PyObject* py_tt_finalize(PyObject* self __attribute__((unused)), PyObject *args __attribute__((unused))) {
	if( tt_finalize() == TT_ERR ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube.finalize failed" );
		return NULL;
	}
	Py_RETURN_NONE;
}


static PyMethodDef g_tokentube_funcs[] = {
	{ "initialize", (PyCFunction)py_tt_initialize, METH_VARARGS, "initialize function" },
	{ "configure",  (PyCFunction)py_tt_configure,  METH_VARARGS, "configuration function" },
	{ "finalize",   (PyCFunction)py_tt_finalize,   METH_VARARGS, "finalize function" },
	{ "user_create", (PyCFunction)py_tt_user_create, METH_VARARGS, "user create function" },
	{ "user_chpass", (PyCFunction)py_tt_user_chpass, METH_VARARGS, "user chpass function" },
	{ "user_delete", (PyCFunction)py_tt_user_delete, METH_VARARGS, "user delete function" },
	{ "user_exists", (PyCFunction)py_tt_user_exists, METH_VARARGS, "user exists function" },
	{ "user_key_add", (PyCFunction)py_tt_user_key_add, METH_VARARGS, "user add-key function" },
	{ "user_key_del", (PyCFunction)py_tt_user_key_del, METH_VARARGS, "user del-key function" },
	{ "otp_create", (PyCFunction)py_tt_otp_create, METH_VARARGS, "otp create function" },
	{ "otp_delete", (PyCFunction)py_tt_otp_delete, METH_VARARGS, "otp delete function" },
	{ "otp_exists", (PyCFunction)py_tt_otp_exists, METH_VARARGS, "otp exists function" },
	{ "auth_user_autoenrollment", (PyCFunction)py_tt_auth_user_autoenrollment, METH_VARARGS, "user autoenrollment function" },
	{ "auth_user_verify", (PyCFunction)py_tt_auth_user_verify, METH_VARARGS, "user verify function" },
	{ "auth_user_loadkey", (PyCFunction)py_tt_auth_user_loadkey, METH_VARARGS, "user load-key function" },
	{ "auth_otp_challenge", (PyCFunction)py_tt_auth_otp_challenge, METH_VARARGS, "otp challenge function" },
	{ "auth_otp_response", (PyCFunction)py_tt_auth_otp_response, METH_VARARGS, "otp response function" },
	{ "auth_otp_loadkey", (PyCFunction)py_tt_auth_otp_loadkey, METH_VARARGS, "otp load-key function" },
	{ "runtime_install_pba", (PyCFunction)py_tt_runtime_install_pba, METH_VARARGS, "pba install function" },
	{ NULL }
};


static struct PyModuleDef g_tokentube_def = {
	PyModuleDef_HEAD_INIT, "tokentube", "libtokentube extension", -1, g_tokentube_funcs, NULL, NULL, NULL, NULL
};


PyMODINIT_FUNC PyInit_tokentube(void) {
	PyObject* module = NULL;

	g_library = malloc( sizeof(tt_library_t) );
	if( g_library == NULL ) {
		return NULL;
	}
	Py_AtExit( PyExit_tokentube );
	module = PyModule_Create( &g_tokentube_def );
	if( module == NULL ) {
		return NULL;
	}
	PyModule_AddIntConstant( module, "TT_KEY_BITS_MAX", TT_KEY_BITS_MAX );
	PyModule_AddIntConstant( module, "TT_OTP_TEXT_MAX", TT_OTP_TEXT_MAX );
	return module;
}


static void PyExit_tokentube(void) {
	if( g_library != NULL ) {
		free( g_library );
		g_library = NULL;
	}
	if( tt_finalize() == TT_ERR ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube.finalize failed" );
	}
}

