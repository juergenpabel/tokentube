#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "libtokentube.h"


__attribute__ ((visibility ("hidden")))
int default__api__luks_load(char* key, size_t* key_size) {
	char	filename[FILENAME_MAX+1] = { 0 };
	size_t	filename_size = sizeof(filename);

	TT_TRACE( "plugin/default", "%s(key='%p',key_size=%p)", __FUNCTION__, key, key_size );
	if( key == NULL || key_size == NULL || *key_size == 0 ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( libtokentube_conf_read_str( "storage|luks-key|file", filename, &filename_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "internal error in %s at %d", __FILE__, __LINE__ );
		return TT_ERR;
	}
	if( filename_size == 0 ) {
		TT_LOG_ERROR( "plugin/default", "no file configured for LUKS key (luks|file)" );
		return TT_IGN;
	}
	if( default__posix_load( filename, (char*)key, key_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "default__posix_load() failed for file '%s'", filename );
		return TT_ERR;
	}
	return TT_OK;
}

