#ifndef __LIBTOKENTUBE_PLUGIN_DEFAULT_H__
#define __LIBTOKENTUBE_PLUGIN_DEFAULT_H__


#define DEFAULT__FILESIZE_MAX 4095
#define DEFAULT__KEY_MAX 32
#define DEFAULT__PARAMETER_MAX 8
#define DEFAULT__CONSTRAINT_MAX 8

typedef struct {
	char      key[TT_IDENTIFIER_CHAR_MAX+1];
	size_t    key_size;
	char      value[TT_IDENTIFIER_CHAR_MAX+1];
	size_t    value_size;
} dflt_user_keyvalue_t;

typedef struct {
	char      cipher[TT_IDENTIFIER_CHAR_MAX+1];
	char      hash[TT_IDENTIFIER_CHAR_MAX+1];
	char      kdf[TT_IDENTIFIER_CHAR_MAX+1];
	size_t    kdf_iter;
} dflt_user_crypto_t;

typedef struct {
	dflt_user_keyvalue_t  parameter[DEFAULT__PARAMETER_MAX];
	dflt_user_keyvalue_t  constraint[DEFAULT__CONSTRAINT_MAX];
} dflt_user_cred_t;

typedef struct {
	dflt_user_keyvalue_t  data;
	dflt_user_keyvalue_t  parameter[DEFAULT__PARAMETER_MAX];
	dflt_user_keyvalue_t  constraint[DEFAULT__CONSTRAINT_MAX];
} dflt_user_key_t;

typedef struct {
	char      data[TT_DIGEST_BITS_MAX/8];
	size_t    data_size;
} dflt_user_hmac_t;

typedef struct {
	dflt_user_crypto_t crypto;
	dflt_user_cred_t   cred;
	dflt_user_key_t    key[DEFAULT__KEY_MAX];
	dflt_user_hmac_t   hmac;
} dflt_user_t;

#define TT_USER__UNDEFINED { {0}, {0}, {0}, {0} }


typedef struct {
	char	hash[TT_IDENTIFIER_CHAR_MAX+1];
	size_t	bits;
	char	data[TT_DIGEST_BITS_MAX/8];
	size_t	data_size;
} tt_otp_t;

#define TT_OTP__UNDEFINED { "", 0, "", 0 }


int  default__initialize(tt_plugin_t* plugin);
int  default__configure();
int  default__finalize();

int  default__api__runtime_get_sysid(char* sysid, size_t* sysid_size);
int  default__api__runtime_get_type(tt_runtime_t* result);
int  default__api__runtime_get_bootdevice(char* const buffer, const size_t buffer_size);
void default__event__runtime_log(tt_loglevel_t, const char* source, const char* message);
void default__event__runtime_debug(tt_debuglevel_t, const char* source, const char* message);

int  default__api__pba_install_pre(const char* type, const char* path);
int  default__api__pba_install_run(const char* type, const char* path);
int  default__api__pba_install_post(const char* type, const char* path);

int  default__api__storage_load(tt_file_t type, const char* identifier, char* buffer, size_t* buffer_size);
int  default__api__storage_save(tt_file_t type, const char* identifier, const char* buffer, size_t buffer_size);
int  default__api__storage_exists(tt_file_t type, const char* identifier, tt_status_t* status);
int  default__api__storage_delete(tt_file_t type, const char* identifier, tt_status_t* status);

int  default__api__user_create(const char* username, const char* password);
int  default__api__user_update(const char* username, const char* old_password, const char* new_password, tt_status_t* status);
int  default__api__user_delete(const char* username, tt_status_t* status);
int  default__api__user_exists(const char* username, tt_status_t* status);
int  default__api__user_key_add(const char* username, const char* password, const char* identifier, tt_status_t* status);
int  default__api__user_key_del(const char* username, const char* password, const char* identifier, tt_status_t* status);
int  default__api__user_execute_verify(const char* username, const char* password, tt_status_t* status);
int  default__api__user_execute_loadkey(const char* username, const char* password, const char* key_name, char* key, size_t* key_size);
int  default__api__user_execute_autoenrollment(const char* username, const char* password, tt_status_t* status);

int  default__impl__user_storage_load(const char* identifier, dflt_user_t* user);
int  default__impl__user_storage_save(const char* identifier, const dflt_user_t* user);
int  default__impl__user_storage_exists(const char* identifier, tt_status_t* status);
int  default__impl__user_storage_delete(const char* identifier, tt_status_t* status);

int  default__impl__user_key_encrypt(const char* username, const char* password, dflt_user_t* user, size_t key_offset);
int  default__impl__user_key_decrypt(const char* username, const char* password, dflt_user_t* user, size_t key_offset);
int  default__impl__user_hmac_set(const char* username, const char* password, dflt_user_t* user);
int  default__impl__user_hmac_test(const char* username, const char* password, dflt_user_t* user, tt_status_t* status);


int  default__storage_ext2fs_load(tt_file_t type, const char* filename, char* data, size_t* data_size);
int  default__storage_ext2fs_save(tt_file_t type, const char* filename, const char* data, const size_t data_size);
int  default__storage_ext2fs_exists(tt_file_t type, const char* filename, tt_status_t* status);
int  default__storage_ext2fs_delete(tt_file_t type, const char* filename, tt_status_t* status);
int  default__storage_posix_load(tt_file_t type, const char* filename, char* data, size_t* data_size);
int  default__storage_posix_save(tt_file_t type, const char* filename, const char* data, const size_t data_size);
int  default__storage_posix_exists(tt_file_t type, const char* filename, tt_status_t* status);
int  default__storage_posix_delete(tt_file_t type, const char* filename, tt_status_t* status);

int  default__impl__storage_get_filename(tt_file_t type, const char* identifier, char* data, const size_t data_size);

int  default__api__otp_create(const char* identifier);
int  default__api__otp_exists(const char* identifier, tt_status_t* status);
int  default__api__otp_delete(const char* identifier, tt_status_t* status);
int  default__api__helpdesk_challenge(const char* identifier, char* challenge, size_t* challenge_size);
int  default__api__helpdesk_response(const char* identifier, const char* challenge, char* response, size_t* response_size);
int  default__api__helpdesk_apply(const char* identifier, const char* challenge_text, const char* response_text, char* key, size_t* key_size);
void default__event__otp_created(const char* identifier);
void default__event__otp_deleted(const char* identifier);


int  default__impl__otp_load(const char* identifier, tt_otp_t* otp);
int  default__impl__otp_save(const char* identifier, tt_otp_t* otp);
int  default__impl__otp_delete(const char* identifier);


#endif /* __LIBTOKENTUBE_PLUGIN_DEFAULT_H__ */

