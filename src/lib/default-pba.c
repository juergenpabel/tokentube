#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <libgen.h>
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
int default__api__pba_install_pre(const char* type, const char* path) {
        char            filename[FILENAME_MAX+1] = {0};
	size_t		filename_size = sizeof(filename);
	struct stat	st;
	int		fd = -1;

	TT_TRACE( "library/conf", "%s(type='%s',path='%s')", __FUNCTION__, type, path );
	if( type == NULL || type[0] == '\0' || path == NULL || path[0] == '\0' ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( strncasecmp( TT_PBA__INITRD, type, strlen(TT_PBA__INITRD) ) != 0 && strncasecmp( TT_PBA__INITRAMFS, type, strlen(TT_PBA__INITRAMFS) ) != 0 ) {
		TT_LOG_ERROR( "plugin/default", "unsupported PBA-type in %s()", __FUNCTION__ );
		return TT_OK;
	}
	snprintf( filename, sizeof(filename)-1, "%s/etc/tokentube", path );
	if( libtokentube_util_posix_mkdir( filename ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "library->posix_mkdir('%s') failed in __FUNCTION__()", filename );
		return TT_ERR;
	}
	snprintf( filename, sizeof(filename)-1, "%s/usr/lib/tokentube/plugins.d", path );
	if( libtokentube_util_posix_mkdir( filename ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "library->posix_mkdir('%s') failed in __FUNCTION__()", filename );
		return TT_ERR;
	}
	snprintf( filename, sizeof(filename)-1, "%s/etc/tokentube/conf.d", path );
	if( libtokentube_util_posix_mkdir( filename ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "library->posix_mkdir('%s') failed in __FUNCTION__()", filename );
		return TT_ERR;
	}
	snprintf( filename, sizeof(filename)-1, "%s%s", path, TT_FILENAME__BOOT_CONF );
	fd = open( filename, O_CREAT|O_TRUNC|O_WRONLY, S_IRUSR|S_IWUSR );
	if( fd < 0 ) {
		TT_LOG_ERROR( "plugin/default", "open(O_CREAT|O_TRUNC|O_WRONLY) failed for '%s' in %s()", filename, __FUNCTION__ );
		return TT_ERR;
	}
	if( stat( "/boot/tokentube", &st ) < 0 ) {
		TT_LOG_ERROR( "plugin/default", "stat('/boot/tokentube') failed in %s()", __FUNCTION__ );
		close( fd );
		return TT_ERR;
	}
	if( libtokentube_util_device_find( st.st_dev, filename, &filename_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_util_device_find('%d') failed in %s()", st.st_dev, __FUNCTION__ );
		close( fd );
		return TT_ERR;
	}
	if( filename_size == 0 ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_util_device_find('%d') found no device in %s()", st.st_dev, __FUNCTION__ );
		close( fd );
		return TT_ERR;
	}
	if( write( fd, filename, filename_size ) < (int)filename_size ) {
		TT_LOG_ERROR( "plugin/default", "write() failed in %s()", __FUNCTION__ );
		close( fd );
		return TT_ERR;
	}
	close( fd );
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int default__api__pba_install(const char* type, const char* path) {
	char            buffer[FILENAME_MAX+1] = {0};
	size_t          buffer_size = sizeof(buffer);
	char            filename_source[FILENAME_MAX+1] = {0};
	size_t          filename_source_size = sizeof(filename_source);
	char            filename_target[FILENAME_MAX+1] = {0};
	tt_module_t*	module = NULL;
	int             fd, i = 0;

	TT_TRACE( "library/conf", "%s(type='%s',path='%s')", __FUNCTION__, type, path );
	if( type == NULL || type[0] == '\0' || path == NULL || path[0] == '\0' ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( strncasecmp( TT_PBA__INITRD, type, strlen(TT_PBA__INITRD) ) != 0 && strncasecmp( TT_PBA__INITRAMFS, type, strlen(TT_PBA__INITRAMFS) ) != 0 ) {
		TT_LOG_ERROR( "plugin/default", "unsupported PBA-type in %s()", __FUNCTION__ );
		return TT_IGN;
	}
	if( libtokentube_conf_print( buffer, &buffer_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_conf_print() failed for '%s' in %()", filename_target, __FUNCTION__ );
		return TT_ERR;
	}
	snprintf( filename_target, sizeof(filename_target)-1, "%s%s", path, TT_FILENAME__TOKENTUBE_CONF );
	fd = open( filename_target, O_CREAT|O_TRUNC|O_WRONLY, S_IRUSR|S_IWUSR );
	if( fd < 0 ) {
		TT_LOG_ERROR( "plugin/default", "open(O_CREAT|O_TRUNC|O_WRONLY) failed for '%s' in %s()", filename_target, __FUNCTION__ );
		return TT_ERR;
	}
	if( write( fd, buffer, buffer_size ) != (ssize_t)buffer_size ) {
		TT_LOG_ERROR( "plugin/default", "write() failed for '%s' in %s()", filename_target, __FUNCTION__ );
		close( fd );
		return TT_ERR;
	}
	close( fd );

	TT_DEBUG2( "plugin/default", "iterating over plugins in %s()", __FUNCTION__ );
	for( i=0; i<MAX_PLUGINS; i++ ) {
		module = g_modules[i];
		if( module != NULL && module->plugin != NULL ) {
			TT_DEBUG3( "plugin/default", "processing plugin '%s' in %s()", module->name, __FUNCTION__ );

			filename_source_size = sizeof(filename_source);
			if( libtokentube_conf_read_plugin_library( module->name, filename_source, &filename_source_size ) != TT_OK ) {
				TT_LOG_ERROR( "plugin/default", "internal error in %s at %d", __FILE__, __LINE__ );
				return TT_ERR;
			}
			if( filename_source_size > 0 ) {
				snprintf( filename_target, sizeof(filename_target)-1, "%s%s", path, filename_source );
				if( libtokentube_util_posix_mkdir( dirname( filename_target ) ) != TT_OK ) {
					TT_LOG_ERROR( "plugin/default", "posix_mkdir() failed for '%s' in %s()", filename_target, __FUNCTION__ );
					return TT_ERR;
				}
				snprintf( filename_target, sizeof(filename_target)-1, "%s%s", path, filename_source );
				if( libtokentube_util_posix_copy( filename_source, filename_target ) != TT_OK ) {
					TT_LOG_ERROR( "plugin/default", "posix_copy() failed for '%s' in %s()", filename_source, __FUNCTION__ );
					return TT_ERR;
				}
			}

			filename_source_size = sizeof(filename_source);
			if( libtokentube_conf_read_plugin_config( module->name, filename_source, &filename_source_size ) != TT_OK ) {
				TT_LOG_ERROR( "plugin/default", "internal error in %s at %d", __FILE__, __LINE__ );
				return TT_ERR;
			}
			if( filename_source_size > 0 ) {
				snprintf( filename_target, sizeof(filename_target)-1, "%s%s", path, filename_source );
				if( libtokentube_util_posix_mkdir( dirname( filename_target ) ) != TT_OK ) {
					TT_LOG_ERROR( "plugin/default", "mkdir() failed for '%s' in %s()", filename_target, __FUNCTION__ );
					return TT_ERR;
				}
				snprintf( filename_target, sizeof(filename_target)-1, "%s%s", path, filename_source );
				if( libtokentube_util_posix_copy( filename_source, filename_target ) != TT_OK ) {
					TT_LOG_ERROR( "plugin/default", "posix_copy() failed for '%s' in %s()", filename_source, __FUNCTION__ );
					return TT_ERR;
				}
			}
		}
	}
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int default__api__pba_install_post(const char* type, const char* path) {

	TT_TRACE( "library/conf", "%s(type='%s',path='%s')", __FUNCTION__, type, path );
	if( type == NULL || type[0] == '\0' || path == NULL || path[0] == '\0' ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
//TODO:sanity checks
	return TT_OK;
}

