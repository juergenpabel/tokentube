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
int default__storage_posix_load(tt_file_t type, const char* filename, char* buffer, size_t* buffer_size) {
        struct stat	st = {0};
	struct dirent*	entry = NULL;
	DIR*		dir = NULL;
	int		pos = 0, fd = -1;

	TT_TRACE( "library/plugin", "%s(filename='%s',buffer=%p,buffer_size=%p)", __FUNCTION__, filename, buffer, buffer_size );
	if( type == TT_FILE__UNDEFINED || filename == NULL || filename[0] == '\0' || buffer == NULL || buffer_size == NULL || *buffer_size == 0 ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	fd = open( filename, O_RDONLY );
	if( fd < 0 ) {
		TT_LOG_ERROR( "plugin/default", "open() failed for '%s' in %s()", filename, __FUNCTION__ );
		return TT_ERR;
	}
	if( fstat( fd, &st ) < 0 ) {
		TT_LOG_ERROR( "plugin/default", "fstat() failed for '%s' in %s()", filename, __FUNCTION__ );
		close( fd );
		return TT_ERR;
	}
	switch( st.st_mode & S_IFMT ) {
		case S_IFDIR:
			dir = fdopendir( fd );
			if( dir == NULL ) {
				TT_LOG_ERROR( "plugin/default", "fdopendir() failed for '%s' in %s()", filename, __FUNCTION__ );
				close( fd );
				return TT_ERR;
			}
			entry = readdir( dir );
			while( entry != NULL ) {
				TT_DEBUG5( "plugin/default", "readdir() found d_name='%s' with d_type=%d", entry->d_name, entry->d_type );
				if( entry->d_type == DT_LNK || entry->d_type == DT_REG ) {
					TT_DEBUG4( "plugin/default", "readdir() returned d_name='%s' with d_type=%d", entry->d_name, entry->d_type );
					pos += snprintf( buffer+pos, *buffer_size-pos, "%s/%s\n", filename, entry->d_name );
				}
				entry = readdir( dir );
			}
			closedir( dir );
			break;
		case S_IFREG:
			if( st.st_size > (ssize_t)*buffer_size ) {
				TT_LOG_ERROR( "plugin/default", "insufficient buffer provided in %s()", __FUNCTION__ );
				close( fd );
				return TT_ERR;
			}
			if( read( fd, buffer, *buffer_size ) < st.st_size ) {
				TT_LOG_ERROR( "plugin/default", "read() failed for '%s' in %s()", filename, __FUNCTION__ );
				close( fd );
				return TT_ERR;
			}
			*buffer_size = st.st_size;
			break;
		default:
			TT_LOG_ERROR( "plugin/default", "fdopendir() failed for '%s' in %s()", filename, __FUNCTION__ );
	}
	close( fd );
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int default__storage_posix_save(tt_file_t type, const char* filename, const char* buffer, const size_t buffer_size) {
	char		username[TT_USERNAME_CHAR_MAX+1] = {0};
	size_t		username_size = sizeof(username);
	char		groupname[TT_USERNAME_CHAR_MAX+1] = {0};
	size_t		groupname_size = sizeof(groupname);
	char		permission[5] = {0};
	size_t		permission_size = sizeof(permission);
	const char*     conf_owner = NULL;
	const char*     conf_group = NULL;
	const char*     conf_perm = NULL;
	struct passwd*	owner = NULL;
	struct group*	group = NULL;
	uid_t		uid = -1;
	gid_t		gid = -1;
	mode_t		mode = 0;
	int		fd = -1;

	TT_TRACE( "library/plugin", "%s(filename=%d,buffer=%p,buffer_size='%zd')", __FUNCTION__, filename, buffer, buffer_size );
	if( type == TT_FILE__UNDEFINED || filename == NULL || filename[0] == '\0' || buffer == NULL || buffer_size == 0 ) {
        	TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	switch( type ) {
		case TT_FILE__KEY:
			conf_owner = "storage|key-files|owner";
			conf_group = "storage|key-files|group";
			conf_perm  = "storage|key-files|permission";
		case TT_FILE__USER:
			conf_owner = "storage|user-files|owner";
			conf_group = "storage|user-files|group";
			conf_perm  = "storage|user-files|permission";
			break;
		case TT_FILE__OTP:
			conf_owner = "storage|otp-files|owner";
			conf_group = "storage|otp-files|group";
			conf_perm  = "storage|otp-files|permission";
		case TT_FILE__UHD:
			conf_owner = "storage|helpdesk-files|owner";
			conf_group = "storage|helpdesk-files|group";
			conf_perm  = "storage|helpdesk-files|permission";
			break;
		default:
			break;
	}
	if( libtokentube_conf_read_str( conf_owner, username, &username_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "reading %s failed in %s()", conf_owner, __FUNCTION__ );
		return TT_ERR;
	}
	if( libtokentube_conf_read_str( conf_group, groupname, &groupname_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "reading %s failed in %s()", conf_group, __FUNCTION__ );
		return TT_ERR;
	}
	if( libtokentube_conf_read_str( conf_perm, permission, &permission_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "reading %s failed in %s()", conf_perm, __FUNCTION__ );
		return TT_ERR;
	}
	if( username[0] != '\0' ) {
		owner = getpwnam( username );
		if( owner == NULL ) {
			TT_LOG_ERROR( "plugin/default", "getpwnam() failed for username='%s' in %s()", username, __FUNCTION__ );
			return TT_ERR;
		}
		uid = owner->pw_uid;
	}
	if( groupname[0] != '\0' ) {
		group = getgrnam( groupname );
		if( group == NULL ) {
			TT_LOG_ERROR( "plugin/default", "getgrnam() failed for groupname='%s' in %s()", groupname, __FUNCTION__ );
			return TT_ERR;
		}
		gid = group->gr_gid;
	}
	if( permission[0] != '\0' ) {
		mode = strtol( permission, NULL, 8 );
		if( mode == 0 ) {
			mode = S_IRWXU|S_IRGRP|S_IXGRP;
			TT_LOG_WARN( "plugin/default", "strtol() failed for mode='%s' in %s()", permission, __FUNCTION__ );
		}
	}
	fd = open( filename, O_CREAT|O_WRONLY|O_TRUNC, mode );
	if( fd < 0 ) {
       		TT_LOG_ERROR( "plugin/default", "open() failed for '%s' in %s()", filename, __FUNCTION__ );
		return TT_ERR;
	}
	if( (int)uid != -1 || (int)gid != -1 ) {
		if( fchown( fd, uid, gid ) < 0 ) {
			TT_LOG_ERROR( "plugin/default", "fchown() failed for '%s' in %s()", filename, __FUNCTION__ );
			unlink( filename );
			close( fd );
			return TT_ERR;
		}
	}
	if( mode > 0 ) {
		if( fchmod( fd, mode ) < 0 ) {
			TT_LOG_ERROR( "plugin/default", "fchmod() failed for '%s' in %s()", filename, __FUNCTION__ );
			unlink( filename );
			close( fd );
			return TT_ERR;
		}
	}
	if( write( fd, buffer, buffer_size ) < (ssize_t)buffer_size ) {
		TT_LOG_ERROR( "plugin/default", "write() failed for '%s' in %s()", filename, __FUNCTION__ );
		unlink( filename );
		close( fd );
		return TT_ERR;
	}
	close( fd );
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int default__storage_posix_exists(tt_file_t type, const char* filename, tt_status_t* status) {
        struct stat	st;

	TT_TRACE( "library/plugin", "%s(filename=%d,status=%p)", __FUNCTION__, filename, status );
	if( type == TT_FILE__UNDEFINED || filename == NULL || filename[0] == '\0' || status == NULL ) {
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
int default__storage_posix_delete(tt_file_t type, const char* filename, tt_status_t* status) {
	TT_TRACE( "library/plugin", "%s(filename=%d,status=%p)", __FUNCTION__, filename, status );
	if( type == TT_FILE__UNDEFINED || filename == NULL || filename[0] == '\0' || status == NULL ) {
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

