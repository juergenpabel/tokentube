#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>
#include <ext2fs/ext2fs.h>
#include <tokentube.h>


int pba_ext2_readfile(const char* device, const char* filename, char* buf, unsigned int bufsize) {
    ext2_filsys e2fs;
    ext2_ino_t  e2ino;
    ext2_file_t e2file;
    const char* next;
    int err;

    err = ext2fs_open( device, EXT2_FLAG_FORCE, 0, 0, unix_io_manager, &e2fs );
    if( err != 0 ) {
        printf( "ERROR: could not open %s as EXT2 filesystem\n", device );
        exit(err);
    }

    e2ino = EXT2_ROOT_INO;
    next = strchr( filename, '/' );
    while( err == 0 && next != NULL ) {
        err = ext2fs_lookup( e2fs, e2ino, filename, next-filename, NULL, &e2ino );
        filename = next+1;
        next = strchr( filename, '/' );
    }
    if(err == 0) {
        err = ext2fs_lookup( e2fs, e2ino, filename, strlen(filename), NULL, &e2ino );
    }
    if(err == 0) {
        err = ext2fs_file_open( e2fs, e2ino, 0, &e2file );
    }
    if(err == 0) {
        err = ext2fs_file_read( e2file, buf, bufsize, &bufsize );
    }
    if(err == 0) {
        err = ext2fs_file_close( e2file );
    }
    ext2fs_close( e2fs );
    if(err == 0) {
        return (int)bufsize;
    }
    return err;
}


int pba_ext2_writefile(const char* device, const char* filename, char* buf, unsigned int bufsize) {
    ext2_filsys e2fs;
    ext2_ino_t  e2ino;
    ext2_file_t e2file;
    const char* next;
    int err;

    err = ext2fs_open( device, EXT2_FLAG_FORCE, 0, 0, unix_io_manager, &e2fs );
    if( err != 0 ) {
        printf( "ERROR: could not open %s as EXT2 filesystem\n", device );
        exit(err);
    }

    e2ino = EXT2_ROOT_INO;
    next = strchr( filename, '/' );
    while( err == 0 && next != NULL ) {
        err = ext2fs_lookup( e2fs, e2ino, filename, next-filename, NULL, &e2ino );
        filename = next+1;
        next = strchr( filename, '/' );
    }
    if(err == 0) {
        err = ext2fs_lookup( e2fs, e2ino, filename, strlen(filename), NULL, &e2ino );
    }
    if(err == 0) {
        err = ext2fs_file_open( e2fs, e2ino, 0, &e2file );
    }
    if(err == 0) {
        err = ext2fs_file_write( e2file, buf, bufsize, &bufsize );
    }
    if(err == 0) {
        err = ext2fs_file_close( e2file );
    }
    ext2fs_close( e2fs );
    if(err == 0) {
        return (int)bufsize;
    }
    return err;
}
