#define _GNU_SOURCE
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include "libtokentube.h"


static char* g_bootdevice = NULL;


__attribute__ ((visibility ("hidden")))
int libtokentube_runtime_initialize() {
	if( getuid() != geteuid() ) {
		return TT_OK;
	}
	g_bootdevice = getenv( "TT_RUNTIME_BOOTDEVICE" );
	if( g_bootdevice != NULL ) {
		g_bootdevice = strndup( g_bootdevice, FILENAME_MAX+1 );
		TT_DEBUG1( "library/runtime", "setting bootdevice='%s' (as specified by TT_RUNTIME_BOOTDEVICE)", g_bootdevice );
	}
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_runtime_configure() {
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_runtime_finalize() {
	if( g_bootdevice != NULL ) {
		free( g_bootdevice );
		g_bootdevice = NULL;
	}
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_runtime_get_sysid(char* sysid, size_t* sysid_size) {
	return libtokentube_plugin__runtime_get_sysid( sysid, sysid_size );
}


__attribute__ ((visibility ("hidden")))
int libtokentube_runtime_get_bootdevice(char* result, size_t result_len) {
	if( g_bootdevice != NULL ) {
		TT_DEBUG2( "library/runtime", "returning bootdevice='%s' in %s()", g_bootdevice, __FUNCTION__ );
		strncpy( result, g_bootdevice, result_len-1 );
		return TT_OK;
	}
	return libtokentube_plugin__runtime_get_bootdevice( result, result_len );
}


__attribute__ ((visibility ("hidden")))
int libtokentube_runtime_get_type(tt_runtime_t* runtime) {
	if( g_bootdevice != NULL ) {
		TT_DEBUG2( "library/runtime", "returning TT_RUNTIME__PBA because of configured bootdevice in %s()", __FUNCTION__ );
		*runtime = TT_RUNTIME__PBA;
		return TT_OK;
	}
	return libtokentube_plugin__runtime_get_type( runtime );
}


__attribute__ ((visibility ("hidden")))
int libtokentube_runtime_is_pba(tt_status_t* status) {
	tt_runtime_t runtime = TT_RUNTIME__UNDEFINED;

	if( g_bootdevice != NULL ) {
		TT_DEBUG2( "library/runtime", "returning TT_YES because of configured bootdevice in %s()", __FUNCTION__ );
		*status = TT_YES;
		return TT_OK;
	}
	if( libtokentube_plugin__runtime_get_type( &runtime ) != TT_OK ) {
		TT_LOG_ERROR( "library/runtime", "libtokentube_plugin__runtime_get_type() failed in %s()", __FUNCTION__ );
		*status = TT_STATUS__UNDEFINED;
		return TT_ERR;
	}
	switch( runtime ) {
		case TT_RUNTIME__PBA:
			TT_DEBUG2( "library/runtime", "returning TT_YES in %s()", __FUNCTION__ );
			*status = TT_YES;
			break;
		default:
			TT_DEBUG2( "library/runtime", "returning TT_NO in %s()", __FUNCTION__ );
			*status = TT_NO;
	}
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_runtime_is_standard(tt_status_t* status) {
	tt_runtime_t runtime = TT_RUNTIME__UNDEFINED;

	if( g_bootdevice != NULL ) {
		TT_DEBUG2( "library/runtime", "returning TT_NO because of configured bootdevice in %s()", __FUNCTION__ );
		*status = TT_NO;
		return TT_OK;
	}
	if( libtokentube_plugin__runtime_get_type( &runtime ) != TT_OK ) {
		TT_LOG_ERROR( "library/runtime", "libtokentube_plugin__runtime_get_type() failed in %s()", __FUNCTION__ );
		*status = TT_STATUS__UNDEFINED;
		return TT_ERR;
	}
	switch( runtime ) {
		case TT_RUNTIME__STANDARD:
			TT_DEBUG2( "library/runtime", "returning TT_YES in %s()", __FUNCTION__ );
			*status = TT_YES;
			break;
		default:
			TT_DEBUG2( "library/runtime", "returning TT_NO in %s()", __FUNCTION__ );
			*status = TT_NO;
	}
	return TT_OK;
}

