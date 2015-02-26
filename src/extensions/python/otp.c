#include <stddef.h>
#include <tokentube.h>
#include <Python.h>
#include "extension.h"


PyObject* py_tt_otp_create(PyObject* self, PyObject *args) {
	char*		identifier = NULL;

	if( g_library == NULL || g_library->version.major != TT_VERSION_MAJOR ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube uninitialized, call tokentube.initialize() first" );
		return NULL;
	}
	if( !PyArg_ParseTuple( args, "s", &identifier ) ) {
		PyErr_SetString(PyExc_TypeError, "PyArg_ParseTuple failed" );
		return NULL;
	}
	if( g_library->api.otp.create( identifier ) != TT_OK ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube.api.user.create failed" );
		return NULL;
	}
	Py_RETURN_NONE;
}


PyObject* py_tt_otp_exists(PyObject* self, PyObject *args) {
	char*		identifier = NULL;
	tt_status_t	status = TT_STATUS__UNDEFINED;

	if( g_library == NULL || g_library->version.major != TT_VERSION_MAJOR ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube uninitialized, call tokentube.initialize() first" );
		return NULL;
	}
	if( !PyArg_ParseTuple( args, "s", &identifier ) ) {
		PyErr_SetString(PyExc_TypeError, "PyArg_ParseTuple failed" );
		return NULL;
	}
	if( g_library->api.otp.exists( identifier, &status ) != TT_OK ) {
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
	char*		identifier = NULL;
	tt_status_t	status = TT_STATUS__UNDEFINED;

	if( g_library == NULL || g_library->version.major != TT_VERSION_MAJOR ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube uninitialized, call tokentube.initialize() first" );
		return NULL;
	}
	if( !PyArg_ParseTuple( args, "s", &identifier ) ) {
		PyErr_SetString(PyExc_TypeError, "PyArg_ParseTuple failed" );
		return NULL;
	}
	if( g_library->api.otp.delete( identifier, &status ) != TT_OK ) {
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
	char*		identifier = NULL;
	char		challenge[TT_OTP_TEXT_MAX+1] = {0};
	size_t		challenge_size = sizeof(challenge);
	tt_status_t	status = TT_STATUS__UNDEFINED;

	if( g_library == NULL || g_library->version.major != TT_VERSION_MAJOR ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube uninitialized, call tokentube.initialize() first" );
		return NULL;
	}
	if( !PyArg_ParseTuple( args, "s", &identifier ) ) {
		PyErr_SetString(PyExc_TypeError, "PyArg_ParseTuple failed" );
		return NULL;
	}
	if( g_library->api.otp.execute_challenge( identifier, challenge, &challenge_size ) != TT_OK ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube.api.otp.execute_challenge failed" );
		return NULL;
	}
	if( challenge_size == 0 ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube.api.otp.execute_challenge return nothing" );
		return NULL;
	}
//TODO:put challenge into python return value
	PyErr_SetString(PyExc_TypeError, "py_tt_otp_execute_challenge: internal error" );
	return NULL;
}


PyObject* py_tt_otp_execute_response(PyObject* self, PyObject *args) {
	char*		identifier = NULL;
	char*		challenge = NULL;
	char		response[TT_OTP_TEXT_MAX+1] = {0};
	size_t		response_size = sizeof(response);
	tt_status_t	status = TT_STATUS__UNDEFINED;

	if( g_library == NULL || g_library->version.major != TT_VERSION_MAJOR ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube uninitialized, call tokentube.initialize() first" );
		return NULL;
	}
	if( !PyArg_ParseTuple( args, "ss", &identifier, &challenge ) ) {
		PyErr_SetString(PyExc_TypeError, "PyArg_ParseTuple failed" );
		return NULL;
	}
	if( g_library->api.otp.execute_response( identifier, challenge, response, &response_size ) != TT_OK ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube.api.otp.execute_response failed" );
		return NULL;
	}
	if( response_size == 0 ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube.api.otp.execute_response return nothing" );
		return NULL;
	}
//TODO:put response into python return value
	PyErr_SetString(PyExc_TypeError, "py_tt_otp_execute_response: internal error" );
	return NULL;
}


PyObject* py_tt_otp_execute_apply(PyObject* self, PyObject *args) {
	char*		identifier = NULL;
	char*		challenge = NULL;
	char*		response = NULL;
	char		key[TT_OTP_TEXT_MAX+1] = {0};
	size_t		key_size = sizeof(key);
	tt_status_t	status = TT_STATUS__UNDEFINED;

	if( g_library == NULL || g_library->version.major != TT_VERSION_MAJOR ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube uninitialized, call tokentube.initialize() first" );
		return NULL;
	}
	if( !PyArg_ParseTuple( args, "sss", &identifier, &challenge, &response ) ) {
		PyErr_SetString(PyExc_TypeError, "PyArg_ParseTuple failed" );
		return NULL;
	}
	if( g_library->api.otp.execute_apply( identifier, challenge, response, key, &key_size ) != TT_OK ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube.api.otp.execute_apply failed" );
		return NULL;
	}
	if( key_size == 0 ) {
		PyErr_SetString(PyExc_TypeError, "libtokentube.api.otp.execute_apply return nothing" );
		return NULL;
	}
//TODO:put key into python return value
	PyErr_SetString(PyExc_TypeError, "py_tt_otp_execute_apply: internal error" );
	return NULL;
}

