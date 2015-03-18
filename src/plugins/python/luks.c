#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <string.h>
#include <confuse.h>
#include <Python.h>
#include <tokentube/plugin.h>
#include "plugin.h"


__attribute__ ((visibility ("hidden")))
int python__luks_load(char* key, size_t* key_size) {
	PyObject*	result = NULL;
	char*		data = NULL;
	ssize_t		data_size = 0;

	if( key == NULL || key_size == NULL || *key_size == 0) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/python", "invalid parameter in python__luks_load()'" );
		return TT_ERR;
	}
	if( python__exec_script( g_cfg, "luks-load", &result ) != TT_OK ) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/python", "no script configured for 'luks_load'" );
		return TT_ERR;
	}
	if( result == NULL ) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/python", "script failed for 'luks_load'" );
		return TT_ERR;
	}
	if( !PyBytes_Check( result ) ) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/python", "script returned invalid result for 'luks_load'" );
		return TT_ERR;
	}
	PyBytes_AsStringAndSize( result, &data, &data_size );
	if( data == NULL ) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/python", "PyBytes_AsStringAndSize() failed for 'luks_load'" );
		return TT_ERR;
	}
	if( (size_t)data_size > *key_size ) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/python", "not enough key_size for 'luks_load'" );
		return TT_ERR;
	}
	memcpy( key, data, data_size );
	*key_size = (size_t)data_size;
	return TT_OK;
}

