#ifndef __TT_LIBRARY_LIBRARY_H__
#define __TT_LIBRARY_LIBRARY_H__


#include <tokentube/defines.h>
#include <tokentube/enums.h>


typedef struct {
	int (*log)( tt_loglevel_t level, const char* source, const char* message, ... );
	int (*debug)( tt_debuglevel_t level, const char* source, const char* message, ... );
	int (*broadcast)(tt_event_t event, const char* identifier);
	int (*sysid)(char* sysid, size_t* sysid_size);
	int (*install_pba)(const char* type, const char* data);
	int (*conf_read_int)(const char* name, int* value);
	int (*conf_read_str)(const char* name, char* value, size_t* value_size);
} tt_library_api_runtime_t;


typedef struct {
	int (*random)(const void* data, size_t data_size);
	int (*encrypt)(void* data, const size_t data_size, const void* key, const size_t key_size, const void* iv, const size_t iv_size);
	int (*decrypt)(void* data, const size_t data_size, const void* key, const size_t key_size, const void* iv, const size_t iv_size);
	int (*hash)(const void* in, size_t in_size, void* out, size_t* out_size);
	int (*hmac)(const void* in, size_t in_size, const void* key, size_t key_size, void* out, size_t* out_size);
	int (*kdf)(const void* salt, size_t salt_size, const void* in, size_t in_size, void* out, size_t* out_size);
} tt_library_api_crypto_t;


typedef struct {
	int (*load)(tt_file_t type, const char* identifier, char* data, size_t* data_size);
	int (*save)(tt_file_t type, const char* identifier, const char* data, size_t data_size);
	int (*exists)(tt_file_t type, const char* identifier, tt_status_t* status);
	int (*delete)(tt_file_t type, const char* identifier, tt_status_t* status);
} tt_library_api_storage_t;


typedef struct {
	int (*create)(const char* username, const char* password);
	int (*update)(const char* username, const char* password, const char* new_password, tt_status_t* status);
	int (*exists)(const char* username, tt_status_t* status);
	int (*delete)(const char* username, tt_status_t* status);
	int (*key_add)(const char* username, const char* password, const char* identifier, tt_status_t* status);
	int (*key_del)(const char* username, const char* password, const char* identifier, tt_status_t* status);
	int (*execute_verify)(const char* username, const char* password, tt_status_t* status);
	int (*execute_loadkey)(const char* username, const char* password, const char* key_name, char* key, size_t* key_size);
	int (*execute_autoenrollment)(const char* username, const char* password, tt_status_t* status);
} tt_library_api_user_t;


typedef struct {
	int (*create)(const char* identifier);
	int (*update)(const char* identifier, const char* key, size_t key_size, const char* new_key, size_t new_key_size, tt_status_t* status);
	int (*exists)(const char* identifier, tt_status_t* status);
	int (*delete)(const char* identifier, tt_status_t* status);
} tt_library_api_otp_t;


typedef struct {
	int (*challenge)(const char* identifier, char* challenge, size_t* challenge_size);
	int (*response)(const char* identifier, const char* challenge, char* response, size_t* response_size);
	int (*apply)(const char* identifier, const char* challenge, const char* response, char* key, size_t* key_size);
} tt_library_api_helpdesk_t;


typedef struct {
	int (*crc16)(const void const* input, size_t input_size, unsigned short* result);
	int (*hex_encode)(const void* bytes, size_t bytes_size, void* text, size_t* text_size);
	int (*hex_decode)(const void* text, size_t text_size, void* bytes, size_t* bytes_size);
	int (*base32_encode)(const void* bytes, size_t bytes_size, void* text, size_t* text_size);
	int (*base32_decode)(const void* text, size_t text_size, void* bytes, size_t* bytes_size);
	int (*base64_encode)(const void* bytes, size_t bytes_size, void* text, size_t* text_size);
	int (*base64_decode)(const void* text, size_t text_size, void* bytes, size_t* bytes_size);
	int (*posix_mkdir)(const char* path);
	int (*posix_copy)(const char* src, const char* dst);
} tt_library_api_util_t;


typedef struct {
	tt_library_api_runtime_t   runtime;
	tt_library_api_crypto_t	   crypto;
	tt_library_api_storage_t   storage;
	tt_library_api_util_t      util;
	tt_library_api_user_t      user;
	tt_library_api_otp_t       otp;
	tt_library_api_helpdesk_t  helpdesk;
} tt_library_api_t;


typedef struct {
	tt_version_t		version;
	tt_library_api_t	api;
} tt_library_t;


#endif /* __TT_LIBRARY_LIBRARY_H__ */

