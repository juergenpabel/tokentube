#include <stddef.h>
#include <tokentube.h>
#include <Python.h>
#include "binding.h"


__attribute__ ((visibility ("hidden")))
PyObject* py_tt_auth_otp_challenge(PyObject* self __attribute__((unused)), PyObject *args) {
	char*		py_identifier = NULL;
	char*		py_challenge = NULL;
	ssize_t		py_challenge_size = 0;
	char		data[TT_OTP_TEXT_MAX+1] = {0};
	size_t		data_size = sizeof(data);

	if( g_library == NULL || g_library->version.major == 0 ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube uninitialized, call tokentube.initialize() first" );
		return NULL;
	}
	if( !PyArg_ParseTuple( args, "ss#", &py_identifier, &py_challenge, &py_challenge_size ) ) {
		PyErr_SetString(PyExc_TypeError, "PyArg_ParseTuple failed" );
		return NULL;
	}
	if( g_library->api.auth.otp.challenge( py_identifier, data, &data_size ) != TT_OK ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube.api.auth.otp.challenge failed" );
		return NULL;
	}
	if( data_size == 0 || data_size > (size_t)py_challenge_size ) {
		Py_RETURN_FALSE;
	}
	if( strncpy( py_challenge, data, data_size ) == NULL ) {
		Py_RETURN_FALSE;
	}
	Py_RETURN_TRUE;
}


__attribute__ ((visibility ("hidden")))
PyObject* py_tt_auth_otp_response(PyObject* self __attribute__((unused)), PyObject *args) {
	char*		py_identifier = NULL;
	char*		py_challenge = NULL;
	char*		py_response = NULL;
	ssize_t		py_response_size = 0;
	char		data[TT_OTP_TEXT_MAX+1] = {0};
	size_t		data_size = sizeof(data);

	if( g_library == NULL || g_library->version.major == 0 ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube uninitialized, call tokentube.initialize() first" );
		return NULL;
	}
	if( !PyArg_ParseTuple( args, "sss#", &py_identifier, &py_challenge, &py_response, &py_response_size ) ) {
		PyErr_SetString(PyExc_TypeError, "PyArg_ParseTuple failed" );
		return NULL;
	}
	if( g_library->api.auth.otp.response( py_identifier, py_challenge, data, &data_size ) != TT_OK ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube.api.auth.otp.response failed" );
		return NULL;
	}
	if( data_size == 0 || data_size > (size_t)py_response_size ) {
		Py_RETURN_FALSE;
	}
	if( strncpy( py_response, data, data_size ) == NULL ) {
		Py_RETURN_FALSE;
	}
	Py_RETURN_TRUE;
}


__attribute__ ((visibility ("hidden")))
PyObject* py_tt_auth_otp_loadkey(PyObject* self __attribute__((unused)), PyObject *args) {
	char*		py_identifier = NULL;
	char*		py_challenge = NULL;
	char*		py_response = NULL;
	char*		py_key = NULL;
	ssize_t		py_key_size = 0;
	char		data[TT_OTP_TEXT_MAX+1] = {0};
	size_t		data_size = sizeof(data);

	if( g_library == NULL || g_library->version.major == 0 ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube uninitialized, call tokentube.initialize() first" );
		return NULL;
	}
	if( !PyArg_ParseTuple( args, "ssss#", &py_identifier, &py_challenge, &py_response, &py_key, &py_key_size ) ) {
		PyErr_SetString(PyExc_TypeError, "PyArg_ParseTuple failed" );
		return NULL;
	}
	if( g_library->api.auth.otp.loadkey( py_identifier, py_challenge, py_response, data, &data_size ) != TT_OK ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube.api.auth.otp.loadkey failed" );
		return NULL;
	}
	if( data_size == 0 || 2*data_size > (size_t)py_key_size ) {
		PyErr_SetString(PyExc_TypeError, "buffer too small in py_tt_auth_otp_loadkey()" );
		Py_RETURN_FALSE;
	}
	if( g_library->api.runtime.util.hex_encode( data, data_size, py_key, &py_key_size ) != TT_OK ) {
		PyErr_SetString(PyExc_TypeError, "API:util.hex_encode() failed in py_tt_auth_otp_loadkey()" );
		Py_RETURN_FALSE;
	}
	Py_RETURN_TRUE;
}

