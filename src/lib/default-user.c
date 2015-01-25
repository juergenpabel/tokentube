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
	tt_user_t	user = TT_USER__UNDEFINED;
	char		data[TT_KEY_BITS_MAX/8] = { 0 };
	size_t		data_size = sizeof(data);

	TT_TRACE( "library/plugin", "%s(username='%s',password='%s')", __FUNCTION__, username, password );
	if( username == NULL || username[0] == '\0' || password == NULL || password[0] == '\0' ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( libtokentube_plugin__luks_load( data, &data_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_plugin__luks_load() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( libtokentube_crypto_encrypt( data, data_size, password, strlen(password), username, strlen(username) ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_crypto_encrypt() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	strncpy( user.cipher, libtokentube_crypto_get_cipher(), sizeof(user.cipher)-1 );
	strncpy( user.hash, libtokentube_crypto_get_hash(), sizeof(user.hash)-1 );
	strncpy( user.kdf, g_crypto_kdf, sizeof(user.kdf)-1 );
	user.kdf_iter = g_crypto_kdf_iter;
	memcpy( user.luks_data, data, data_size );
	user.luks_data_len = data_size;
	if( default__impl__user_verifier_set( username, password, &user ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "default__impl__user_verifier_set() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( default__impl__user_storage_save( username, &user ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_plugin__user_save() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int default__api__user_update(const char* username, const char* old_password, const char* new_password, tt_status_t* status) {
	tt_user_t	user = TT_USER__UNDEFINED;

	TT_TRACE( "library/plugin", "%s(username='%s',old_password='%s',new_password='%s')", __FUNCTION__, username, old_password, new_password );
	if( username == NULL || username[0] == '\0' || old_password == NULL || old_password[0] == '\0' || new_password == NULL || new_password[0] == '\0' || status == NULL ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( default__impl__user_storage_load( username, &user ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_plugin__user_load() failed for username='%s' in %s()", username, __FUNCTION__ );
		return TT_ERR;
	}
	if( default__impl__user_verifier_test( username, old_password, &user, status ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "default__impl__user_verifier_test() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( *status == TT_STATUS__YES ) {
		if( libtokentube_crypto_decrypt( user.luks_data, user.luks_data_len, old_password, strlen(old_password), username, strlen(username) ) != TT_OK ) {
			TT_LOG_ERROR( "plugin/default", "libtokentube_crypto_decrypt() failed in %s()", __FUNCTION__ );
			return TT_ERR;
		}
		if( libtokentube_crypto_encrypt( user.luks_data, user.luks_data_len, new_password, strlen(new_password), username, strlen(username) ) != TT_OK ) {
			TT_LOG_ERROR( "plugin/default", "libtokentube_crypto_encrypt() failed in %s()", __FUNCTION__ );
			memset( &user, '\0', sizeof(user) );
			return TT_ERR;
		}
		if( default__impl__user_verifier_set( username, new_password, &user ) != TT_OK ) {
			TT_LOG_ERROR( "plugin/default", "default__impl__user_verifier_set() failed in %s()", __FUNCTION__ );
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
	TT_TRACE( "library/plugin", "%s(username='%s',status=%p)", __FUNCTION__, username, status );
	if( username == NULL || username[0] == '\0' || status == NULL ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( libtokentube_plugin__file_delete( TT_FILE__USER, username, status ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_user_delete() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int default__api__user_exists(const char* username, tt_status_t* status) {
	TT_TRACE( "library/plugin", "%s(username='%s',status=%p)", __FUNCTION__, username, status );
	if( username == NULL || username[0] == '\0' || status == NULL ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( libtokentube_plugin__file_exists( TT_FILE__USER, username, status ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_plugin__file_exists() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int default__api__user_execute_verify(const char* username, const char* password, tt_status_t* status) {
	tt_user_t	user = TT_USER__UNDEFINED;

	TT_TRACE( "library/plugin", "%s(username='%s',password='%s',status=%p)", __FUNCTION__, username, password, status );
	if( username == NULL || username[0] == '\0' || password == NULL || password[0] == '\0' || status == NULL ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( default__impl__user_storage_load( username, &user ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "default__impl__user_storage_load() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( default__impl__user_verifier_test( username, password, &user, status ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "default__impl__user_verifier_test() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int default__api__user_execute_load(const char* username, const char* password, char* key, size_t* key_size) {
	tt_status_t	status = TT_STATUS__UNDEFINED;
	tt_user_t	user = TT_USER__UNDEFINED;

	TT_TRACE( "library/plugin", "%s(username='%s',password='%s',key=%p,key_size=%p)", __FUNCTION__, username, password, key, key_size );
	if( username == NULL || username[0] == '\0' || password == NULL || password[0] == '\0' || key == NULL || key_size == NULL || *key_size == 0) {
		TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( default__impl__user_storage_load( username, &user ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "default__impl__user_storage_load() failed for username='%s' in %s()", username, __FUNCTION__ );
		return TT_ERR;
	}
	if( default__impl__user_verifier_test( username, password, &user, &status ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "default__impl__user_verifier_test() failed for username='%s' in %s()", username, __FUNCTION__ );
		return TT_ERR;
	}
	if( status == TT_STATUS__NO ) {
		*key_size = 0;
		return TT_OK;
	}
	if( *key_size < user.luks_data_len ) {
                TT_LOG_ERROR( "plugin/default", "user loaded but provided buffer too small for username='%s' in %s()", username, __FUNCTION__ );
		return TT_ERR;
	}
	if( libtokentube_crypto_decrypt( user.luks_data, user.luks_data_len, password, strlen(password), username, strlen(username) ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_crypto_decrypt() failed for username='%s' in %s()", username, __FUNCTION__ );
		return TT_ERR;
	}
	memcpy( key, user.luks_data, user.luks_data_len );
	*key_size = user.luks_data_len;
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

	TT_TRACE( "library/plugin", "%s(username='%s',status=%p)", __FUNCTION__, username, status );
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

