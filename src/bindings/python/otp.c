#include <stddef.h>
#include <tokentube.h>
#include <Python.h>
#include "binding.h"


__attribute__ ((visibility ("hidden")))
PyObject* py_tt_otp_create(PyObject* self __attribute__((unused)), PyObject *args) {
	tt_status_t  status = TT_STATUS__UNDEFINED;
	char*        py_identifier = NULL;

	if( g_library == NULL || g_library->version.major == 0 ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube uninitialized, call tokentube.initialize() first" );
		return NULL;
	}
	if( !PyArg_ParseTuple( args, "s", &py_identifier ) ) {
		PyErr_SetString(PyExc_TypeError, "PyArg_ParseTuple failed" );
		return NULL;
	}
	if( g_library->api.database.otp.create( py_identifier, &status ) != TT_OK ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube.api.database.otp.create failed" );
		return NULL;
	}
	Py_RETURN_TRUE;
}


__attribute__ ((visibility ("hidden")))
PyObject* py_tt_otp_exists(PyObject* self __attribute__((unused)), PyObject *args) {
	char*		py_identifier = NULL;
	tt_status_t	status = TT_STATUS__UNDEFINED;

	if( g_library == NULL || g_library->version.major == 0 ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube uninitialized, call tokentube.initialize() first" );
		return NULL;
	}
	if( !PyArg_ParseTuple( args, "s", &py_identifier ) ) {
		PyErr_SetString(PyExc_TypeError, "PyArg_ParseTuple failed" );
		return NULL;
	}
	if( g_library->api.database.otp.exists( py_identifier, &status ) != TT_OK ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube.api.database.otp.exists failed" );
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
			PyErr_SetString(PyExc_TypeError, "libtokentube.api.database.otp.exists returned unknown status" );
			return NULL;
	}
	PyErr_SetString(PyExc_TypeError, "py_tt_otp_exists: internal error" );
	return NULL;
}


__attribute__ ((visibility ("hidden")))
PyObject* py_tt_otp_delete(PyObject* self __attribute__((unused)), PyObject *args) {
	char*		py_identifier = NULL;
	tt_status_t	status = TT_STATUS__UNDEFINED;

	if( g_library == NULL || g_library->version.major == 0 ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube uninitialized, call tokentube.initialize() first" );
		return NULL;
	}
	if( !PyArg_ParseTuple( args, "s", &py_identifier ) ) {
		PyErr_SetString(PyExc_TypeError, "PyArg_ParseTuple failed" );
		return NULL;
	}
	if( g_library->api.database.otp.delete( py_identifier, &status ) != TT_OK ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube.api.database.otp.delete failed" );
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
			PyErr_SetString(PyExc_TypeError, "libtokentube.api.database.otp.delete returned unknown status" );
			return NULL;
	}
	PyErr_SetString(PyExc_TypeError, "py_tt_otp_delete: internal error" );
	return NULL;
}

