#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gcrypt.h>
#include "libtokentube.h"


static int default__impl__user_hmac_calc(const char* username, const char* password, tt_user_t* user, char* hmac, size_t* hmac_size) {
        char       buffer[DEFAULT__FILESIZE_MAX+1] =  {0};
	size_t     buffer_size = 0;
	size_t     buffer_pos = 0;
	size_t     offset = 0;

	buffer_pos += snprintf( buffer, sizeof(buffer)-1, "%s\n%s\n%s\n%s\n%zd\n", username, user->crypto.hash, user->crypto.cipher, user->crypto.kdf, user->crypto.kdf_iter );
	for( offset=0; offset<DEFAULT__CRED_MAX; offset++ ) {
		if( user->cred[offset].key_size > 0 ) {
			buffer_size = sizeof(buffer) - buffer_pos - 1;
			if( libtokentube_util_hex_encode( user->cred[offset].key, user->cred[offset].key_size, buffer+buffer_pos, &buffer_size ) != TT_OK ) {
				TT_LOG_ERROR( "plugin/default", "libtokentube_util_hex_encode() failed in %s()", __FUNCTION__ );
				return TT_ERR;
			}
			buffer_pos += buffer_size;
			buffer[buffer_pos] = '=';
			buffer_pos++;
			buffer_size = sizeof(buffer) - buffer_pos - 1;
			if( libtokentube_util_hex_encode( user->cred[offset].value, user->cred[offset].value_size, buffer+buffer_pos, &buffer_size ) != TT_OK ) {
				TT_LOG_ERROR( "plugin/default", "libtokentube_util_hex_encode() failed in %s()", __FUNCTION__ );
				return TT_ERR;
			}
			buffer_pos += buffer_size;
			buffer[buffer_pos] = '\n';
			buffer_pos++;
		}
	}
	for( offset=0; offset<DEFAULT__KEY_MAX; offset++ ) {
		if( user->key[offset].uuid_size > 0 ) {
			buffer_size = sizeof(buffer) - buffer_pos - 1;
			if( libtokentube_util_hex_encode( user->key[offset].uuid, user->key[offset].uuid_size, buffer+buffer_pos, &buffer_size ) != TT_OK ) {
				TT_LOG_ERROR( "plugin/default", "libtokentube_util_hex_encode() failed in %s()", __FUNCTION__ );
				return TT_ERR;
			}
			buffer_pos += buffer_size;
			buffer[buffer_pos] = '=';
			buffer_pos++;
			buffer_size = sizeof(buffer) - buffer_pos - 1;
			if( libtokentube_util_hex_encode( user->key[offset].data, user->key[offset].data_size, buffer+buffer_pos, &buffer_size ) != TT_OK ) {
				TT_LOG_ERROR( "plugin/default", "libtokentube_util_hex_encode() failed in %s()", __FUNCTION__ );
				return TT_ERR;
			}
			buffer_pos += buffer_size;
			buffer[buffer_pos] = '\n';
			buffer_pos++;
		}
	}
	if( libtokentube_crypto_hmac_impl( user->crypto.hash, buffer, buffer_pos, password, strlen(password), hmac, hmac_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_crypto_hmac_impl() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int default__impl__user_hmac_set(const char* username, const char* password, tt_user_t* user) {

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
int default__impl__user_hmac_test(const char* username, const char* password, tt_user_t* user, tt_status_t* status) {
        tt_user_hmac_t  hmac = {0};

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

