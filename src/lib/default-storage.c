#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/kdev_t.h>
#include <pwd.h>
#include <grp.h>
#include "libtokentube.h"


__attribute__ ((visibility ("hidden")))
int default__api__storage_load(tt_file_t type, const char* identifier, char* buffer, size_t* buffer_size) {
	char		filename[FILENAME_MAX+1] = {0};
	tt_runtime_t	runtime = TT_RUNTIME__UNDEFINED;
	size_t		pos = 0;

	TT_TRACE( "library/plugin", "%s(type=%d,identifier='%s',buffer=%p,buffer_size=%p)", __FUNCTION__, type, identifier, buffer, buffer_size );
	if( identifier == NULL || buffer == NULL || buffer_size == NULL || *buffer_size == 0 ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( libtokentube_runtime_get_type( &runtime ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_runtime_get_type() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( default__impl__storage_get_filename( type, identifier, filename, sizeof(filename) ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "default__impl__storage_get_filename() failed for identifier '%s' in %s()", identifier, __FUNCTION__ );
		return TT_ERR;
	}
	switch( runtime ) {
		case TT_RUNTIME__PBA:
			if( strncmp( "/boot/", filename, 6 ) == 0 ) {
				for( pos=0; 6+pos<sizeof(filename); pos++ ) {
					filename[pos] = filename[6+pos];
				}
				if( default__storage_ext2fs_load( type, identifier, filename, buffer, buffer_size ) != TT_OK ) {
					TT_LOG_ERROR( "plugin/default", "default__storage_ext2fs_load() failed in %s()", __FUNCTION__ );
					return TT_ERR;
				}
			} else {
				if( default__storage_posix_load( type, identifier, filename, buffer, buffer_size ) != TT_OK ) {
					TT_LOG_ERROR( "plugin/default", "default__storage_posix_load() failed in %s()", __FUNCTION__ );
					return TT_ERR;
				}
			}
			break;
		case TT_RUNTIME__STANDARD:
			if( default__storage_posix_load( type, identifier, filename, buffer, buffer_size ) != TT_OK ) {
				TT_LOG_ERROR( "plugin/default", "default__storage_posix_load() failed in %s()", __FUNCTION__ );
				return TT_ERR;
			}
			break;
		default:
			TT_LOG_ERROR( "plugin/default", "internal error in %s()", __FUNCTION__ );
			return TT_ERR;
	}
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int default__api__storage_save(tt_file_t type, const char* identifier, const char* buffer, size_t buffer_size) {
	char		filename[FILENAME_MAX+1] = {0};
	tt_runtime_t	runtime = TT_RUNTIME__UNDEFINED;
	size_t		pos = 0;

	TT_TRACE( "library/plugin", "%s(type=%d,identifier='%s',buffer=%p,buffer_size=%zd)", __FUNCTION__, type, identifier, buffer, buffer_size );
	if( type == TT_FILE__UNDEFINED || identifier == NULL || identifier[0] == '\0' || buffer == NULL || buffer_size == 0 ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( libtokentube_runtime_get_type( &runtime ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_runtime_get_type() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( default__impl__storage_get_filename( type, identifier, filename, sizeof(filename) ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "default__impl__storage_get_filename() failed for identifier '%s' in %s()", identifier, __FUNCTION__ );
		return TT_ERR;
	}
	switch( runtime ) {
		case TT_RUNTIME__PBA:
			if( strncmp( "/boot/", filename, 6 ) == 0 ) {
				for( pos=0; 6+pos<sizeof(filename); pos++ ) {
					filename[pos] = filename[6+pos];
				}
				if( default__storage_ext2fs_save( type, identifier, filename, buffer, buffer_size ) != TT_OK ) {
					TT_LOG_ERROR( "plugin/default", "default__storage_ext2fs_save() failed in %s()", __FUNCTION__ );
					return TT_ERR;
				}
			} else {
				if( default__storage_posix_save( type, identifier, filename, buffer, buffer_size ) != TT_OK ) {
					TT_LOG_ERROR( "plugin/default", "default__storage_posix_save() failed in %s()", __FUNCTION__ );
					return TT_ERR;
				}
			}
			break;
		case TT_RUNTIME__STANDARD:
			if( default__storage_posix_save( type, identifier, filename, buffer, buffer_size ) != TT_OK ) {
				TT_LOG_ERROR( "plugin/default", "default__storage_posix_save() failed in %s()", __FUNCTION__ );
				return TT_ERR;
			}
			break;
		default:
			TT_LOG_ERROR( "plugin/default", "internal error in %s()", __FUNCTION__ );
			return TT_ERR;
	}
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int default__api__storage_exists(tt_file_t type, const char* identifier, tt_status_t* status) {
	char		filename[FILENAME_MAX+1] = {0};
	tt_runtime_t	runtime = TT_RUNTIME__UNDEFINED;
	size_t		pos = 0;

	TT_TRACE( "library/plugin", "%s(type=%d,identifier='%s',status=%p)", __FUNCTION__, type, identifier, status );
	if( type == TT_FILE__UNDEFINED || identifier == NULL || identifier[0] == '\0' || status == NULL ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( libtokentube_runtime_get_type( &runtime ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_runtime_get_type() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( default__impl__storage_get_filename( type, identifier, filename, sizeof(filename) ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "default__impl__storage_get_filename() failed for identifier '%s' in %s()", identifier, __FUNCTION__ );
		return TT_ERR;
	}
	switch( runtime ) {
		case TT_RUNTIME__PBA:
			if( strncmp( "/boot/", filename, 6 ) == 0 ) {
				for( pos=0; 6+pos<sizeof(filename); pos++ ) {
					filename[pos] = filename[6+pos];
				}
				if( default__storage_ext2fs_exists( type, identifier, filename, status ) != TT_OK ) {
					TT_LOG_ERROR( "plugin/default", "default__storage_ext2fs_exists() failed in %s()", __FUNCTION__ );
					return TT_ERR;
				}
			} else {
				if( default__storage_posix_exists( type, identifier, filename, status ) != TT_OK ) {
					TT_LOG_ERROR( "plugin/default", "default__storage_posix_exists() failed in %s()", __FUNCTION__ );
					return TT_ERR;
				}
			}
			break;
		case TT_RUNTIME__STANDARD:
			if( default__storage_posix_exists( type, identifier, filename, status ) != TT_OK ) {
				TT_LOG_ERROR( "plugin/default", "default__storage_posix_exists() failed in %s()", __FUNCTION__ );
				return TT_ERR;
			}
			break;
		default:
			TT_LOG_ERROR( "plugin/default", "internal error in %s()", __FUNCTION__ );
			return TT_ERR;
	}
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int default__api__storage_delete(tt_file_t type, const char* identifier, tt_status_t* status) {
	char		filename[FILENAME_MAX+1] = {0};
	tt_runtime_t	runtime = TT_RUNTIME__UNDEFINED;
	size_t		pos = 0;

	TT_TRACE( "library/plugin", "%s(type=%d,identifier='%s',status=%p)", __FUNCTION__, type, identifier, status );
	if( type == TT_FILE__UNDEFINED || identifier == NULL || identifier[0] == '\0' || status == NULL ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( libtokentube_runtime_get_type( &runtime ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_runtime_get_type() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( default__impl__storage_get_filename( type, identifier, filename, sizeof(filename) ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "default__impl__storage_get_filename() failed for identifier '%s' in %s()", identifier, __FUNCTION__ );
		return TT_ERR;
	}
	switch( runtime ) {
		case TT_RUNTIME__PBA:
			if( strncmp( "/boot/", filename, 6 ) == 0 ) {
				for( pos=0; 6+pos<sizeof(filename); pos++ ) {
					filename[pos] = filename[6+pos];
				}
				if( default__storage_ext2fs_delete( type, identifier, filename, status ) != TT_OK ) {
					TT_LOG_ERROR( "plugin/default", "default__storage_ext2fs_delete() failed in %s()", __FUNCTION__ );
					return TT_ERR;
				}
			} else {
				if( default__storage_posix_delete( type, identifier, filename, status ) != TT_OK ) {
					TT_LOG_ERROR( "plugin/default", "default__storage_posix_delete() failed in %s()", __FUNCTION__ );
					return TT_ERR;
				}
			}
			break;
		case TT_RUNTIME__STANDARD:
			if( default__storage_posix_delete( type, identifier, filename, status ) != TT_OK ) {
				TT_LOG_ERROR( "plugin/default", "default__storage_posix_delete() failed in %s()", __FUNCTION__ );
				return TT_ERR;
			}
			break;
		default:
			TT_LOG_ERROR( "plugin/default", "internal error in %s()", __FUNCTION__ );
			return TT_ERR;
	}
	return TT_OK;
}

