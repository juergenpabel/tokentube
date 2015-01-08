#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/kdev_t.h>
#include <pwd.h>
#include <grp.h>
#include <syslog.h>
#include <tokentube/plugin.h>


extern tt_plugin_t	g_self;


__attribute__ ((visibility ("hidden")))
int test__impl__posix_exists(const char* filename, int* status) {
        struct stat st;
        int	result = TT_ERR;

        if( filename == NULL || status == NULL ) {
		return TT_ERR;
	}
	switch( stat( filename, &st ) ) {
		case 0:
			result = TT_OK;
			*status = TT_YES;
			break;
		case -1:
			result = TT_OK;
			*status = TT_NO;
			break;
		default:
			g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/test", "stat('%s') failed in test__impl__get_filename()", filename );
	}
	return result;
}


__attribute__ ((visibility ("hidden")))
int test__impl__posix_load(const char* filename, char* buffer, size_t* buffer_size) {
        struct stat st;
        int	fd, result = TT_ERR;

        if( filename == NULL ) {
		return TT_ERR;
	}
	if( stat( filename, &st ) != 0 ) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/test", "stat('%s') failed in test__impl__posix_load()", filename );
		return TT_ERR;
	}
	if( st.st_size <= (int)*buffer_size ) {
		fd = open( filename, O_RDONLY );
		if( fd >= 0 ) {
			if( read( fd, buffer, *buffer_size ) == st.st_size ) {
				*buffer_size = st.st_size;
				result = TT_OK;
				g_self.library.api.runtime.debug( TT_DEBUG__VERBOSITY5, "plugin/test", "successfully loaded '%s' in test__impl__posix_load()", filename );
			}
			close(fd);
		}
	} else {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/test", "buffer (%zd bytes) not sufficient for '%s' (%zd bytes) in test__impl__get_filename()", *buffer_size, filename, st.st_size );
	}
        return result;
}


__attribute__ ((visibility ("hidden")))
int test__impl__posix_save(const char* filename, const char* buffer, const size_t buffer_size) {
	int             fd = -1;
	int             result = TT_ERR;

	if( filename == NULL ) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/test", "invalid filename in test__impl__posix_save()" );
		return TT_ERR;
	}
	fd = open( filename, O_CREAT|O_WRONLY|O_TRUNC, 0600 );
	if( fd >= 0 ) {
		if( write( fd, buffer, buffer_size ) == (int)buffer_size ) {
			result = TT_OK;
			g_self.library.api.runtime.debug( TT_DEBUG__VERBOSITY5, "plugin/test", "successfully saved '%s' in test__impl__posix_save()", filename );
		}
		close(fd);
        }
	return result;
}


__attribute__ ((visibility ("hidden")))
int test__impl__posix_delete(const char* filename, int* status) {
	int	result = TT_ERR;

	if( filename != NULL ) {
		switch( unlink( filename ) ) {
			case 0:
				result = TT_OK;
				*status = TT_YES;
				break;
			case -1:
				if( errno == ENOENT ) {
					result = TT_OK;
					*status = TT_NO;
					break;
				}
			default:
				g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/test", "unlink('%s') failed test__impl__posix_delete()", filename );
		}
	}
	return result;
}

