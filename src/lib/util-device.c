#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include "libtokentube.h"


__attribute__ ((visibility ("hidden")))
int libtokentube_util_device_find(dev_t device, char* buffer, size_t* buffer_size) {
	struct dirent**	list = NULL;
	char*		name = NULL;
	struct stat	st;
	int		i, entries;

	if( buffer == NULL || buffer_size == NULL || *buffer_size == 0 ) {
		TT_LOG_ERROR( "library/util", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	name = malloc( *buffer_size );
	if( name == NULL ) {
		TT_LOG_ERROR( "library/util", "malloc() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	memset( name, '\0', *buffer_size );
	entries = scandir( "/dev/disk/by-uuid", &list, NULL, alphasort );
	for( i=0; i<entries; i++ ) {
		if( name != NULL ) {
			snprintf( name, *buffer_size-1, "/dev/disk/by-uuid/%s", list[i]->d_name );
			if( stat( name, &st ) < 0 ) {
				TT_LOG_ERROR( "library/util", "stat() failed for '%s' in %s()", name, __FUNCTION__ );
				free( name );
				return TT_ERR;
			}
			if( st.st_rdev == device ) {
				strncpy( buffer, name, *buffer_size );
				*buffer_size = strnlen( buffer, *buffer_size );
				free( name );
				name = NULL;
			}
		}
		free( list[i] );
		list[i] = NULL;
	}
	if( name != NULL ) {
		free( name );
		*buffer_size = 0;
	}
	free( list );
	return TT_OK;
}

