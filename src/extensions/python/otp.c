#include <stddef.h>
#include <tokentube.h>
#include <Python.h>
#include "extension.h"


PyObject* py_tt_otp_create(PyObject* self, PyObject *args) {
	char*		py_identifier = NULL;

	if( g_library == NULL || g_library->version.major != TT_VERSION_MAJOR ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube uninitialized, call tokentube.initialize() first" );
		return NULL;
	}
	if( !PyArg_ParseTuple( args, "s", &py_identifier ) ) {
		PyErr_SetString(PyExc_TypeError, "PyArg_ParseTuple failed" );
		return NULL;
	}
	if( g_library->api.otp.create( py_identifier ) != TT_OK ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube.api.user.create failed" );
		return NULL;
	}
	Py_RETURN_TRUE;
}


PyObject* py_tt_otp_exists(PyObject* self, PyObject *args) {
	char*		py_identifier = NULL;
	tt_status_t	status = TT_STATUS__UNDEFINED;

	if( g_library == NULL || g_library->version.major != TT_VERSION_MAJOR ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube uninitialized, call tokentube.initialize() first" );
		return NULL;
	}
	if( !PyArg_ParseTuple( args, "s", &py_identifier ) ) {
		PyErr_SetString(PyExc_TypeError, "PyArg_ParseTuple failed" );
		return NULL;
	}
	if( g_library->api.otp.exists( py_identifier, &status ) != TT_OK ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube.api.otp.exists failed" );
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
			PyErr_SetString(PyExc_TypeError, "libtokentube.api.otp.exists returned unknown status" );
			return NULL;
	}
	PyErr_SetString(PyExc_TypeError, "py_tt_otp_exists: internal error" );
	return NULL;
}


PyObject* py_tt_otp_delete(PyObject* self, PyObject *args) {
	char*		py_identifier = NULL;
	tt_status_t	status = TT_STATUS__UNDEFINED;

	if( g_library == NULL || g_library->version.major != TT_VERSION_MAJOR ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube uninitialized, call tokentube.initialize() first" );
		return NULL;
	}
	if( !PyArg_ParseTuple( args, "s", &py_identifier ) ) {
		PyErr_SetString(PyExc_TypeError, "PyArg_ParseTuple failed" );
		return NULL;
	}
	if( g_library->api.otp.delete( py_identifier, &status ) != TT_OK ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube.api.otp.delete failed" );
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
			PyErr_SetString(PyExc_TypeError, "libtokentube.api.otp.delete returned unknown status" );
			return NULL;
	}
	PyErr_SetString(PyExc_TypeError, "py_tt_otp_delete: internal error" );
	return NULL;
}


PyObject* py_tt_otp_execute_challenge(PyObject* self, PyObject *args) {
	char*		py_identifier = NULL;
	char*		py_challenge = NULL;
	ssize_t		py_challenge_size = 0;
	char		data[TT_OTP_TEXT_MAX+1] = {0};
	size_t		data_size = sizeof(data);

	if( g_library == NULL || g_library->version.major != TT_VERSION_MAJOR ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube uninitialized, call tokentube.initialize() first" );
		return NULL;
	}
	if( !PyArg_ParseTuple( args, "ss#", &py_identifier, &py_challenge, &py_challenge_size ) ) {
		PyErr_SetString(PyExc_TypeError, "PyArg_ParseTuple failed" );
		return NULL;
	}
	if( g_library->api.otp.execute_challenge( py_identifier, data, &data_size ) != TT_OK ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube.api.otp.execute_challenge failed" );
		return NULL;
	}
	if( data_size == 0 || data_size > py_challenge_size ) {
		Py_RETURN_FALSE;
	}
	if( strncpy( py_challenge, data, data_size ) == NULL ) {
		Py_RETURN_FALSE;
	}
	Py_RETURN_TRUE;
}


PyObject* py_tt_otp_execute_response(PyObject* self, PyObject *args) {
	char*		py_identifier = NULL;
	char*		py_challenge = NULL;
	char*		py_response = NULL;
	ssize_t		py_response_size = 0;
	char		data[TT_OTP_TEXT_MAX+1] = {0};
	size_t		data_size = sizeof(data);

	if( g_library == NULL || g_library->version.major != TT_VERSION_MAJOR ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube uninitialized, call tokentube.initialize() first" );
		return NULL;
	}
	if( !PyArg_ParseTuple( args, "sss#", &py_identifier, &py_challenge, &py_response, &py_response_size ) ) {
		PyErr_SetString(PyExc_TypeError, "PyArg_ParseTuple failed" );
		return NULL;
	}
	if( g_library->api.otp.execute_response( py_identifier, py_challenge, data, &data_size ) != TT_OK ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube.api.otp.execute_response failed" );
		return NULL;
	}
	if( data_size == 0 || data_size > py_response_size ) {
		Py_RETURN_FALSE;
	}
	if( strncpy( py_response, data, data_size ) == NULL ) {
		Py_RETURN_FALSE;
	}
	Py_RETURN_TRUE;
}


PyObject* py_tt_otp_execute_apply(PyObject* self, PyObject *args) {
	char*		py_identifier = NULL;
	char*		py_challenge = NULL;
	char*		py_response = NULL;
	char*		py_key = NULL;
	ssize_t		py_key_size = 0;
	char		data[TT_OTP_TEXT_MAX+1] = {0};
	size_t		data_size = sizeof(data);

	if( g_library == NULL || g_library->version.major != TT_VERSION_MAJOR ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube uninitialized, call tokentube.initialize() first" );
		return NULL;
	}
	if( !PyArg_ParseTuple( args, "ssss#", &py_identifier, &py_challenge, &py_response, &py_key, &py_key_size ) ) {
		PyErr_SetString(PyExc_TypeError, "PyArg_ParseTuple failed" );
		return NULL;
	}
	if( g_library->api.otp.execute_apply( py_identifier, py_challenge, py_response, data, &data_size ) != TT_OK ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube.api.otp.execute_apply failed" );
		return NULL;
	}
	if( data_size == 0 || data_size > py_key_size ) {
		Py_RETURN_FALSE;
	}
	if( strncpy( py_key, data, data_size ) == NULL ) {
		Py_RETURN_FALSE;
	}
	Py_RETURN_TRUE;
}

