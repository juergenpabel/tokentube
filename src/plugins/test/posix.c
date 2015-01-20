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
        struct stat	st;
        int		fd;

        if( filename == NULL ) {
		return TT_ERR;
	}
	fd = open( filename, O_RDONLY );
	if( fd < 0 ) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/test", "open() failed for '%s' in %s()", filename, __FUNCTION__ );
		return TT_ERR;
	}
	if( fstat( fd, &st ) < 0 ) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/test", "fstat() failed for '%s' in %s()", filename, __FUNCTION__ );
		close( fd );
		return TT_ERR;
	}
	if( st.st_size > (off_t)*buffer_size ) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/test", "buffer insufficient (%zd vs. %zd) bytes for '%s' in %s()", *buffer_size, st.st_size, filename, __FUNCTION__ );
		close( fd );
		return TT_ERR;
	}
	if( read( fd, buffer, *buffer_size ) != st.st_size ) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/test", "read() failed for '%s' in %s()", filename, __FUNCTION__ );
		close( fd );
		return TT_ERR;
	}
	*buffer_size = st.st_size;
	close( fd );
	g_self.library.api.runtime.debug( TT_DEBUG__VERBOSITY5, "plugin/test", "successfully loaded '%s' in test__impl__posix_load()", filename );
        return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int test__impl__posix_save(const char* filename, const char* buffer, const size_t buffer_size) {
	int             fd = -1;

	if( filename == NULL ) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/test", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	g_self.library.api.runtime.debug( TT_DEBUG__VERBOSITY5, "plugin/test", "saving '%s' in %s()", filename, __FUNCTION__ );
	fd = open( filename, O_CREAT|O_WRONLY|O_TRUNC, 0600 );
	if( fd < 0 ) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/test", "open() failed for '%s' in %s()", filename, __FUNCTION__ );
		return TT_ERR;
	}
	if( write( fd, buffer, buffer_size ) != (int)buffer_size ) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/test", "write() failed for '%s' in %s()", filename, __FUNCTION__ );
		close( fd );
		return TT_ERR;
	}
	close( fd );
	g_self.library.api.runtime.debug( TT_DEBUG__VERBOSITY4, "plugin/test", "successfully saved '%s' in %s()", filename, __FUNCTION__ );
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int test__impl__posix_delete(const char* filename, int* status) {
	int	result = TT_ERR;

	if( filename == NULL || status == NULL ) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/test", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	*status = TT_STATUS__UNDEFINED;
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
			g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/test", "unlink() failed for '%s' in %s()", filename, __FUNCTION__ );
	}
	return result;
}

