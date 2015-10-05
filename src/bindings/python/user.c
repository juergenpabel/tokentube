#include <stddef.h>
#include <tokentube.h>
#include <Python.h>
#include "binding.h"


__attribute__ ((visibility ("hidden")))
PyObject* py_tt_user_create(PyObject* self __attribute__((unused)), PyObject *args) {
	char*		py_username = NULL;
	char*		py_password = NULL;

	if( g_library == NULL || g_library->version.major == 0 ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube uninitialized, call tokentube.initialize() first" );
		return NULL;
	}
	if( !PyArg_ParseTuple( args, "ss", &py_username, &py_password ) ) {
		PyErr_SetString(PyExc_TypeError, "PyArg_ParseTuple failed" );
		return NULL;
	}
	if( g_library->api.database.user.create( py_username, py_password ) != TT_OK ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube.api.database.user.create failed" );
		return NULL;
	}
	Py_RETURN_TRUE;
}


__attribute__ ((visibility ("hidden")))
PyObject* py_tt_user_update(PyObject* self __attribute__((unused)), PyObject *args) {
	char*		py_username = NULL;
	char*		py_password_cur = NULL;
	char*		py_password_new = NULL;
	tt_status_t	status = TT_STATUS__UNDEFINED;

	if( g_library == NULL || g_library->version.major == 0 ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube uninitialized, call tokentube.initialize() first" );
		return NULL;
	}
	if( !PyArg_ParseTuple( args, "sss", &py_username, &py_password_cur, &py_password_new ) ) {
		PyErr_SetString(PyExc_TypeError, "PyArg_ParseTuple failed" );
		return NULL;
	}
	if( g_library->api.database.user.update( py_username, py_password_cur, py_password_new, &status ) != TT_OK ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube.api.database.user.update failed" );
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
			PyErr_SetString(PyExc_TypeError, "libtokentube.api.database.user.update returned unknown status" );
			return NULL;
	}
	PyErr_SetString(PyExc_TypeError, "py_tt_user_update: internal error" );
	return NULL;
}


__attribute__ ((visibility ("hidden")))
PyObject* py_tt_user_exists(PyObject* self __attribute__((unused)), PyObject *args) {
	char*		py_username = NULL;
	tt_status_t	status = TT_STATUS__UNDEFINED;

	if( g_library == NULL || g_library->version.major == 0 ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube uninitialized, call tokentube.initialize() first" );
		return NULL;
	}
	if( !PyArg_ParseTuple( args, "s", &py_username ) ) {
		PyErr_SetString(PyExc_TypeError, "PyArg_ParseTuple failed" );
		return NULL;
	}
	if( g_library->api.database.user.exists( py_username, &status ) != TT_OK ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube.api.database.user.exists failed" );
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
			PyErr_SetString(PyExc_TypeError, "libtokentube.api.database.user.exists returned unknown status" );
			return NULL;
	}
	PyErr_SetString(PyExc_TypeError, "py_tt_user_exists: internal error" );
	return NULL;
}


__attribute__ ((visibility ("hidden")))
PyObject* py_tt_user_delete(PyObject* self __attribute__((unused)), PyObject *args) {
	char*		py_username = NULL;
	tt_status_t	status = TT_STATUS__UNDEFINED;

	if( g_library == NULL || g_library->version.major == 0 ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube uninitialized, call tokentube.initialize() first" );
		return NULL;
	}
	if( !PyArg_ParseTuple( args, "s", &py_username ) ) {
		PyErr_SetString(PyExc_TypeError, "PyArg_ParseTuple failed" );
		return NULL;
	}
	if( g_library->api.database.user.delete( py_username, &status ) != TT_OK ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube.api.database.user.delete failed" );
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
			PyErr_SetString(PyExc_TypeError, "libtokentube.api.database.user.delete returned unknown status" );
			return NULL;
	}
	PyErr_SetString(PyExc_TypeError, "py_tt_user_delete: internal error" );
	return NULL;
}


__attribute__ ((visibility ("hidden")))
PyObject* py_tt_user_key_add(PyObject* self __attribute__((unused)), PyObject *args) {
	tt_status_t	status = TT_STATUS__UNDEFINED;
	char*		py_username = NULL;
	char*		py_password = NULL;
	char*		py_identifier = NULL;

	if( g_library == NULL || g_library->version.major == 0 ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube uninitialized, call tokentube.initialize() first" );
		return NULL;
	}
	if( !PyArg_ParseTuple( args, "sss", &py_username, &py_password, &py_identifier ) ) {
		PyErr_SetString(PyExc_TypeError, "PyArg_ParseTuple failed" );
		return NULL;
	}
	if( g_library->api.database.user.key_add( py_username, py_password, py_identifier, &status ) != TT_OK ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube.api.database.user.key_add failed" );
		return NULL;
	}
	if( status != TT_STATUS__YES ) {
		Py_RETURN_FALSE;
	}
	Py_RETURN_TRUE;
}


__attribute__ ((visibility ("hidden")))
PyObject* py_tt_user_key_del(PyObject* self __attribute__((unused)), PyObject *args) {
	tt_status_t	status = TT_STATUS__UNDEFINED;
	char*		py_username = NULL;
	char*		py_password = NULL;
	char*		py_identifier = NULL;

	if( g_library == NULL || g_library->version.major == 0 ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube uninitialized, call tokentube.initialize() first" );
		return NULL;
	}
	if( !PyArg_ParseTuple( args, "sss", &py_username, &py_password, &py_identifier ) ) {
		PyErr_SetString(PyExc_TypeError, "PyArg_ParseTuple failed" );
		return NULL;
	}
	if( g_library->api.database.user.key_del( py_username, py_password, py_identifier, &status ) != TT_OK ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube.api.database.user.key_del failed" );
		return NULL;
	}
	if( status != TT_STATUS__YES ) {
		Py_RETURN_FALSE;
	}
	Py_RETURN_TRUE;
}


__attribute__ ((visibility ("hidden")))
PyObject* py_tt_auth_user_verify(PyObject* self __attribute__((unused)), PyObject *args) {
	char*		py_username = NULL;
	char*		py_password = NULL;
	tt_status_t	status = TT_STATUS__UNDEFINED;

	if( g_library == NULL || g_library->version.major == 0 ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube uninitialized, call tokentube.initialize() first" );
		return NULL;
	}
	if( !PyArg_ParseTuple( args, "ss", &py_username, &py_password ) ) {
		PyErr_SetString(PyExc_TypeError, "PyArg_ParseTuple failed" );
		return NULL;
	}
	if( g_library->api.auth.user.verify( py_username, py_password, &status ) != TT_OK ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube.api.auth.user.verify failed" );
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
			PyErr_SetString(PyExc_TypeError, "libtokentube.api.auth.user.verify returned unknown status" );
			return NULL;
	}
	PyErr_SetString(PyExc_TypeError, "py_tt_auth_user_verify: internal error" );
	return NULL;
}


__attribute__ ((visibility ("hidden")))
PyObject* py_tt_auth_user_loadkey(PyObject* self __attribute__((unused)), PyObject *args) {
	char*		py_username = NULL;
	char*		py_password = NULL;
	char*		py_identifier = NULL;
	char*		py_key = NULL;
	ssize_t		py_key_size = 0;
	char		data[TT_KEY_BITS_MAX/8] = {0};
	size_t		data_size = sizeof(data);

	if( g_library == NULL || g_library->version.major == 0 ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube uninitialized, call tokentube.initialize() first" );
		return NULL;
	}
	if( !PyArg_ParseTuple( args, "ssss#", &py_username, &py_password, &py_identifier, &py_key, &py_key_size ) ) {
		PyErr_SetString(PyExc_TypeError, "PyArg_ParseTuple failed" );
		return NULL;
	}
	if( g_library->api.auth.user.loadkey( py_username, py_password, py_identifier, data, &data_size ) != TT_OK ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube.api.auth.user.loadkey failed" );
		return NULL;
	}
	if( data_size == 0 || data_size > (size_t)py_key_size ) {
		Py_RETURN_FALSE;
	}
	if( memcpy( py_key, data, data_size ) == NULL ) {
		Py_RETURN_FALSE;
	}
	Py_RETURN_TRUE;
}


__attribute__ ((visibility ("hidden")))
PyObject* py_tt_auth_user_autoenrollment(PyObject* self __attribute__((unused)), PyObject *args) {
	char*		py_username = NULL;
	char*		py_password = NULL;
	tt_status_t	status = TT_STATUS__UNDEFINED;

	if( g_library == NULL || g_library->version.major == 0 ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube uninitialized, call tokentube.initialize() first" );
		return NULL;
	}
	if( !PyArg_ParseTuple( args, "ss", &py_username, &py_password ) ) {
		PyErr_SetString(PyExc_TypeError, "PyArg_ParseTuple failed" );
		return NULL;
	}
	if( g_library->api.auth.user.autoenrollment( py_username, py_password, &status ) != TT_OK ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube.api.auth.user.autoenrollment failed" );
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
			PyErr_SetString(PyExc_TypeError, "libtokentube.api.auth.user.autoenrollment returned unknown status" );
			return NULL;
	}
	PyErr_SetString(PyExc_TypeError, "py_tt_auth_user_autoenrollment: internal error" );
	return NULL;
}

