#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>
#include <gcrypt.h>
#include "libtokentube.h"


__attribute__ ((visibility ("hidden")))
int default__posix_load(const char* filename, char* buffer, size_t* buffer_size) {
        struct stat	st;
	int		fd = -1;

	TT_TRACE( "library/plugin", "%s(filename=%d,buffer=%p,buffer_size='%p')", __FUNCTION__, filename, buffer, buffer_size );
	if( filename == NULL || filename[0] == '\0' || buffer == NULL || buffer_size == NULL || *buffer_size == 0 ) {
        	TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( stat( filename, &st ) < 0 ) {
       		TT_LOG_ERROR( "plugin/default", "stat() failed for '%s' in %s()", filename, __FUNCTION__ );
		return TT_ERR;
	}
	if( st.st_size > (int)*buffer_size ) {
       		TT_LOG_ERROR( "plugin/default", "insufficient buffer provided in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	fd = open( filename, O_RDONLY );
	if( fd < 0 ) {
       		TT_LOG_ERROR( "plugin/default", "open() failed for '%s' in %s()", filename, __FUNCTION__ );
		return TT_ERR;
	}
	if( read( fd, buffer, *buffer_size ) < st.st_size ) {
       		TT_LOG_ERROR( "plugin/default", "read() failed for '%s' in %s()", filename, __FUNCTION__ );
		close(fd);
		return TT_ERR;
	}
	close(fd);
	*buffer_size = st.st_size;
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int default__posix_save(const char* filename, const char* buffer, const size_t buffer_size) {
	char		username[TT_USERNAME_CHAR_MAX+1] = {0};
	size_t		username_size = sizeof(username);
	char		groupname[TT_USERNAME_CHAR_MAX+1] = {0};
	size_t		groupname_size = sizeof(groupname);
	char		permission[5] = {0};
	size_t		permission_size = sizeof(permission);
	struct passwd*	owner = NULL;
	struct group*	group = NULL;
	mode_t		mode = 0;
	int		fd = -1;

	TT_TRACE( "library/plugin", "%s(filename=%d,buffer=%p,buffer_size='%zd')", __FUNCTION__, filename, buffer, buffer_size );
	if( filename == NULL || filename[0] == '\0' || buffer == NULL || buffer_size == 0 ) {
        	TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( libtokentube_conf_read_str( "storage|files|owner", username, &username_size ) != TT_OK ) {
		return TT_ERR;
	}
	if( libtokentube_conf_read_str( "storage|files|group", groupname, &groupname_size ) != TT_OK ) {
		return TT_ERR;
	}
	if( libtokentube_conf_read_str( "storage|files|permission", permission, &permission_size ) != TT_OK ) {
		return TT_ERR;
	}
	owner = getpwnam( username );
	if( owner == NULL ) {
        	TT_LOG_ERROR( "plugin/default", "getpwnam() failed for username='%s' in %s()", username, __FUNCTION__ );
		return TT_ERR;
	}
	group = getgrnam( groupname );
	if( owner == NULL ) {
        	TT_LOG_ERROR( "plugin/default", "getgrnam() failed for groupname='%s' in %s()", groupname, __FUNCTION__ );
		return TT_ERR;
	}
	mode = strtol( permission, NULL, 8 );
	if( mode == 0 ) {
		mode = S_IRWXU|S_IRGRP|S_IXGRP;
		TT_LOG_WARN( "plugin/default", "strtol() failed for mode='%s' in %s()", permission, __FUNCTION__ );
	}
	fd = open( filename, O_CREAT|O_WRONLY|O_TRUNC, mode );
	if( fd < 0 ) {
       		TT_LOG_ERROR( "plugin/default", "open() failed for '%s' in %s()", filename, __FUNCTION__ );
		return TT_ERR;
	}
	if( fchown( fd, owner->pw_uid, group->gr_gid ) < 0 ) {
       		TT_LOG_ERROR( "plugin/default", "fchown() failed for '%s' in %s()", filename, __FUNCTION__ );
		unlink( filename );
		close(fd);
		return TT_ERR;
	}
	if( fchmod( fd, mode ) < 0 ) {
       		TT_LOG_ERROR( "plugin/default", "fchmod() failed for '%s' in %s()", filename, __FUNCTION__ );
		unlink( filename );
		close(fd);
		return TT_ERR;
	}
	if( write( fd, buffer, buffer_size ) < (int)buffer_size ) {
		TT_LOG_ERROR( "plugin/default", "write() failed for '%s' in %s()", filename, __FUNCTION__ );
		unlink( filename );
		close(fd);
		return TT_ERR;
	}
	close(fd);
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int default__posix_exists(const char* filename, tt_status_t* status) {
        struct stat	st;

	TT_TRACE( "library/plugin", "%s(filename=%d,status=%p)", __FUNCTION__, filename, status );
	if( filename == NULL || filename[0] == '\0' || status == NULL ) {
        	TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
       	if( stat( filename, &st ) < 0 ) {
		if( errno != ENOENT ) {
			TT_LOG_ERROR( "plugin/default", "stat() failed for '%s' in %s()", filename, __FUNCTION__ );
			return TT_ERR;
		}
		*status = TT_NO;
	} else {
		*status = TT_YES;
	}
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int default__posix_delete(const char* filename, tt_status_t* status) {
	TT_TRACE( "library/plugin", "%s(filename=%d,status=%p)", __FUNCTION__, filename, status );
	if( filename == NULL || filename[0] == '\0' || status == NULL ) {
        	TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( unlink( filename ) < 0 ) {
		if( errno == ENOENT ) {
			*status = TT_NO;
			return TT_OK;
		}
		TT_LOG_ERROR( "plugin/default", "unlink() failed for '%s' in %s()", filename, __FUNCTION__ );
		return TT_ERR;
	}
	*status = TT_YES;
	return TT_OK;
}

