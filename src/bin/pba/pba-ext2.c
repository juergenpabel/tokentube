#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>
#include <ext2fs/ext2fs.h>
//include <tokentube.h>


int pba_ext2_readfile(const char* device, const char* filename, char* buffer, size_t* buffer_size) {
	ext2_filsys	e2fs;
	ext2_ino_t	e2ino;
	ext2_file_t	e2file;
	const char*	next;
	int		err;

	if( ext2fs_open( device, EXT2_FLAG_FORCE, 0, 0, unix_io_manager, &e2fs ) != 0 ) {
		printf( "ERROR: could not open %s as EXT2 filesystem\n", device );
		return TT_ERR;
	}
	e2ino = EXT2_ROOT_INO;
	next = strchr( filename, '/' );
	while( next != NULL ) {
		if( ext2fs_lookup( e2fs, e2ino, filename, next-filename, NULL, &e2ino ) != 0 ) {
			ext2fs_close( e2fs );
			return TT_ERR;
		}
		filename = next+1;
		next = strchr( filename, '/' );
	}
	if( ext2fs_lookup( e2fs, e2ino, filename, strnlen(filename, FILENAME_MAX), NULL, &e2ino ) != 0 ) {
		ext2fs_close( e2fs );
		return TT_ERR;
	}
	if( ext2fs_file_open( e2fs, e2ino, 0, &e2file ) != 0 ) {
		ext2fs_close( e2fs );
		return TT_ERR;
	}
	if( ext2fs_file_read( e2file, buffer, *buffer_size, buffer_size ) != 0 ) {
		ext2fs_close( e2fs );
		return TT_ERR;
	}
	if( ext2fs_file_close( e2file ) != 0 ) {
		ext2fs_close( e2fs );
		return TT_ERR;
	}
	ext2fs_close( e2fs );
	return TT_OK;
}


int pba_ext2_writefile(const char* device, const char* filename, char* buffer, size_t* buffer_size) {
	ext2_filsys	e2fs;
	ext2_ino_t	e2ino;
	ext2_file_t	e2file;
	const char*	next;
	int		err;

	if( ext2fs_open( device, EXT2_FLAG_FORCE, 0, 0, unix_io_manager, &e2fs ) != 0 ) {
		printf( "ERROR: could not open %s as EXT2 filesystem\n", device );
		return TT_ERR;
	}
	e2ino = EXT2_ROOT_INO;
	next = strchr( filename, '/' );
	while( next != NULL ) {
		if( ext2fs_lookup( e2fs, e2ino, filename, next-filename, NULL, &e2ino ) != 0 ) {
			ext2fs_close( e2fs );
			return TT_ERR;
		}
		filename = next+1;
		next = strchr( filename, '/' );
	}
	if( ext2fs_lookup( e2fs, e2ino, filename, strnlen(filename, FILENAME_MAX), NULL, &e2ino ) != 0 ) {
		ext2fs_close( e2fs );
		return TT_ERR;
	}
	if( ext2fs_file_open( e2fs, e2ino, 0, &e2file ) != 0 ) {
		ext2fs_close( e2fs );
		return TT_ERR;
	}
	if( ext2fs_file_write( e2file, buffer, *buffer_size, buffer_size ) != 0 ) {
		ext2fs_close( e2fs );
		return TT_ERR;
	}
	if( ext2fs_file_close( e2file ) != 0 ) {
		ext2fs_close( e2fs );
		return TT_ERR;
	}
	ext2fs_close( e2fs );
	return TT_OK;
}

