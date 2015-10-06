#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <confuse.h>
#include <gcrypt.h>
#include "libtokentube.h"


__attribute__ ((visibility ("hidden")))
int default__api__auth_user_verify(const char* username, const char* password, tt_status_t* status) {
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
int default__api__auth_user_loadkey(const char* username, const char* password, const char* identifier, char* key, size_t* key_size) {
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
int default__api__auth_user_autoenrollment(const char* username, const char* password, tt_status_t* status) {
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
		if( libtokentube_plugin__user_create( username, password, status ) != TT_OK ) {
			TT_LOG_ERROR( "plugin/default", "API:user.create failed in %s()", __FUNCTION__ );
			return TT_ERR;
		}
	}
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int default__api__auth_otp_challenge( const char* identifier, char* challenge, size_t* challenge_size) {
	char		data[2+TT_OTP_BITS_MAX/8] = { 0 };
	dflt_otp_t	otp = {0};
	unsigned short	crc = 0;
	size_t		i = 0;

	TT_TRACE( "plugin/default", "%s(identifier='%s')", __FUNCTION__, identifier );
	if( identifier == NULL || challenge == NULL || challenge_size == NULL || *challenge_size == 0 ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameters in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	srandom( time(NULL) );
	if( default__impl__otp_storage_load( identifier, &otp ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "default__impl__otp_storage_load() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( 2+otp.bits/8 > sizeof(data) ) {
		TT_LOG_ERROR( "plugin/default", "otp.bits/8=%d > sizeof(data)=%d in %s()", otp.bits/8, sizeof(data), __FUNCTION__ );
		return TT_ERR;
	}
	if( libtokentube_util_crc16( otp.data, otp.bits/8, &crc ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "crc16() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	data[0] = (crc >> 0) & 0xff;
	data[1] = (crc >> 8) & 0xff;
	for( i=0; i<otp.bits/8; i++ ) {
		data[2+i] = random() & 0xff;
	}
	if( libtokentube_util_base32_encode( data, 2+otp.bits/8, challenge, challenge_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "API:base32_encode() failed for identifier '%s' in %s()", identifier,  __FUNCTION__ );
		return TT_ERR;
	}
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int default__api__auth_otp_response(const char* identifier, const char* challenge, char* response, size_t* response_size) {
	char            challenge_raw[2+TT_OTP_BITS_MAX/8] = {0};
	size_t          challenge_raw_size = sizeof(challenge_raw);
	char            response_raw[2+TT_OTP_BITS_MAX/8] = {0};
	char		data[TT_KEY_BITS_MAX/8] = {0};
	size_t		data_size = sizeof(data);
	dflt_uhd_t	uhd = {0};
	size_t		i = 0;
	unsigned short	crc;


	TT_TRACE( "plugin/default", "%s(identifier='%s',challenge='%s')", __FUNCTION__, identifier, challenge );
	if( identifier == NULL || challenge == NULL || response == NULL || response_size == NULL || *response_size == 0 ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameters in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( default__impl__uhd_storage_load( identifier, &uhd ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "API:storage_load() failed for identifier='%s' in %s()", identifier, __FUNCTION__ );
		return TT_ERR;
	}
	memcpy( data, uhd.key_data, uhd.key_data_size );
	for( i=0; i<uhd.otp_iter; i++ ) {
		data_size = sizeof(data);
		if( libtokentube_crypto_hash_impl( uhd.otp_hash, data, uhd.otp_bits/8, data, &data_size ) != TT_OK ) {
			TT_LOG_ERROR( "plugin/default", "libtokentube_crypto_hash_impl() failed in %s()", __FUNCTION__ );
			return TT_ERR;
		}
	}
	for( i=uhd.key_data_size; i>0; i-- ) {
		data[i-1] = uhd.key_data[i-1] ^ data[(i-1)%(uhd.otp_bits/8)];
	}
	if( libtokentube_util_crc16( data, uhd.otp_bits/8, &crc ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "crc16() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( libtokentube_util_base32_decode( challenge, strnlen( challenge, TT_OTP_TEXT_MAX ), challenge_raw, &challenge_raw_size ) != TT_OK) {
		TT_LOG_ERROR( "plugin/default", "base32_decode() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( challenge_raw_size != (2+uhd.otp_bits/8) ) {
		TT_LOG_ERROR( "plugin/default", "challenge and reponse are of different length" );
		return TT_ERR;
	}
	if( (char)((crc>>0) & 0xff) != challenge_raw[0] || (char)((crc>>8) & 0xff) != challenge_raw[1] ) {
		TT_LOG_ERROR( "plugin/default", "CRC of challenge incorrect (%u %u, instead of %u %u)", (unsigned char)challenge_raw[0], (unsigned char)challenge_raw[1], ((crc>>0)&0xff), ((crc>>8)&0xff) );
		return TT_ERR;
	}
	for( i=0; i<uhd.otp_bits/8; i++ ) {
		response_raw[2+i] = challenge_raw[2+i] ^ uhd.key_data[i] ^ data[i];
	}
	for( i=0; i<uhd.key_data_size; i++ ) {
		data[i] = data[i] ^ uhd.key_data[i];
	}
	data_size = sizeof(data);
	if( libtokentube_crypto_hash_impl( uhd.otp_hash, data, uhd.otp_bits/8, data, &data_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_crypto_hash_impl() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	for( i=uhd.key_data_size; i>0; i-- ) {
		data[i-1] = uhd.key_data[i-1] ^ data[(i-1)%(uhd.otp_bits/8)];
	}
	if( libtokentube_util_crc16( data, uhd.otp_bits/8, &crc ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "API:crc16() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	response_raw[0] = (crc>>0) & 0xff;
	response_raw[1] = (crc>>8) & 0xff;
	if( libtokentube_util_base32_encode(response_raw, 2+uhd.otp_bits/8, response, response_size) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "API:base32_encode() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	uhd.otp_iter++;
	if( default__impl__uhd_storage_save( identifier, &uhd ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "API:storage_save() failed for identifier='%s' in %s()", identifier, __FUNCTION__ );
		return TT_ERR;
	}
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int default__api__auth_otp_loadkey(const char* identifier, const char* challenge, const char* response, char* key, size_t* key_size) {
	char		challenge_raw[TT_OTP_TEXT_MAX+1] = {0};
	size_t		challenge_raw_size = sizeof(challenge_raw);
	char		response_raw[TT_OTP_TEXT_MAX+1] = {0};
	size_t		response_raw_size = sizeof(response_raw);
	size_t		i = 0;
	unsigned short	crc = 0;
	dflt_otp_t	otp = {0};

	TT_TRACE( "plugin/default", "%s(identifier='%s',challenge='%s',response='%s')", __FUNCTION__, identifier, challenge, response );
	if( identifier == NULL || challenge == NULL || response == NULL || key == NULL || key_size == NULL || *key_size == 0 ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameters in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	memset( key, '\0', *key_size );
	if( libtokentube_util_base32_decode( challenge, strnlen(challenge, TT_OTP_TEXT_MAX), challenge_raw, &challenge_raw_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "base32_decode() failed for identifier '%s' in %s()", identifier, __FUNCTION__ );
		return TT_ERR;
	}
	if( libtokentube_util_base32_decode( response, strnlen(response, TT_OTP_TEXT_MAX), response_raw, &response_raw_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "base32_decode() failed for identifier '%s' in %s()", identifier, __FUNCTION__ );
		return TT_ERR;
	}

	if( default__impl__otp_storage_load( identifier, &otp ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "default__impl__otp_storage_load() failed for '%s' in %s()", identifier, __FUNCTION__ );
		return TT_ERR;
	}
	if( otp.data_size == 0 || otp.data_size > sizeof(otp.data) || otp.data_size > *key_size ) {
		TT_LOG_ERROR( "plugin/default", "invalid otp.data_size for identifier '%s' in %s()", identifier, __FUNCTION__ );
		return TT_ERR;
	}
	if( otp.bits/8 == 0 || otp.bits/8 > sizeof(otp.data) || otp.bits/8 > *key_size ) {
		TT_LOG_ERROR( "plugin/default", "invalid otp.bits for identifier '%s' in %s()", identifier, __FUNCTION__ );
		return TT_ERR;
	}
	if( otp.bits/8 != (challenge_raw_size-2) || otp.bits/8 != (response_raw_size-2) ) {
		TT_LOG_ERROR( "plugin/default", "otp.bits=%d, challenge_raw_size=%d, response_raw_size=%d", otp.bits, challenge_raw_size-2, response_raw_size-2 );
		return TT_ERR;
	}
	*key_size = otp.data_size;
	for( i=0; i<*key_size; i++ ) {
		key[i] = challenge_raw[2+i%(otp.bits/8)] ^ response_raw[2+i%(otp.bits/8)] ^ otp.data[i];
	}
	for( i=0; i<*key_size; i++ ) {
		otp.data[i] = otp.data[i] ^ key[i];
	}
	otp.data_size = sizeof(otp.data);
	if( libtokentube_crypto_hash_impl( otp.hash, otp.data, otp.bits/8, otp.data, &otp.data_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_crypto_hash_impl() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	for( i=*key_size; i>0; i-- ) {
		otp.data[i-1] = otp.data[(i-1)%(otp.bits/8)] ^ key[i-1];
	}

	if( libtokentube_util_crc16( otp.data, otp.bits/8, &crc ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "API:crc16() failed for identifier '%s' in %s()", identifier, __FUNCTION__ );
		return TT_ERR;
	}
	if( (response_raw[0] & 0xff) != ((crc >> 0) & 0xff) || (response_raw[1] & 0xff) != ((crc >> 8) & 0xff) ) {
		TT_LOG_ERROR( "plugin/default", "crc(response)[%x%x] != hash(otp.data)[%x%x] in %s()", ((crc >> 0) & 0xff), ((crc >> 8) & 0xff), response_raw[0] & 0xff, response_raw[1] & 0xff, __FUNCTION__ );
		return TT_ERR;
	}
	otp.data_size = *key_size;
	if( default__impl__otp_storage_save( identifier, &otp ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "default__impl__otp_storage_save() failed for '%s' in %s()", identifier, __FUNCTION__ );
		return TT_ERR;
	}
	TT_DEBUG3( "plugin/default", "default__impl__otp_storage_save() successful for '%s' in %s()", identifier, __FUNCTION__ );
        return TT_OK;
}

