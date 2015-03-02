#include <stddef.h>
#include <tokentube.h>
#include <Python.h>
#include "extension.h"


PyObject* py_tt_user_create(PyObject* self, PyObject *args) {
	char*		py_username = NULL;
	char*		py_password = NULL;

	if( g_library == NULL || g_library->version.major != TT_VERSION_MAJOR ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube uninitialized, call tokentube.initialize() first" );
		return NULL;
	}
	if( !PyArg_ParseTuple( args, "ss", &py_username, &py_password ) ) {
		PyErr_SetString(PyExc_TypeError, "PyArg_ParseTuple failed" );
		return NULL;
	}
	if( g_library->api.user.create( py_username, py_password ) != TT_OK ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube.api.user.create failed" );
		return NULL;
	}
	Py_RETURN_TRUE;
}


PyObject* py_tt_user_update(PyObject* self, PyObject *args) {
	char*		py_username = NULL;
	char*		py_password_cur = NULL;
	char*		py_password_new = NULL;
	tt_status_t	status = TT_STATUS__UNDEFINED;

	if( g_library == NULL || g_library->version.major != TT_VERSION_MAJOR ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube uninitialized, call tokentube.initialize() first" );
		return NULL;
	}
	if( !PyArg_ParseTuple( args, "sss", &py_username, &py_password_cur, &py_password_new ) ) {
		PyErr_SetString(PyExc_TypeError, "PyArg_ParseTuple failed" );
		return NULL;
	}
	if( g_library->api.user.update( py_username, py_password_cur, py_password_new, &status ) != TT_OK ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube.api.user.update failed" );
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
			PyErr_SetString(PyExc_TypeError, "libtokentube.api.user.update returned unknown status" );
			return NULL;
	}
	PyErr_SetString(PyExc_TypeError, "py_tt_user_update: internal error" );
	return NULL;
}


PyObject* py_tt_user_exists(PyObject* self, PyObject *args) {
	char*		py_username = NULL;
	tt_status_t	status = TT_STATUS__UNDEFINED;

	if( g_library == NULL || g_library->version.major != TT_VERSION_MAJOR ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube uninitialized, call tokentube.initialize() first" );
		return NULL;
	}
	if( !PyArg_ParseTuple( args, "s", &py_username ) ) {
		PyErr_SetString(PyExc_TypeError, "PyArg_ParseTuple failed" );
		return NULL;
	}
	if( g_library->api.user.exists( py_username, &status ) != TT_OK ) {
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
			PyErr_SetString(PyExc_TypeError, "libtokentube.api.user.exists returned unknown status" );
			return NULL;
	}
	PyErr_SetString(PyExc_TypeError, "py_tt_user_exists: internal error" );
	return NULL;
}


PyObject* py_tt_user_delete(PyObject* self, PyObject *args) {
	char*		py_username = NULL;
	tt_status_t	status = TT_STATUS__UNDEFINED;

	if( g_library == NULL || g_library->version.major != TT_VERSION_MAJOR ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube uninitialized, call tokentube.initialize() first" );
		return NULL;
	}
	if( !PyArg_ParseTuple( args, "s", &py_username ) ) {
		PyErr_SetString(PyExc_TypeError, "PyArg_ParseTuple failed" );
		return NULL;
	}
	if( g_library->api.user.delete( py_username, &status ) != TT_OK ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube.api.user.delete failed" );
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
			PyErr_SetString(PyExc_TypeError, "libtokentube.api.user.delete returned unknown status" );
			return NULL;
	}
	PyErr_SetString(PyExc_TypeError, "py_tt_user_delete: internal error" );
	return NULL;
}


PyObject* py_tt_user_execute_verify(PyObject* self, PyObject *args) {
	char*		py_username = NULL;
	char*		py_password = NULL;
	tt_status_t	status = TT_STATUS__UNDEFINED;

	if( g_library == NULL || g_library->version.major != TT_VERSION_MAJOR ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube uninitialized, call tokentube.initialize() first" );
		return NULL;
	}
	if( !PyArg_ParseTuple( args, "ss", &py_username, &py_password ) ) {
		PyErr_SetString(PyExc_TypeError, "PyArg_ParseTuple failed" );
		return NULL;
	}
	if( g_library->api.user.execute_verify( py_username, py_password, &status ) != TT_OK ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube.api.user.execute_verify failed" );
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
			PyErr_SetString(PyExc_TypeError, "libtokentube.api.user.execute_verify returned unknown status" );
			return NULL;
	}
	PyErr_SetString(PyExc_TypeError, "py_tt_user_execute_verify: internal error" );
	return NULL;
}


PyObject* py_tt_user_execute_load(PyObject* self, PyObject *args) {
	char*		py_username = NULL;
	char*		py_password = NULL;
	char*		py_key = NULL;
	char		data[TT_KEY_BITS_MAX/8] = {0};
	size_t		data_size = sizeof(data);

	if( g_library == NULL || g_library->version.major != TT_VERSION_MAJOR ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube uninitialized, call tokentube.initialize() first" );
		return NULL;
	}
	if( !PyArg_ParseTuple( args, "sss", &py_username, &py_password, &py_key ) ) {
		PyErr_SetString(PyExc_TypeError, "PyArg_ParseTuple failed" );
		return NULL;
	}
	if( g_library->api.user.execute_load( py_username, py_password, data, &data_size ) != TT_OK ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube.api.user.execute_load failed" );
		return NULL;
	}
	if( data_size == 0 ) {
		Py_RETURN_FALSE;
	}
//TODO:put challenge into python parameter
	Py_RETURN_TRUE;
}


PyObject* py_tt_user_execute_autoenrollment(PyObject* self, PyObject *args) {
	char*		py_username = NULL;
	char*		py_password = NULL;
	tt_status_t	status = TT_STATUS__UNDEFINED;

	if( g_library == NULL || g_library->version.major != TT_VERSION_MAJOR ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube uninitialized, call tokentube.initialize() first" );
		return NULL;
	}
	if( !PyArg_ParseTuple( args, "ss", &py_username, &py_password ) ) {
		PyErr_SetString(PyExc_TypeError, "PyArg_ParseTuple failed" );
		return NULL;
	}
	if( g_library->api.user.execute_autoenrollment( py_username, py_password, &status ) != TT_OK ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube.api.user.execute_autoenrollment failed" );
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
			PyErr_SetString(PyExc_TypeError, "libtokentube.api.user.execute_autoenrollment returned unknown status" );
			return NULL;
	}
	PyErr_SetString(PyExc_TypeError, "py_tt_user_execute_autoenrollment: internal error" );
	return NULL;
}

