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
int default__impl__storage_get_filename(tt_file_t file, const char* identifier, char* buffer, const size_t buffer_size) {
	char	     directory[FILENAME_MAX+1] = { 0 };
	size_t       directory_size = sizeof(directory);
	char         filehash[TT_IDENTIFIER_CHAR_MAX+1] = {0};
	size_t       filehash_size = sizeof(filehash);
	char	     uuid[TT_DIGEST_BITS_MAX/8*2+1] = { 0 };
	size_t       uuid_size = sizeof(uuid);
	const char*  conf_directory = NULL;
	const char*  conf_filehash = NULL;
	const char*  filename = NULL;

	TT_TRACE( "library/plugin", "%s(file=%d,identifier='%s',buffer=%p, buffer_size=%zd)", __FUNCTION__, file, identifier, buffer, buffer_size );
	if( identifier == NULL || buffer == NULL || buffer_size == 0 ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	TT_DEBUG1( "plugin/default", "processing '%s' in %s()", identifier, __FUNCTION__ );
	memset( buffer, '\0', buffer_size );
	if( file == TT_FILE__CONFIG_STANDARD || file == TT_FILE__CONFIG_PBA ) {
		strncpy( buffer, identifier, buffer_size-1 );
		return TT_OK;
	}
	switch( file ) {
		case TT_FILE__KEY:
			conf_directory = "storage|key-files|directory";
			conf_filehash  = "storage|key-files|filename-hash";
			break;
		case TT_FILE__USER:
			conf_directory = "storage|user-files|directory";
			conf_filehash  = "storage|user-files|filename-hash";
			break;
		case TT_FILE__OTP:
			conf_directory = "storage|otp-files|directory";
			conf_filehash  = "storage|otp-files|filename-hash";
			break;
		case TT_FILE__UHD:
			conf_directory = "storage|helpdesk-files|directory";
			conf_filehash  = "storage|helpdesk-files|filename-hash";
			break;
		default:
			TT_LOG_ERROR( "plugin/default", "invalid file=%d in %s()", file, __FUNCTION__ );
			return TT_ERR;
	}
	if( libtokentube_conf_read_str( conf_directory, directory, &directory_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "reading %s failed in %s()", conf_directory, __FUNCTION__ );
		return TT_ERR;
	}
	if( libtokentube_conf_read_str( conf_filehash, filehash, &filehash_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "reading %s failed in %s()", conf_filehash, __FUNCTION__ );
		return TT_ERR;
	}
	snprintf( buffer, buffer_size-1, "%s/", directory );
	filename = identifier;
	if( strncasecmp( filehash, "null", 5 ) != 0 ) {
		if( libtokentube_crypto_hash_impl( libtokentube_name2oid( filehash ), identifier, strnlen( identifier, TT_IDENTIFIER_CHAR_MAX ), uuid, &uuid_size ) != TT_OK ) {
			TT_LOG_ERROR( "plugin/default", "internal error in %s at %d", __FILE__, __LINE__ );
			return TT_ERR;
		}
		if( libtokentube_util_hex_encode( uuid, uuid_size, uuid, &uuid_size ) != TT_OK ) {
			TT_LOG_ERROR( "plugin/default", "internal error in %s at %d", __FILE__, __LINE__ );
			return TT_ERR;
		}
		filename = uuid;
	}
	if( directory_size + strnlen( filename, TT_IDENTIFIER_CHAR_MAX ) >= buffer_size ) {
		TT_LOG_ERROR( "plugin/default", "buffer too small for filename in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	strncat( buffer, filename, buffer_size - strnlen( buffer, buffer_size ) - 1 );
       	TT_DEBUG2( "plugin/default", "returning '%s' in %s()", buffer, __FUNCTION__ );
	return TT_OK;
}

