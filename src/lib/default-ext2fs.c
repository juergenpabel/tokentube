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
#include <linux/kdev_t.h>
#include <ext2fs/ext2_fs.h>
#include <ext2fs/ext2fs.h>
#include <pwd.h>
#include <grp.h>
#include <gcrypt.h>
#include "libtokentube.h"


__attribute__ ((visibility ("hidden")))
int default__ext2fs_load(const char* filename, char* buffer, size_t* buffer_size) {
	char		device[FILENAME_MAX+1] = {0};
	ext2_filsys	e2fs;
	ext2_ino_t	e2dir;
	ext2_ino_t	e2file;
	ext2_file_t	target_file;
	const char*	next;

	TT_TRACE( "plugin/default", "%s(filename='%s',buffer=%p,buffer_size=%p)", __FUNCTION__, filename, buffer, buffer_size );
	if( filename == NULL || buffer == NULL || buffer_size == NULL || *buffer_size == 0 ) {
        	TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}

	if( libtokentube_runtime_get_bootdevice( device, sizeof(device) ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "internal error in %s at %d", __FILE__, __LINE__ );
		return TT_ERR;
	}

	TT_DEBUG2( "plugin/default", "opening '%s' as EXT2 filesystem in %s()", device, __FUNCTION__ );
	if( ext2fs_open( device, EXT2_FLAG_FORCE, 0, 0, unix_io_manager, &e2fs ) != 0 ) {
		TT_LOG_ERROR( "plugin/default", "failed to open '%s' as EXT2 filesystem", device );
		return TT_ERR;
	}

	TT_DEBUG2( "plugin/default", "invoking ext2fs_lookup() for '%s'", filename );
	e2dir = EXT2_ROOT_INO;
	next = strchr( filename, '/' );
	while( next != NULL ) {
		switch( ext2fs_lookup( e2fs, e2dir, filename, next-filename, NULL, &e2dir ) ) {
			case 0:
				filename = next+1;
				next = strchr( filename, '/' );
				break;
			case EXT2_ET_FILE_NOT_FOUND:
				TT_DEBUG2( "plugin/default", "directory '%s' not found on EXT2 filesystem '%s'", filename, device );
				ext2fs_close( e2fs );
				return TT_ERR;
			default:
				TT_LOG_ERROR( "plugin/default", "error while looking for '%s' on EXT2 filesystem '%s'", filename, device );
				ext2fs_close( e2fs );
				return TT_ERR;
		}
	}
	if( ext2fs_lookup( e2fs, e2dir, filename, strnlen(filename, FILENAME_MAX), NULL, &e2file ) == EXT2_ET_FILE_NOT_FOUND ) {
		TT_DEBUG2( "plugin/default", "file '%s' not found on EXT2 filesystem '%s'", filename, device );
		ext2fs_close( e2fs );
		return TT_ERR;
	}
	if( ext2fs_file_open( e2fs, e2file, 0, &target_file ) != 0 ) {
		TT_LOG_ERROR( "plugin/default", "failed to load '%s' from EXT2 filesystem ('%s')", filename, device );
		ext2fs_close( e2fs );
		return TT_ERR;
	}
	if( ext2fs_file_read( target_file, buffer, *buffer_size, (unsigned int*)buffer_size ) != 0 ) {
		TT_LOG_ERROR( "plugin/default", "failed to load '%s' from EXT2 filesystem ('%s')", filename, device );
		ext2fs_file_close( target_file );
		ext2fs_close( e2fs );
		return TT_ERR;
	}
	ext2fs_file_close( target_file );
	ext2fs_close( e2fs );
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int default__ext2fs_save(const char* filename, const char* buffer, const size_t buffer_size) {
	char		device[FILENAME_MAX+1] = {0};
	char		username[TT_USERNAME_CHAR_MAX+1] = {0};
	size_t		username_size = sizeof(username);
	char		groupname[TT_USERNAME_CHAR_MAX+1] = {0};
	size_t		groupname_size = sizeof(groupname);
	char		permission[4] = {0};
	size_t		permission_size = sizeof(permission);
	struct passwd*	owner = NULL;
	struct group*	group = NULL;
	mode_t		mode = 0;
	ext2_filsys	e2fs;
	ext2_ino_t	e2dir;
	ext2_ino_t	e2file;
	struct ext2_inode target_inode;
	ext2_file_t	target_file;
	const char*	next;

	TT_TRACE( "plugin/default", "%s(filename='%s',buffer=%p,buffer_size=%zd)", __FUNCTION__, filename, buffer, buffer_size );
	if( filename == NULL || buffer == NULL || buffer_size == 0 ) {
        	TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( libtokentube_runtime_get_bootdevice( device, sizeof(device) ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "internal error in %s at %d", __FILE__, __LINE__ );
		return TT_ERR;
	}
	if( libtokentube_conf_read_str( "storage|files|owner", username, &username_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "internal error in %s at %d", __FILE__, __LINE__ );
		return TT_ERR;
	}
	if( libtokentube_conf_read_str( "storage|files|group", groupname, &groupname_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "internal error in %s at %d", __FILE__, __LINE__ );
		return TT_ERR;
	}
	if( libtokentube_conf_read_str( "storage|files|permission", permission, &permission_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "internal error in %s at %d", __FILE__, __LINE__ );
		return TT_ERR;
	}
	owner = getpwnam( username );
	if( owner == NULL ) {
		TT_LOG_ERROR( "plugin/default", "internal error in %s at %d", __FILE__, __LINE__ );
		return TT_ERR;
	}
	group = getgrnam( groupname );
	if( owner == NULL ) {
		TT_LOG_ERROR( "plugin/default", "internal error in %s at %d", __FILE__, __LINE__ );
		return TT_ERR;
	}
	mode = strtol( permission, NULL, 8 );
	if( mode == 0 ) {
		TT_LOG_WARN( "plugin/default", "internal error in %s at %d", __FILE__, __LINE__ );
		mode = S_IRWXU|S_IRGRP|S_IXGRP;
	}

	TT_DEBUG2( "plugin/default", "opening '%s' as EXT2 filesystem in %s()", device, __FUNCTION__ );
	if( ext2fs_open( device, EXT2_FLAG_RW|EXT2_FLAG_FORCE, 0, 0, unix_io_manager, &e2fs ) != 0 ) {
		TT_LOG_ERROR( "plugin/default", "failed to open '%s' as EXT2 filesystem", device );
		return TT_ERR;
	}
	if( ext2fs_read_bitmaps( e2fs ) != 0 ) {
		TT_LOG_ERROR( "plugin/default", "internal error in %s at %d", __FILE__, __LINE__ );
		ext2fs_close( e2fs );
		return TT_ERR;
	}

	TT_DEBUG2( "plugin/default", "invoking ext2fs_lookup() for '%s' in %s()", filename, __FUNCTION__ );
	e2dir = EXT2_ROOT_INO;
	next = strchr( filename, '/' );
	while( next != NULL ) {
		switch( ext2fs_lookup( e2fs, e2dir, filename, next-filename, NULL, &e2dir ) ) {
			case 0:
				filename = next+1;
				next = strchr( filename, '/' );
				break;
			case EXT2_ET_FILE_NOT_FOUND:
				TT_DEBUG2( "plugin/default", "directory '%s' not found on EXT2 filesystem '%s'", filename, device );
				ext2fs_close( e2fs );
				return TT_ERR;
			default:
				TT_LOG_ERROR( "plugin/default", "error while looking for '%s' on EXT2 filesystem '%s'", filename, device );
				ext2fs_close( e2fs );
				return TT_ERR;
		}
	}
	if( ext2fs_lookup( e2fs, e2dir, filename, strnlen(filename, FILENAME_MAX), NULL, &e2file ) == EXT2_ET_FILE_NOT_FOUND ) {
		TT_DEBUG2( "plugin/default", "file-not-found, creating new inode&file entries in %s()", __FUNCTION__ );
		memset(&target_inode, 0, sizeof(target_inode));
		target_inode.i_uid = owner->pw_uid;
		target_inode.i_gid = group->gr_gid;
		target_inode.i_mode = LINUX_S_IFREG | (mode & 0777);
		target_inode.i_links_count = 1;
		target_inode.i_size = buffer_size;

		if( ext2fs_new_inode(e2fs, e2dir, S_IFREG|S_IRWXU, 0, &e2file) != 0 ) {
			TT_LOG_ERROR( "plugin/default", "error while creating file '%s' on EXT2 filesystem '%s'", filename, device );
			ext2fs_close( e2fs );
			return TT_ERR;
		}
		if( ext2fs_write_new_inode(e2fs, e2file, &target_inode) != 0 ) {
			TT_LOG_ERROR( "plugin/default", "error while creating file '%s' on EXT2 filesystem '%s'", filename, device );
			ext2fs_close( e2fs );
			return TT_ERR;
		}
		ext2fs_inode_alloc_stats2(e2fs, e2file, 1, 1);
		if( ext2fs_file_open( e2fs, e2file, EXT2_FILE_CREATE|EXT2_FILE_WRITE, &target_file ) != 0 ) {
			TT_LOG_ERROR( "plugin/default", "error while creating file '%s' on EXT2 filesystem '%s'", filename, device );
			ext2fs_close( e2fs );
			return TT_ERR;
		}
		if( ext2fs_link( e2fs, e2dir, filename, e2file, EXT2_FT_REG_FILE ) != 0 ) {
			TT_LOG_ERROR( "plugin/default", "error while creating file '%s' on EXT2 filesystem '%s'", filename, device );
			ext2fs_file_close( target_file );
			ext2fs_close( e2fs );
			return TT_ERR;
		}
	} else {
		if( ext2fs_file_open( e2fs, e2file, EXT2_FILE_WRITE, &target_file ) != 0 ) {
			TT_LOG_ERROR( "plugin/default", "error while opening file '%s' on EXT2 filesystem '%s'", filename, device );
			ext2fs_close( e2fs );
			return TT_ERR;
		}
	}
	if( ext2fs_file_write( target_file, buffer, buffer_size, (unsigned int*)&buffer_size ) != 0 ) {
		TT_LOG_ERROR( "plugin/default", "error while saving file '%s' on EXT2 filesystem '%s'", filename, device );
		ext2fs_file_close( target_file );
		ext2fs_close( e2fs );
		return TT_ERR;
	}
	if( ext2fs_file_flush( target_file ) != 0 ) {
		TT_LOG_ERROR( "plugin/default", "error while flushing file '%s' on EXT2 filesystem '%s'", filename, device );
		ext2fs_file_close( target_file );
		ext2fs_close( e2fs );
		return TT_ERR;
	}
	ext2fs_file_close( target_file );
	ext2fs_write_bitmaps( e2fs );
	ext2fs_close( e2fs );
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int default__ext2fs_exists(const char* filename, tt_status_t* status) {
	char		device[FILENAME_MAX+1] = {0};
	ext2_filsys	e2fs;
	ext2_ino_t	e2dir;
	ext2_ino_t	e2file;
	const char*	next;

	TT_TRACE( "plugin/default", "%s(filename='%s',status=%p)", __FUNCTION__, filename, status );
	if( filename == NULL || status == NULL ) {
        	TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( libtokentube_runtime_get_bootdevice( device, sizeof(device) ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "internal error in %s at %d", __FILE__, __LINE__ );
		return TT_ERR;
	}

	TT_DEBUG2( "plugin/default", "opening '%s' as EXT2 filesystem", device );
	if( ext2fs_open( device, EXT2_FLAG_FORCE, 0, 0, unix_io_manager, &e2fs ) != 0 ) {
		TT_LOG_ERROR( "plugin/default", "failed to open '%s' as EXT2 filesystem", device );
		return TT_ERR;
	}

	TT_DEBUG2( "plugin/default", "invoking ext2fs_lookup() for '%s' in %s()", filename, __FUNCTION__ );
	e2dir = EXT2_ROOT_INO;
	next = strchr( filename, '/' );
	while( next != NULL ) {
		switch( ext2fs_lookup( e2fs, e2dir, filename, next-filename, NULL, &e2dir ) ) {
			case 0:
				filename = next+1;
				next = strchr( filename, '/' );
				break;
			case EXT2_ET_FILE_NOT_FOUND:
				TT_DEBUG2( "plugin/default", "directory '%s' not found on EXT2 filesystem '%s'", filename, device );
				ext2fs_close( e2fs );
				*status = TT_NO;
				return TT_OK;
			default:
				TT_LOG_ERROR( "plugin/default", "error while looking for '%s' on EXT2 filesystem '%s'", filename, device );
				ext2fs_close( e2fs );
				return TT_ERR;
		}
	}
	if( ext2fs_lookup( e2fs, e2dir, filename, strnlen(filename, FILENAME_MAX), NULL, &e2file ) == EXT2_ET_FILE_NOT_FOUND ) {
		TT_DEBUG1( "plugin/default", "file '%s' not found on EXT2 filesystem '%s'", filename, device );
		ext2fs_close( e2fs );
		*status = TT_NO;
		return TT_OK;
	}
	*status = TT_YES;
	ext2fs_close( e2fs );
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int default__ext2fs_delete(const char* filename, tt_status_t* status) {
	char		device[FILENAME_MAX+1] = {0};
	ext2_filsys	e2fs;
	ext2_ino_t	e2dir;
	ext2_ino_t	e2file;
	const char*	next;

	TT_TRACE( "plugin/default", "%s(filename='%s',status=%p)", __FUNCTION__, filename, status );
	if( filename == NULL || status == NULL ) {
        	TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( libtokentube_runtime_get_bootdevice( device, sizeof(device) ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "internal error in %s at %d", __FILE__, __LINE__ );
		return TT_ERR;
	}

	TT_DEBUG2( "plugin/default", "opening '%s' as EXT2 filesystem", device );
	if( ext2fs_open( device, EXT2_FLAG_RW|EXT2_FLAG_FORCE, 0, 0, unix_io_manager, &e2fs ) != 0 ) {
		TT_LOG_ERROR( "plugin/default", "failed to open '%s' as EXT2 filesystem", device );
		return TT_ERR;
	}

	TT_DEBUG2( "plugin/default", "invoking ext2fs_lookup() for '%s' in %s()", filename, __FUNCTION__ );
	e2dir = EXT2_ROOT_INO;
	next = strchr( filename, '/' );
	while( next != NULL ) {
		switch( ext2fs_lookup( e2fs, e2dir, filename, next-filename, NULL, &e2dir ) ) {
			case 0:
				filename = next+1;
				next = strchr( filename, '/' );
				break;
			case EXT2_ET_FILE_NOT_FOUND:
				*status = TT_NO;
				ext2fs_close( e2fs );
				return TT_OK;
			default:
				TT_LOG_ERROR( "plugin/default", "failed to look for '%s' on EXT2 filesystem '%s'", filename, device );
				ext2fs_close( e2fs );
				return TT_ERR;
		}
	}
	if( ext2fs_lookup( e2fs, e2dir, filename, strnlen(filename, FILENAME_MAX), NULL, &e2file ) == EXT2_ET_FILE_NOT_FOUND ) {
		TT_DEBUG1( "plugin/default", "file '%s' not found on EXT2 filesystem '%s'", filename, device );
		ext2fs_close( e2fs );
		*status = TT_NO;
		return TT_OK;
	}
	if( ext2fs_unlink( e2fs, e2dir, filename, 0, 0 ) != 0 ) {
		TT_LOG_ERROR( "plugin/default", "failed to look for '%s' on EXT2 filesystem '%s'", filename, device );
		ext2fs_close( e2fs );
		return TT_ERR;
	}
	*status = TT_YES;	
	ext2fs_close( e2fs );
	return TT_OK;
}

