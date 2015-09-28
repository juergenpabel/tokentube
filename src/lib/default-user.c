#define _GNU_SOURCE
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
int default__api__user_create(const char* username, const char* password) {
	char       path[FILENAME_MAX+1] = {0};
	size_t     path_size = sizeof(path);
	char*      filename_start = NULL;
	char*      filename_end = NULL;
	size_t     key_offset = 0;
	char       buffer[FILENAME_MAX+1] = {0};
	size_t     buffer_size = sizeof(buffer);
	dflt_user_t  user = TT_USER__UNDEFINED;

	TT_TRACE( "plugin/default", "%s(username='%s',password='%s')", __FUNCTION__, username, password );
	if( username == NULL || username[0] == '\0' || password == NULL || password[0] == '\0' ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	strncpy( user.crypto.cipher, libtokentube_crypto_get_cipher(), sizeof(user.crypto.cipher)-1 );
	strncpy( user.crypto.hash, libtokentube_crypto_get_hash(), sizeof(user.crypto.hash)-1 );
	strncpy( user.crypto.kdf, g_crypto_kdf, sizeof(user.crypto.kdf)-1 );
	user.crypto.kdf_iter = g_crypto_kdf_iter;

	if( libtokentube_conf_read_str( "storage|key-files|directory", path, &path_size) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_conf_read_str() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( strnlen( path, sizeof(path) ) == 0 ) {
		strncpy( path, "/etc/tokentube/keys", sizeof(path) );
	}
	TT_DEBUG3( "plugin/default", "loading all keys from '%s' in %s()", path, __FUNCTION__ );
	if( default__storage_posix_load( TT_FILE__KEY, path, buffer, &buffer_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "default__storage_posix_load() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	filename_start = buffer;
	filename_end = strchrnul( filename_start+1, '\n' );
	while( filename_start[0] != '\0' ) {
		filename_end = strchrnul( filename_start, '\n' );
		filename_end[0] = '\0';
		if( filename_start != filename_end ) {
			TT_DEBUG3( "plugin/default", "loading key from '%s' in %s()", filename_start, __FUNCTION__ );
			strncpy( user.key[key_offset].data.key, basename( filename_start ), sizeof(user.key[key_offset].data.key) );
			user.key[key_offset].data.key_size = strnlen( user.key[key_offset].data.key, TT_IDENTIFIER_CHAR_MAX );
			user.key[key_offset].data.value_size = sizeof(user.key[key_offset].data.value);
			if( default__storage_posix_load( TT_FILE__KEY, filename_start, user.key[key_offset].data.value, &user.key[key_offset].data.value_size ) != TT_OK ) {
				TT_LOG_ERROR( "plugin/default", "default__storage_posix_load() failed in %s()", __FUNCTION__ );
				memset( &user, '\0', sizeof(user) );
				return TT_ERR;
			}
			if( default__impl__user_key_encrypt( username, password, &user, key_offset ) != TT_OK ) {
				TT_LOG_ERROR( "plugin/default", "default__impl__user_key_encrypt() failed in %s()", __FUNCTION__ );
				memset( &user, '\0', sizeof(user) );
				return TT_ERR;
			}
			filename_start = strrchr( filename_start, '/' ) + 1;
			key_offset++;
		}
		filename_start = filename_end + 1;
	}
	if( default__impl__user_hmac_set( username, password, &user ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "default__impl__user_hmac_set() failed in %s()", __FUNCTION__ );
		memset( &user, '\0', sizeof(user) );
		return TT_ERR;
	}
	if( default__impl__user_storage_save( username, &user ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_plugin__user_save() failed in %s()", __FUNCTION__ );
		memset( &user, '\0', sizeof(user) );
		return TT_ERR;
	}
	memset( &user, '\0', sizeof(user) );
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int default__api__user_update(const char* username, const char* old_password, const char* new_password, tt_status_t* status) {
	dflt_user_t  user = TT_USER__UNDEFINED;
	size_t     key_offset = 0;

	TT_TRACE( "plugin/default", "%s(username='%s',old_password='%s',new_password='%s')", __FUNCTION__, username, old_password, new_password );
	if( username == NULL || username[0] == '\0' || old_password == NULL || old_password[0] == '\0' || new_password == NULL || new_password[0] == '\0' || status == NULL ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( default__impl__user_storage_load( username, &user ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_plugin__user_load() failed for username='%s' in %s()", username, __FUNCTION__ );
		return TT_ERR;
	}
	if( default__impl__user_hmac_test( username, old_password, &user, status ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "default__impl__user_hmac_test() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( *status == TT_STATUS__YES ) {
		strncpy( user.crypto.cipher, libtokentube_crypto_get_cipher(), sizeof(user.crypto.cipher)-1 );
		strncpy( user.crypto.hash, libtokentube_crypto_get_hash(), sizeof(user.crypto.hash)-1 );
		strncpy( user.crypto.kdf, g_crypto_kdf, sizeof(user.crypto.kdf)-1 );
		user.crypto.kdf_iter = g_crypto_kdf_iter;
		for( key_offset = 0; key_offset<DEFAULT__KEY_MAX; key_offset++ ) {
			if( user.key[key_offset].data.key_size != 0 ) {
				if( default__impl__user_key_decrypt( username, old_password, &user, key_offset ) != TT_OK ) {
					TT_LOG_ERROR( "plugin/default", "default__impl__user_key_decrypt() failed in %s()", __FUNCTION__ );
					memset( &user, '\0', sizeof(user) );
					return TT_ERR;
				}
				if( default__impl__user_key_encrypt( username, new_password, &user, key_offset ) != TT_OK ) {
					TT_LOG_ERROR( "plugin/default", "default__impl__user_key_encrypt() failed in %s()", __FUNCTION__ );
					memset( &user, '\0', sizeof(user) );
					return TT_ERR;
				}
			}
		}
		if( default__impl__user_hmac_set( username, new_password, &user ) != TT_OK ) {
			TT_LOG_ERROR( "plugin/default", "default__impl__user_hmac_set() failed in %s()", __FUNCTION__ );
			memset( &user, '\0', sizeof(user) );
			return TT_ERR;
		}
		if( default__impl__user_storage_save( username, &user ) != TT_OK ) {
			TT_LOG_ERROR( "plugin/default", "default__impl__user_save() failed in %s()", __FUNCTION__ );
			memset( &user, '\0', sizeof(user) );
			return TT_ERR;
		}
	}
	memset( &user, '\0', sizeof(user) );
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int default__api__user_delete(const char* username, tt_status_t* status) {
	TT_TRACE( "plugin/default", "%s(username='%s',status=%p)", __FUNCTION__, username, status );
	if( username == NULL || username[0] == '\0' || status == NULL ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( libtokentube_plugin__storage_delete( TT_FILE__USER, username, status ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_user_delete() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int default__api__user_exists(const char* username, tt_status_t* status) {
	TT_TRACE( "plugin/default", "%s(username='%s',status=%p)", __FUNCTION__, username, status );
	if( username == NULL || username[0] == '\0' || status == NULL ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( libtokentube_plugin__storage_exists( TT_FILE__USER, username, status ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_plugin__storage_exists() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int default__api__user_key_add(const char* username, const char* password, const char* identifier, tt_status_t* status) {
	dflt_user_t  user = TT_USER__UNDEFINED;
	size_t       identifier_size = 0;
	size_t       key_offset = 0;

	TT_TRACE( "plugin/default", "%s(username='%s',password='%s',idenfifier='%s')", __FUNCTION__, username, password, identifier );
	if( username == NULL || username[0] == '\0' || password == NULL || password[0] == '\0' || identifier == NULL || identifier[0] == '\0' || status == NULL ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	identifier_size = strnlen( identifier, TT_IDENTIFIER_CHAR_MAX );
	*status = TT_STATUS__UNDEFINED;
	if( default__impl__user_storage_load( username, &user ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_plugin__user_load() failed for username='%s' in %s()", username, __FUNCTION__ );
		return TT_ERR;
	}
	if( default__impl__user_hmac_test( username, password, &user, status ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "default__impl__user_hmac_test() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( *status == TT_STATUS__NO ) {
		TT_LOG_ERROR( "plugin/default", "default__impl__user_hmac_test() returned TT_NO in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	*status = TT_STATUS__NO;
	for( key_offset = 0; *status == TT_STATUS__NO && key_offset<DEFAULT__KEY_MAX; key_offset++ ) {
		if( user.key[key_offset].data.key_size == identifier_size && memcmp( user.key[key_offset].data.key, identifier, identifier_size ) == 0) {
			TT_DEBUG3( "plugin/default", "key with identifier '%s' found at key offset %zd in %s()", identifier, key_offset, __FUNCTION__ );
			return TT_OK;
		}
		if( user.key[key_offset].data.key_size == 0 ) {
			TT_DEBUG3( "plugin/default", "loading key with identifier '%s' into key offset %zd in %s()", identifier, key_offset, __FUNCTION__ );
			strncpy( user.key[key_offset].data.key, identifier, sizeof(user.key[key_offset].data.key) );
			user.key[key_offset].data.key_size = strnlen( user.key[key_offset].data.key, TT_IDENTIFIER_CHAR_MAX );
			user.key[key_offset].data.value_size = sizeof(user.key[key_offset].data.value);
			if( libtokentube_plugin__storage_load( TT_FILE__KEY, identifier, user.key[key_offset].data.value, &user.key[key_offset].data.value_size ) != TT_OK ) {
				TT_LOG_ERROR( "plugin/default", "libtokentube_plugin__storage_load() failed in %s()", __FUNCTION__ );
				return TT_ERR;
			}
			if( default__impl__user_key_encrypt( username, password, &user, key_offset ) != TT_OK ) {
				TT_LOG_ERROR( "plugin/default", "default__impl__user_key_encrypt() failed in %s()", __FUNCTION__ );
				return TT_ERR;
			}
			*status = TT_STATUS__YES;
		}
	}
	if( *status == TT_STATUS__YES ) {
		if( default__impl__user_hmac_set( username, password, &user ) != TT_OK ) {
			TT_LOG_ERROR( "plugin/default", "default__impl__user_hmac_set() failed in %s()", __FUNCTION__ );
			return TT_ERR;
		}
		if( default__impl__user_storage_save( username, &user ) != TT_OK ) {
			TT_LOG_ERROR( "plugin/default", "default__impl__user_save() failed in %s()", __FUNCTION__ );
			return TT_ERR;
		}
	}
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int default__api__user_key_del(const char* username, const char* password, const char* identifier, tt_status_t* status) {
	dflt_user_t  user = TT_USER__UNDEFINED;
	size_t       identifier_size = 0;
	size_t       key_offset = 0;

	TT_TRACE( "plugin/default", "%s(username='%s',password='%s',idenfifier='%s')", __FUNCTION__, username, password, identifier );
	if( username == NULL || username[0] == '\0' || password == NULL || password[0] == '\0' || identifier == NULL || identifier[0] == '\0' || status == NULL ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	identifier_size = strnlen( identifier, TT_IDENTIFIER_CHAR_MAX );
	*status = TT_STATUS__UNDEFINED;
	if( default__impl__user_storage_load( username, &user ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_plugin__user_load() failed for username='%s' in %s()", username, __FUNCTION__ );
		return TT_ERR;
	}
	if( default__impl__user_hmac_test( username, password, &user, status ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "default__impl__user_hmac_test() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( *status == TT_STATUS__NO ) {
		TT_LOG_ERROR( "plugin/default", "default__impl__user_hmac_test() returned TT_NO in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	*status = TT_STATUS__NO;
	for( key_offset = 0; *status == TT_STATUS__NO && key_offset<DEFAULT__KEY_MAX; key_offset++ ) {
		if( user.key[key_offset].data.key_size == identifier_size && memcmp( user.key[key_offset].data.key, identifier, identifier_size ) == 0 ) {
			memset( user.key[key_offset].data.key, '\0', sizeof(user.key[key_offset].data.key) );
			user.key[key_offset].data.key_size = 0;
			memset( user.key[key_offset].data.value, '\0', sizeof(user.key[key_offset].data.value) );
			user.key[key_offset].data.value_size = 0;
			*status = TT_STATUS__YES;
		}
	}
	if( *status == TT_STATUS__YES ) {
		if( default__impl__user_hmac_set( username, password, &user ) != TT_OK ) {
			TT_LOG_ERROR( "plugin/default", "default__impl__user_hmac_set() failed in %s()", __FUNCTION__ );
			return TT_ERR;
		}
		if( default__impl__user_storage_save( username, &user ) != TT_OK ) {
			TT_LOG_ERROR( "plugin/default", "default__impl__user_save() failed in %s()", __FUNCTION__ );
			return TT_ERR;
		}
	}
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int default__api__user_execute_verify(const char* username, const char* password, tt_status_t* status) {
	dflt_user_t	user = TT_USER__UNDEFINED;

	TT_TRACE( "plugin/default", "%s(username='%s',password='%s',status=%p)", __FUNCTION__, username, password, status );
	if( username == NULL || username[0] == '\0' || password == NULL || password[0] == '\0' || status == NULL ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( default__impl__user_storage_load( username, &user ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "default__impl__user_storage_load() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( default__impl__user_hmac_test( username, password, &user, status ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "default__impl__user_hmac_test() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int default__api__user_execute_loadkey(const char* username, const char* password, const char* identifier, char* key, size_t* key_size) {
	tt_status_t     status = TT_STATUS__UNDEFINED;
	dflt_user_t     user = TT_USER__UNDEFINED;
	size_t		identifier_size = 0;
	size_t		key_offset = 0;

	TT_TRACE( "plugin/default", "%s(username='%s',password='%s',key=%p,key_size=%p)", __FUNCTION__, username, password, key, key_size );
	if( username == NULL || username[0] == '\0' || password == NULL || password[0] == '\0' || identifier == NULL || identifier[0] == '\0' || key == NULL || key_size == NULL || *key_size == 0) {
		TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	identifier_size = strnlen( identifier, TT_IDENTIFIER_CHAR_MAX );
	if( default__impl__user_storage_load( username, &user ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "default__impl__user_storage_load() failed for username='%s' in %s()", username, __FUNCTION__ );
		return TT_ERR;
	}
	if( default__impl__user_hmac_test( username, password, &user, &status ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "default__impl__user_hmac_test() failed for username='%s' in %s()", username, __FUNCTION__ );
		return TT_ERR;
	}
	if( status == TT_STATUS__NO ) {
		TT_DEBUG3( "plugin/default", "default__impl__user_hmac_test() returned TT_STATUS__NO in %s()", __FUNCTION__ );
		*key_size = 0;
		return TT_OK;
	}
	status = TT_STATUS__NO;
	for( key_offset=0; status == TT_STATUS__NO && key_offset<DEFAULT__KEY_MAX; key_offset++ ) {
		if( memcmp( user.key[key_offset].data.key, identifier, identifier_size ) == 0 ) {
			if( *key_size < user.key[key_offset].data.key_size ) {
				TT_LOG_ERROR( "plugin/default", "user loaded but provided buffer too small for username='%s' in %s()", username, __FUNCTION__ );
				return TT_ERR;
			}
			if( default__impl__user_key_decrypt( username, password, &user, key_offset ) != TT_OK ) {
				TT_LOG_ERROR( "plugin/default", "default__impl__user_key_decrypt() failed for username='%s' in %s()", username, __FUNCTION__ );
				return TT_ERR;
			}
			memcpy( key, user.key[key_offset].data.value, user.key[key_offset].data.value_size );
			*key_size = user.key[key_offset].data.value_size;
			status = TT_STATUS__YES;
		}
	}
	if( status == TT_STATUS__NO ) {
		TT_DEBUG3( "plugin/default", "no matching key found in %s()", __FUNCTION__ );
		*key_size = 0;
	}
	memset( &user, '\0', sizeof(user) );
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int default__api__user_execute_autoenrollment(const char* username, const char* password, tt_status_t* status) {
	char		groupname[TT_USERNAME_CHAR_MAX+1] = { 0 };
	size_t		groupname_size = sizeof(groupname);
	char**		groupmembers = NULL;
	int		uid_min, uid_max;
	int		gid_min, gid_max;
	struct passwd*	user = NULL;
	struct group*	group = NULL;
	int		offset = 0;

	TT_TRACE( "plugin/default", "%s(username='%s',status=%p)", __FUNCTION__, username, status );
	if( username == NULL || username[0] == '\0' || status == NULL ) {
		return TT_ERR;
	}
	*status = TT_STATUS__UNDEFINED;
	user = getpwnam( username );
	if( user == NULL ) {
		TT_LOG_ERROR( "plugin/default", "getpwnam('%s') failed in %s()", username, __FUNCTION__ );
		return TT_ERR;
	}
	if( libtokentube_conf_read_int( "user|auto-enrollment|uid-minimum", &uid_min ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_conf_read_int() failed for 'user|auto-enrollment|uid-minimum' in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( libtokentube_conf_read_int( "user|auto-enrollment|uid-maximum", &uid_max ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_conf_read_int() failed for 'user|auto-enrollment|uid-maximum' in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( (int)user->pw_uid >= uid_min && (int)user->pw_uid <= uid_max ) {
		*status = TT_STATUS__YES;
		return TT_OK;
	}
	if( libtokentube_conf_read_int( "user|auto-enrollment|gid-minimum", &gid_min ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_conf_read_int() failed for 'user|auto-enrollment|gid-minimum' in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( libtokentube_conf_read_int( "user|auto-enrollment|gid-maximum", &gid_max ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_conf_read_int() failed for 'user|auto-enrollment|gid-maximum' in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( (int)user->pw_gid >= gid_min && (int)user->pw_gid <= gid_max ) {
		*status = TT_STATUS__YES;
		return TT_OK;
	}
	groupname_size = sizeof(groupname);
	if( libtokentube_conf_read_list( "user|auto-enrollment|groups", offset, groupname, &groupname_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_conf_read_list() failed for 'user|auto-enrollment|groups' in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	while( groupname_size > 0 ) {
		group = getgrgid( user->pw_gid );
		if( group != NULL ) {
			if( strncmp( group->gr_name, groupname, groupname_size ) == 0 ) {
				*status = TT_STATUS__YES;
				return TT_OK;
			}
		}
		group = getgrnam( groupname );
		if( group != NULL ) {
			groupmembers = group->gr_mem;
			while( groupmembers != NULL && *groupmembers != NULL ) {
				if( strcmp(*groupmembers, username) == 0 ) {
					*status = TT_STATUS__YES;
					return TT_OK;
				}
				groupmembers++;
			}
		}
		offset++;
		groupname_size = sizeof(groupname);
		if( libtokentube_conf_read_list( "user|auto-enrollment|groups", offset, groupname, &groupname_size ) != TT_OK ) {
			TT_LOG_ERROR( "plugin/default", "libtokentube_conf_read_list() failed for 'user|auto-enrollment|groups' in %s()", __FUNCTION__ );
			return TT_ERR;
		}
	}
	if( *status == TT_STATUS__UNDEFINED ) {
		*status = TT_STATUS__NO;
	}
	if( *status == TT_STATUS__YES ) {
		if( libtokentube_plugin__user_create( username, password ) != TT_OK ) {
			TT_LOG_ERROR( "plugin/default", "API:user.create failed in %s()", __FUNCTION__ );
			return TT_ERR;
		}
	}
	return TT_OK;
}

