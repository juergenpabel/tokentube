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
int default__identifier2uuid(const char* identifier, char* out, size_t out_size) {
	char	hash[TT_DIGEST_BITS_MAX/8] = {0};
	size_t	hash_size = 0;

	TT_TRACE( "library/plugin", "%s(identifier='%s',out=%p, out_size=%zd)", __FUNCTION__, identifier, out, out_size );
	if( identifier == NULL || identifier[0] == '\0' || out == NULL || out_size == 0 ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	hash_size = gcry_md_get_algo_dlen( gcry_md_map_name( libtokentube_crypto_get_hash() ) );
	if( hash_size == 0 ) {
		TT_LOG_ERROR( "plugin/default", "unsupported hash algorithm '%s' in %s()", libtokentube_crypto_get_hash(), __FUNCTION__ );
		return TT_ERR;
	}
	if( hash_size > sizeof(hash) ) {
		TT_LOG_ERROR( "plugin/default", "hash algorithm '%s' too many bits in %s()", libtokentube_crypto_get_hash(), __FUNCTION__ );
		return TT_ERR;
	}
	if( out_size < hash_size*2+1 ) {
		TT_LOG_ERROR( "plugin/default", "buffer too small for hash algorithm '%s' in %s()", libtokentube_crypto_get_hash(), __FUNCTION__ );
		return TT_ERR;
	}
	memset( out, '\0', out_size );
	if( libtokentube_crypto_hash( identifier, strlen(identifier), hash, &hash_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "internal error in %s at %d", __FILE__, __LINE__ );
		return TT_ERR;
	}
	if( libtokentube_util_hex_encode( hash, hash_size, out, &out_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "internal error in %s at %d", __FILE__, __LINE__ );
		return TT_ERR;
	}
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int default__storage_get_filename(tt_file_t file, const char* identifier, char* buffer, const size_t buffer_size) {
	char	uuid[TT_DIGEST_BITS_MAX/8*2+1] = { 0 };
	char	path[FILENAME_MAX] = { 0 };
	size_t  path_size = sizeof(path);
	size_t	pos = 0;

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
	if( file == TT_FILE__KEY ) {
		if( libtokentube_conf_read_str( "storage|key-files|directory", path, &path_size) != TT_OK ) {
			TT_LOG_ERROR( "plugin/default", "libtokentube_conf_read_str() failed for 'storage|key-files|directory' in %s()", __FUNCTION__ );
			return TT_ERR;
		}
		snprintf( buffer, buffer_size-1, "%s/%s", path, identifier );
		return TT_OK;
	}
	if( 17+5+strnlen(identifier,TT_DIGEST_BITS_MAX/8*2+1)+5 >= buffer_size ) {
		TT_LOG_ERROR( "plugin/default", "buffer too small for filename in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	strncpy( buffer, "/boot/tokentube/", buffer_size-1 );
	pos = strnlen( buffer, buffer_size );
	if( default__identifier2uuid( identifier, uuid, sizeof(uuid) ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "internal error in %s at %d", __FILE__, __LINE__ );
		return TT_ERR;
	}
	switch( file ) {
		case TT_FILE__USER:
			snprintf( buffer+pos, buffer_size-pos-1, "user/%s.dat", uuid );
			break;
		case TT_FILE__OTP:
			snprintf( buffer+pos, buffer_size-pos-1, "otp/%s.dat", uuid );
			break;
		default:
			memset( buffer, '\0', buffer_size );
			return TT_ERR;
	}
       	TT_DEBUG2( "plugin/default", "returning '%s' in %s()", buffer, __FUNCTION__ );
	return TT_OK;
}

