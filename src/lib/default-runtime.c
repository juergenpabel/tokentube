#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "libtokentube.h"


__attribute__ ((visibility ("hidden")))
int default__api__runtime_get_sysid(char* sysid, size_t* sysid_size) {
	TT_TRACE( "library/plugin", "%s(sysid=%p,sysid_size=%p)", __FUNCTION__, sysid, sysid_size );
	if( sysid == NULL || sysid_size == NULL || *sysid_size == 0 ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( gethostname( sysid, *sysid_size ) != 0 ) {
		TT_LOG_ERROR( "plugin/default", "gethostname() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int default__api__runtime_get_type(tt_runtime_t* result) {
	TT_TRACE( "library/plugin", "%s(result=%p)", __FUNCTION__, result );
	if( result == NULL ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( access( TT_FILENAME__BOOT_CONF, F_OK ) == 0 ) {
		*result = TT_RUNTIME__PBA;
	} else {
		*result = TT_RUNTIME__STANDARD;
	}
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int default__api__runtime_get_bootdevice(char* const buffer, const size_t buffer_size) {
	int len = 0, fd = -1;

	TT_TRACE( "library/plugin", "%s(buffer=%p,buffer_size=%zd)", __FUNCTION__, buffer, buffer_size );
	if( buffer == NULL || buffer_size == 0 ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	fd = open( TT_FILENAME__BOOT_CONF, O_RDONLY );
	if( fd < 0 ) {
		TT_LOG_ERROR( "plugin/default", "open() failed parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	len = read( fd, buffer, buffer_size );
	if( len <= 0 ) {
		TT_LOG_ERROR( "plugin/default", "read() failed parameter in %s()", __FUNCTION__ );
		close( fd );
		return TT_ERR;
	}
	if( buffer[len-1] == '\n' ) {
		buffer[len-1] = '\0';
	} else {
		buffer[len] = '\0';
	}
	close( fd );
	return TT_OK;
}

