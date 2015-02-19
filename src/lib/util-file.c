#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "libtokentube.h"


__attribute__ ((visibility ("hidden")))
int libtokentube_util_posix_mkdir(const char* path) {
	char		data[FILENAME_MAX+1] = { 0 };
	size_t		pos = 0;

	if( path == NULL ) {
		TT_LOG_ERROR( "library/util", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	strncpy( data, path, sizeof(data)-1 );
	for( pos=0; data[pos] != '\0'; pos++ ) {
		if( data[pos] == '/' && pos > 0 ) {
			data[pos] = '\0';
			if( mkdir( data, 0700 ) < 0 && errno != EEXIST ) {
				TT_LOG_ERROR( "library/util", "mkdir() failed for '%s' in %s()", data, __FUNCTION__ );
				return TT_ERR;
			}
			data[pos] = '/';
		}
	}
	if( mkdir( data, 0700 ) < 0 && errno != EEXIST ) {
		TT_LOG_ERROR( "library/util", "mkdir() failed for '%s' in %s()", data, __FUNCTION__ );
		return TT_ERR;
	}
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_util_posix_copy(const char* src, const char* dst) {
	char		data[DEFAULT__FILESIZE_MAX];
	ssize_t		bytes;
	struct stat	st;
	int		fd_src, fd_dst;

	if( src == NULL || dst == NULL ) {
		TT_LOG_ERROR( "library/util", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( stat( src, &st ) != 0 ) {
		TT_LOG_ERROR( "library/util", "stat() failed for '%s' in %s()", src, __FUNCTION__ );
		return TT_ERR;
	}
	fd_src = open( src, O_RDONLY );
	fd_dst = open( dst, O_CREAT|O_TRUNC|O_WRONLY, st.st_mode );
	if( fd_src < 0 || fd_dst < 0 ) {
		TT_LOG_ERROR( "library/util", "open() failed for '%s' or '%s' in %s()", src, dst, __FUNCTION__ );
		close( fd_src );
		close( fd_dst );
		return TT_ERR;
	}
	fchown( fd_dst, st.st_uid, st.st_gid );
	bytes = read( fd_src, data, sizeof(data) );
	while( bytes > 0 ) {
		if( write( fd_dst, data, bytes ) != bytes ) {
			TT_LOG_ERROR( "library/util", "write() failed for %zd bytes in %s()", bytes, __FUNCTION__ );
			close( fd_src );
			close( fd_dst );
			return TT_ERR;
		}
		bytes = read( fd_src, data, sizeof(data) );
	}
	close( fd_src );
	close( fd_dst );
	return TT_OK;
}

