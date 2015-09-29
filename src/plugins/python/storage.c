#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <string.h>
#include <confuse.h>
#include <Python.h>
#include <tokentube/plugin.h>
#include "plugin.h"


__attribute__ ((visibility ("hidden")))
int python__storage_load(tt_file_t file, const char* identifier, char* data, size_t* data_size) {
	const char*     type = NULL;
	PyObject*	object = NULL;
	char*		buffer = NULL;
	ssize_t		buffer_size = 0;

	g_self.library.api.runtime.debug( TT_DEBUG__TRACE, "plugin/python", "%s(file=%zd,identifier=%p,data=%p,data_size=%p)", __FUNCTION__, file, identifier, data, data_size );
	if( identifier == NULL || data == NULL || data_size == NULL || *data_size == 0) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/python", "invalid parameter in %s()'", __FUNCTION__ );
		return TT_ERR;
	}
	switch( file ) {
		case TT_FILE__USER:
			type = "USER";
			break;
		case TT_FILE__OTP:
			type = "OTP";
			break;
		case TT_FILE__KEY:
			type = "KEY";
			break;
		default:
			return TT_IGN;
	}
	if( python__exec_script( g_cfg, "storage-load", type, identifier, NULL, &object ) != TT_OK ) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/python", "no script configured for 'storage-load'" );
		return TT_ERR;
	}
	if( object == NULL ) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/python", "script failed for 'storage-load'" );
		return TT_ERR;
	}
	if( !PyBytes_Check( object ) ) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/python", "script returned invalid object for 'storage-load'" );
		return TT_ERR;
	}
	PyBytes_AsStringAndSize( object, &buffer, &buffer_size );
	if( buffer == NULL ) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/python", "PyBytes_AsStringAndSize() failed for 'storage-load'" );
		return TT_ERR;
	}
	if( (size_t)buffer_size > *data_size ) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/python", "not enough data_size for 'storage-load'" );
		return TT_ERR;
	}
	memcpy( data, buffer, buffer_size );
	*data_size = (size_t)buffer_size;
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int python__storage_save(tt_file_t file, const char* identifier, const char* data, size_t data_size) {
	const char*     type = NULL;
	PyObject*	object = NULL;

	g_self.library.api.runtime.debug( TT_DEBUG__TRACE, "plugin/python", "%s(file=%zd,identifier=%p,data=%p,data_size=%d)", __FUNCTION__, file, identifier, data, data_size );
	if( identifier == NULL || data == NULL || data_size == 0) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/python", "invalid parameter in %s()'", __FUNCTION__ );
		return TT_ERR;
	}
	switch( file ) {
		case TT_FILE__USER:
			type = "USER";
			break;
		case TT_FILE__OTP:
			type = "OTP";
			break;
		case TT_FILE__KEY:
			type = "KEY";
			break;
		default:
			return TT_IGN;
	}
	object = Py_BuildValue( "y#", data, data_size );
	if( object == NULL ) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/python", "Py_BuildValue() failed for 'storage-save'" );
		return TT_ERR;
	}
	if( python__exec_script( g_cfg, "storage-save", type, identifier, &object, NULL ) != TT_OK ) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/python", "no script configured for 'storage-save'" );
		return TT_ERR;
	}
	return TT_OK;
}

