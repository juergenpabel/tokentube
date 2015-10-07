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


typedef struct {
	const char*	path;
	char*		buffer;
	size_t*		buffer_size;
} default_ext2_dir_t;


static int default__storage_ext2fs_dir_iter(struct ext2_dir_entry* de, int offset, int blocksize, char* buffer, void* priv_data) {
	default_ext2_dir_t*	data = (default_ext2_dir_t*)priv_data;
	size_t			buffer_offset = 0;

	(void)offset;
	(void)blocksize;
	(void)buffer;
	if( de->name[0] == '.' ) {
		return 0;
	}
	buffer_offset = strnlen( data->buffer, *data->buffer_size );
	snprintf( data->buffer+buffer_offset, *data->buffer_size-buffer_offset, "/boot/%s/%.*s\n", data->path, de->name_len&0xff, de->name );
	return 0;
}


__attribute__ ((visibility ("hidden")))
int default__storage_ext2fs_load(tt_file_t type, const char* identifier, const char* filename, char* buffer, size_t* buffer_size) {
	char			device[FILENAME_MAX+1] = {0};
	ext2_filsys		e2fs;
	ext2_ino_t		e2dir;
	ext2_ino_t		e2file;
	ext2_file_t		target_file;
	struct ext2_inode	inode;
	default_ext2_dir_t	dir_data;
	const char*		name;
	const char*		next;

	TT_TRACE( "plugin/default", "%s(type=%zd,identifier='%s',filename='%s',buffer=%p,buffer_size=%p)", __FUNCTION__, type, identifier, filename, buffer, buffer_size );
	if( type == TT_FILE__UNDEFINED || identifier == NULL || filename == NULL || buffer == NULL || buffer_size == NULL || *buffer_size == 0 ) {
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
	name = filename;
	next = strchr( name, '/' );
	while( next != NULL ) {
		switch( ext2fs_lookup( e2fs, e2dir, name, next-name, NULL, &e2dir ) ) {
			case 0:
				name = next+1;
				next = strchr( name, '/' );
				break;
			case EXT2_ET_FILE_NOT_FOUND:
				TT_DEBUG2( "plugin/default", "directory '%s' not found on EXT2 filesystem '%.*s'", name-filename, filename, device );
				ext2fs_close( e2fs );
				return TT_ERR;
			default:
				TT_LOG_ERROR( "plugin/default", "error while looking for '%s' on EXT2 filesystem '%s'", filename, device );
				ext2fs_close( e2fs );
				return TT_ERR;
		}
	}
	if( ext2fs_lookup( e2fs, e2dir, name, strnlen(name, FILENAME_MAX), NULL, &e2file ) == EXT2_ET_FILE_NOT_FOUND ) {
		TT_DEBUG2( "plugin/default", "file '%s' not found on EXT2 filesystem '%s'", filename, device );
		ext2fs_close( e2fs );
		return TT_ERR;
	}
	if( ext2fs_read_inode( e2fs, e2file, &inode ) != 0) {
		TT_DEBUG2( "plugin/default", "file '%s' failed to load inode on EXT2 filesystem '%s'", filename, device );
		ext2fs_close( e2fs );
		return TT_ERR;
	}
	switch( inode.i_mode & LINUX_S_IFMT ) {
		case LINUX_S_IFREG:
			if( ext2fs_file_open( e2fs, e2file, 0, &target_file ) != 0 ) {
				TT_LOG_ERROR( "plugin/default", "failed to open '%s' on EXT2 filesystem ('%s')", filename, device );
				ext2fs_close( e2fs );
				return TT_ERR;
			}
			if( ext2fs_file_read( target_file, buffer, *buffer_size, (unsigned int*)buffer_size ) != 0 ) {
				TT_LOG_ERROR( "plugin/default", "failed to read '%s' on EXT2 filesystem ('%s')", filename, device );
				ext2fs_file_close( target_file );
				ext2fs_close( e2fs );
				return TT_ERR;
			}
			ext2fs_file_close( target_file );
			break;
		case LINUX_S_IFDIR:
			dir_data.path = filename;
			dir_data.buffer = buffer;
			dir_data.buffer_size = buffer_size;
			if( ext2fs_dir_iterate( e2fs, e2file, 0, NULL, default__storage_ext2fs_dir_iter, &dir_data ) != 0 ) {
				TT_LOG_ERROR( "plugin/default", "failed to iterate dir '%s' on EXT2 filesystem ('%s')", filename, device );
				ext2fs_close( e2fs );
				return TT_ERR;
			}
			*buffer_size = strnlen( buffer, *buffer_size );
			break;
		default:
			TT_DEBUG2( "plugin/default", "file '%s' not a regular file or directory on EXT2 filesystem '%s'", filename, device );
			ext2fs_close( e2fs );
			return TT_ERR;
	}
	ext2fs_close( e2fs );
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int default__storage_ext2fs_save(tt_file_t type, const char* identifier, const char* filename, const char* buffer, const size_t buffer_size) {
	char		device[FILENAME_MAX+1] = {0};
	char		username[TT_USERNAME_CHAR_MAX+1] = {0};
	size_t		username_size = sizeof(username);
	char		groupname[TT_USERNAME_CHAR_MAX+1] = {0};
	size_t		groupname_size = sizeof(groupname);
	char		perm[4] = {0};
	size_t		perm_size = sizeof(perm);
	const char*	conf_owner = NULL;
	const char*	conf_group = NULL;
	const char*	conf_perm = NULL;
	struct passwd*	owner = NULL;
	struct group*	group = NULL;
	mode_t		mode = 0;
	ext2_filsys	e2fs;
	ext2_ino_t	e2dir;
	ext2_ino_t	e2file;
	struct ext2_inode target_inode;
	ext2_file_t	target_file;
	const char*	next;

	TT_TRACE( "plugin/default", "%s(type=%zd,identifier='%s',filename='%s',buffer=%p,buffer_size=%zd)", __FUNCTION__, type, identifier, filename, buffer, buffer_size );
	if( type == TT_FILE__UNDEFINED || identifier == NULL || filename == NULL || buffer == NULL || buffer_size == 0 ) {
        	TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( libtokentube_runtime_get_bootdevice( device, sizeof(device) ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "internal error in %s at %d", __FILE__, __LINE__ );
		return TT_ERR;
	}
	switch( type ) {
		case TT_FILE__USER:
			conf_owner = "storage|user-files|owner";
			conf_group = "storage|user-files|group";
			conf_perm  = "storage|user-files|perm";
			break;
		case TT_FILE__OTP:
			conf_owner = "storage|otp-files|owner";
			conf_group = "storage|otp-files|group";
			conf_perm  = "storage|otp-files|perm";
			break;
		default:
			break;
	}
	if( libtokentube_conf_read_str( conf_owner, username, &username_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "internal error in %s at %d", __FILE__, __LINE__ );
		return TT_ERR;
	}
	if( strncasecmp( username, "$USER", sizeof(username) ) == 0 ) {
		strncpy( username, identifier, sizeof(username) );
		username_size = strnlen( username, sizeof(username) );
	}
	if( libtokentube_conf_read_str( conf_group, groupname, &groupname_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "internal error in %s at %d", __FILE__, __LINE__ );
		return TT_ERR;
	}
	if( libtokentube_conf_read_str( conf_perm, perm, &perm_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "internal error in %s at %d", __FILE__, __LINE__ );
		return TT_ERR;
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
		owner = getpwnam( username );
		group = getgrnam( groupname );
		mode = strtol( perm, NULL, 8 );
		if( owner != NULL )  {
			target_inode.i_uid = owner->pw_uid;
		}
		if( group != NULL )  {
			target_inode.i_gid = group->gr_gid;
		}
		if( mode == 0 ) {
			mode = umask( 0 );
			umask( mode );
			mode = ( S_IRWXU | S_IRWXG | S_IRWXO )  ^ mode;
		}

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
	if( ext2fs_file_set_size( target_file, buffer_size ) != 0 ) {
		TT_LOG_ERROR( "plugin/default", "error while setting size for file '%s' on EXT2 filesystem '%s'", filename, device );
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
int default__storage_ext2fs_exists(tt_file_t type, const char* identifier, const char* filename, tt_status_t* status) {
	char		device[FILENAME_MAX+1] = {0};
	ext2_filsys	e2fs;
	ext2_ino_t	e2dir;
	ext2_ino_t	e2file;
	const char*	next;

	TT_TRACE( "plugin/default", "%s(type=%zd,identifier='%s',filename='%s',status=%p)", __FUNCTION__, type, identifier, filename, status );
	if( type == TT_FILE__UNDEFINED || identifier == NULL || filename == NULL || status == NULL ) {
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
int default__storage_ext2fs_delete(tt_file_t type, const char* identifier, const char* filename, tt_status_t* status) {
	char		device[FILENAME_MAX+1] = {0};
	ext2_filsys	e2fs;
	ext2_ino_t	e2dir;
	ext2_ino_t	e2file;
	const char*	next;

	TT_TRACE( "plugin/default", "%s(type=%zd,idenfifier='%s',filename='%s',status=%p)", __FUNCTION__, type, identifier, filename, status );
	if( type == TT_FILE__UNDEFINED || identifier == NULL || filename == NULL || status == NULL ) {
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

