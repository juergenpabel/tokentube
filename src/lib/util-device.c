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
	char		filename[FILENAME_MAX+1] = {0};
	struct dirent**	list = NULL;
	struct stat	st;
	int		i, entries;

	if( buffer == NULL || buffer_size == NULL || *buffer_size == 0 ) {
		TT_LOG_ERROR( "library/util", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	memset( buffer, '\0', *buffer_size );
	entries = scandir( "/dev/disk/by-uuid", &list, NULL, alphasort );
	for( i=0; i<entries; i++ ) {
		snprintf( filename, sizeof(filename)-1, "/dev/disk/by-uuid/%s", list[i]->d_name );
		if( stat( filename, &st ) == 0 ) {
			if( st.st_rdev == device ) {
				strncpy( buffer, filename, *buffer_size );
			}
		}
		free( list[i] );
		list[i] = NULL;
	}
	free( list );
	*buffer_size = strnlen( buffer, *buffer_size );
	return TT_OK;
}

