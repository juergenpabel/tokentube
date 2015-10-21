#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gcrypt.h>
#include "libtokentube.h"


static int default__impl__user_kv_serialize(const char* name, const dflt_user_keyvalue_t* kv, size_t kv_size, char* buffer, size_t* buffer_size) {
	size_t     buffer_pos = 0;
	size_t     buffer_len = 0;
	size_t     offset = 0;

	buffer_pos += snprintf( buffer, *buffer_size-1, "%s\n", name );
	for( offset=0; offset<kv_size; offset++ ) {
		if( kv[offset].key_size > 0 ) {
			buffer_len = *buffer_size - buffer_pos - 1;
			if( libtokentube_util_hex_encode( kv[offset].key, kv[offset].key_size, buffer+buffer_pos, &buffer_len ) != TT_OK ) {
				TT_LOG_ERROR( "plugin/default", "libtokentube_util_hex_encode() failed in %s()", __FUNCTION__ );
				return TT_ERR;
			}
			buffer_pos += buffer_len;
			buffer[buffer_pos] = '=';
			buffer_pos++;
			buffer_len = *buffer_size - buffer_pos - 1;
			if( libtokentube_util_hex_encode( kv[offset].value, kv[offset].value_size, buffer+buffer_pos, &buffer_len ) != TT_OK ) {
				TT_LOG_ERROR( "plugin/default", "libtokentube_util_hex_encode() failed in %s()", __FUNCTION__ );
				return TT_ERR;
			}
			buffer_pos += buffer_len;
			buffer[buffer_pos] = '\n';
			buffer_pos++;
		}
	}
	*buffer_size = buffer_pos;
	return TT_OK;
}


static int default__impl__user_hmac_calc(const char* username, const char* password, dflt_user_t* user, char* hmac, size_t* hmac_size) {
	size_t     username_size, password_size;
	char       key[TT_KEY_BITS_MAX/8] =  {0};
	size_t     key_size = sizeof(key);
	char       buffer[DEFAULT__FILESIZE_MAX+1] =  {0};
	size_t     buffer_size = 0;
	size_t     buffer_pos = 0;
	size_t     offset = 0;

	buffer_pos += snprintf( buffer, sizeof(buffer)-1, "username\n%s\ncrypto\n%s\n%s\n%s\n%zd\n", username, user->crypto.hash, user->crypto.cipher, user->crypto.kdf, user->crypto.kdf_iter );
	buffer_size = sizeof(buffer) - buffer_pos - 1;
	if( default__impl__user_kv_serialize( "credentials:parameters", user->cred.parameter, DEFAULT__PARAMETER_MAX, buffer+buffer_pos, &buffer_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "default__impl__user_kv_serialize() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	buffer_pos += buffer_size;
	buffer_size = sizeof(buffer) - buffer_pos - 1;
	if( default__impl__user_kv_serialize( "credentials:constraints", user->cred.constraint, DEFAULT__CONSTRAINT_MAX, buffer+buffer_pos, &buffer_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "default__impl__user_kv_serialize() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	buffer_pos += buffer_size;
	buffer_size = sizeof(buffer) - buffer_pos - 1;
	for( offset=0; offset<DEFAULT__KEY_MAX; offset++ ) {
		if( user->key[offset].data.key_size > 0 ) {
			buffer_pos += snprintf( buffer+buffer_pos, buffer_size, "key\n" );
			buffer_size = sizeof(buffer) - buffer_pos - 1;
			if( default__impl__user_kv_serialize( "key:constraints", user->key[offset].constraint, DEFAULT__CONSTRAINT_MAX, buffer+buffer_pos, &buffer_size ) != TT_OK ) {
				TT_LOG_ERROR( "plugin/default", "default__impl__user_kv_serialize() failed in %s()", __FUNCTION__ );
				return TT_ERR;
			}
			buffer_pos += buffer_size;
			buffer_size = sizeof(buffer) - buffer_pos - 1;
			if( default__impl__user_kv_serialize( "key:parameters", user->key[offset].parameter, DEFAULT__PARAMETER_MAX, buffer+buffer_pos, &buffer_size ) != TT_OK ) {
				TT_LOG_ERROR( "plugin/default", "default__impl__user_kv_serialize() failed in %s()", __FUNCTION__ );
				return TT_ERR;
			}
			buffer_pos += buffer_size;
			buffer_size = sizeof(buffer) - buffer_pos - 1;
			if( default__impl__user_kv_serialize( "key:data", &user->key[offset].data, 1, buffer+buffer_pos, &buffer_size ) != TT_OK ) {
				TT_LOG_ERROR( "plugin/default", "default__impl__user_kv_serialize() failed in %s()", __FUNCTION__ );
				return TT_ERR;
			}
			buffer_pos += buffer_size;
			buffer_size = sizeof(buffer) - buffer_pos - 1;
		}
	}
	username_size = strnlen(username,TT_USERNAME_CHAR_MAX);
	password_size = strnlen(password,TT_PASSWORD_CHAR_MAX);
	if( libtokentube_crypto_kdf_impl( user->crypto.kdf, user->crypto.kdf_iter, user->crypto.hash, username, username_size, password, password_size, key, &key_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_crypto_kdf_impl() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( libtokentube_crypto_hmac_impl( user->crypto.hash, buffer, buffer_pos, key, key_size, hmac, hmac_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_crypto_hmac_impl() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int default__impl__user_hmac_set(const char* username, const char* password, dflt_user_t* user) {
	TT_TRACE( "plugin/default", "%s(username='%s',password='%s',user=%p)", __FUNCTION__, username, password, user );
	if( username == NULL || username[0] == '\0' || password == NULL || password[0] == '\0' || user == NULL ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( gcry_md_get_algo_dlen( gcry_md_map_name( user->crypto.hash ) ) == 0 ) {
		TT_LOG_ERROR( "plugin/default", "gcry_md_get_algo_dlen() failed for '%s' in %s()", user->crypto.hash,__FUNCTION__ );
		return TT_ERR;
	}
	user->hmac.data_size = sizeof(user->hmac.data);
	if( default__impl__user_hmac_calc( username, password, user, user->hmac.data, &user->hmac.data_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "default__impl__user_serialize() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int default__impl__user_hmac_test(const char* username, const char* password, dflt_user_t* user, tt_status_t* status) {
        dflt_user_hmac_t  hmac = {0};

	TT_TRACE( "plugin/default", "%s(username='%s',password='%s',user=%p,status=%p)", __FUNCTION__, username, password, user, status );
	if( username == NULL || username[0] == '\0' || password == NULL || password[0] == '\0' || user == NULL || status == NULL ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( gcry_md_get_algo_dlen( gcry_md_map_name( user->crypto.hash ) ) == 0 ) {
		TT_LOG_ERROR( "plugin/default", "gcry_md_get_algo_dlen() failed for '%s' in %s()", user->crypto.hash,__FUNCTION__ );
		return TT_ERR;
	}
	hmac.data_size = sizeof(hmac.data);
	if( default__impl__user_hmac_calc( username, password, user, hmac.data, &hmac.data_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "default__impl__user_serialize() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( hmac.data_size != user->hmac.data_size ) {
		TT_LOG_WARN( "plugin/default", "size of calculated hmac does not match size of stored hmac in %s()", __FUNCTION__ );
		*status = TT_NO;
		return TT_OK;
	}
	if( memcmp( hmac.data, user->hmac.data, hmac.data_size ) == 0 ) {
		TT_DEBUG3( "plugin/default", "%s() returns status=TT_YES", __FUNCTION__ );
		*status = TT_YES;
	} else {
		TT_DEBUG3( "plugin/default", "%s() returns status=TT_NO", __FUNCTION__ );
		*status = TT_NO;
	}
	return TT_OK;
}

