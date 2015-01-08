#ifndef __LIBTOKENTUBE_PLUGIN_DEFAULT_H__
#define __LIBTOKENTUBE_PLUGIN_DEFAULT_H__


#define DEFAULT__FILESIZE_MAX 4096

typedef struct {
	char	cipher[64];
	char	hash[64];
	char	kdf[64];
	size_t	kdf_iter;
	char    luks_data[TT_KEY_BITS_MAX/8];
	size_t  luks_data_len;
	char    luks_vrfy[TT_DIGEST_BITS_MAX/8];
	size_t  luks_vrfy_len;
} tt_user_t;

#define TT_USER__UNDEFINED { 0, 0, 0, 0 }


typedef struct {
	char	hash[64];
	size_t	bits;
	char	data[TT_DIGEST_BITS_MAX/8];
	size_t	data_len;
} tt_otp_t;

#define TT_OTP__UNDEFINED { 0, 0, 0, 0, 0 }


int  default__initialize(tt_plugin_t* plugin);
int  default__configure();
int  default__finalize();

int  default__api__runtime_get_sysid(char* sysid, size_t* sysid_size);
int  default__api__runtime_get_type(tt_runtime_t* result);
int  default__api__runtime_get_bootdevice(char* const buffer, const size_t buffer_size);
void default__event__runtime_log(tt_loglevel_t, const char* source, const char* message);
void default__event__runtime_debug(tt_debuglevel_t, const char* source, const char* message);

int  default__api__pba_install_pre(const char* type, const char* path);
int  default__api__pba_install(const char* type, const char* path);
int  default__api__pba_install_post(const char* type, const char* path);

int  default__api__storage_file_load(tt_file_t type, const char* identifier, char* buffer, size_t* buffer_len);
int  default__api__storage_file_save(tt_file_t type, const char* identifier, const char* buffer, size_t buffer_len);
int  default__api__storage_file_exists(tt_file_t type, const char* identifier, tt_status_t* status);
int  default__api__storage_file_delete(tt_file_t type, const char* identifier, tt_status_t* status);

int  default__api__user_create(const char* username, const char* password);
int  default__api__user_update(const char* username, const char* old_password, const char* new_password, tt_status_t* status);
int  default__api__user_delete(const char* username, tt_status_t* status);
int  default__api__user_exists(const char* username, tt_status_t* status);
int  default__api__user_execute_verify(const char* username, const char* password, tt_status_t* status);
int  default__api__user_execute_load(const char* username, const char* password, char* key, size_t* key_size);
int  default__api__user_execute_autoenrollment(const char* username, const char* password, tt_status_t* status);

int  default__impl__user_storage_load(const char* identifier, tt_user_t* user);
int  default__impl__user_storage_save(const char* identifier, const tt_user_t* user);
int  default__impl__user_storage_exists(const char* identifier, tt_status_t* status);
int  default__impl__user_storage_delete(const char* identifier, tt_status_t* status);

int  default__impl__user_verifier_set(const char* username, const char* password, tt_user_t* user);
int  default__impl__user_verifier_test(const char* username, const char* password, tt_user_t* user, tt_status_t* status);


int  default__api__luks_load(char* key, size_t* key_len);


int  default__ext2fs_load(const char* filename, char* data, size_t* data_size);
int  default__ext2fs_save(const char* filename, const char* data, const size_t data_size);
int  default__ext2fs_exists(const char* filename, tt_status_t* status);
int  default__ext2fs_delete(const char* filename, tt_status_t* status);
int  default__posix_load(const char* filename, char* data, size_t* data_size);
int  default__posix_save(const char* filename, const char* data, const size_t data_size);
int  default__posix_exists(const char* filename, tt_status_t* status);
int  default__posix_delete(const char* filename, tt_status_t* status);

int  default__get_filename(tt_file_t type, const char* identifier, char* data, const size_t data_size);

int  default__api__otp_create(const char* identifier);
int  default__api__otp_exists(const char* identifier, tt_status_t* status);
int  default__api__otp_delete(const char* identifier, tt_status_t* status);
int  default__api__otp_execute_challenge(const char* identifier, char* challenge, size_t* challenge_len);
int  default__api__otp_execute_response(const char* identifier, const char* challenge, char* response, size_t* response_len);
int  default__api__otp_execute_apply(const char* identifier, const char* challenge_text, const char* response_text, char* key, size_t* key_len);
void default__event__otp_created(const char* identifier);
void default__event__otp_deleted(const char* identifier);


int  default__impl__otp_load(const char* identifier, tt_otp_t* otp);
int  default__impl__otp_save(const char* identifier, tt_otp_t* otp);
int  default__impl__otp_delete(const char* identifier);


#endif /* __LIBTOKENTUBE_PLUGIN_DEFAULT_H__ */

