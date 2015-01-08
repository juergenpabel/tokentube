#ifndef __TT_PLUGIN_INTERFACE_H__
#define __TT_PLUGIN_INTERFACE_H__


typedef struct {
	/*[TT_OK|TT_ERR]*/ int (*get_sysid)(char* sysid, size_t* sysid_size);
	/*[TT_OK|TT_ERR]*/ int (*get_type)(tt_runtime_t* type);
	/*[TT_OK|TT_ERR]*/ int (*get_bootdevice)(char* bootdevice, size_t bootdevice_size);
} tt_plugin_api_runtime_t;


typedef struct {
	/*[TT_OK|TT_ERR]*/ int (*file_load)(tt_file_t type, const char* identifier, char* data, size_t* data_size);
	/*[TT_OK|TT_ERR]*/ int (*file_save)(tt_file_t type, const char* identifier, const char* data, size_t data_size);
	/*[TT_OK|TT_ERR]*/ int (*file_exists)(tt_file_t type, const char* identifier, tt_status_t* status);
	/*[TT_OK|TT_ERR]*/ int (*file_delete)(tt_file_t type, const char* identifier, tt_status_t* status);
	/*[TT_OK|TT_ERR]*/ int (*luks_load)(char* key, size_t* key_size);
} tt_plugin_api_storage_t;


typedef struct {
	/*[TT_OK|TT_ERR]*/ int (*install_pre)(const char* type, const char* data);
	/*[TT_OK|TT_ERR]*/ int (*install)(const char* type, const char* data);
	/*[TT_OK|TT_ERR]*/ int (*install_post)(const char* type, const char* data);
} tt_plugin_api_pba_t;


typedef struct {
	/*[TT_OK|TT_ERR]*/ int (*create)(const char* username, const char* password);
	/*[TT_OK|TT_ERR]*/ int (*update)(const char* username, const char* password, const char* new_password, tt_status_t* status);
	/*[TT_OK|TT_ERR]*/ int (*exists)(const char* username, tt_status_t* status);
	/*[TT_OK|TT_ERR]*/ int (*delete)(const char* username, tt_status_t* status);
	/*[TT_OK|TT_ERR]*/ int (*execute_verify)(const char* username, const char* password, tt_status_t* status);
	/*[TT_OK|TT_ERR]*/ int (*execute_load)(const char* username, const char* password, char* key, size_t* key_size);
	/*[TT_OK|TT_ERR]*/ int (*execute_autoenrollment)(const char* username, const char* password, tt_status_t* status);
} tt_plugin_api_user_t;


typedef struct {
	/*[TT_OK|TT_ERR]*/ int (*create)(const char* identifier);
	/*[TT_OK|TT_ERR]*/ int (*exists)(const char* identifier, tt_status_t* status);
	/*[TT_OK|TT_ERR]*/ int (*delete)(const char* identifier, tt_status_t* status);
	/*[TT_OK|TT_ERR]*/ int (*execute_challenge)(const char* identifier, char* challenge, size_t* challenge_size);
	/*[TT_OK|TT_ERR]*/ int (*execute_response)(const char* identifier, const char* challenge, char* response, size_t* response_size);
	/*[TT_OK|TT_ERR]*/ int (*execute_apply)(const char* identifier, const char* challenge, const char* response, char* key, size_t* key_size);
} tt_plugin_api_otp_t;


typedef struct {
	tt_plugin_api_runtime_t	runtime;
	tt_plugin_api_storage_t	storage;
	tt_plugin_api_pba_t	pba;
	tt_plugin_api_user_t	user;
	tt_plugin_api_otp_t	otp;
} tt_plugin_api_t;


typedef struct {
	void (*created)(const char* identifier);
	void (*verified)(const char* identifier);
	void (*updated)(const char* identifier);
	void (*deleted)(const char* identifier);
} tt_plugin_events_user_t;


typedef struct {
	void (*created)(const char* identifier);
	void (*applied)(const char* identifier);
	void (*deleted)(const char* identifier);
} tt_plugin_events_otp_t;


typedef struct {
	tt_plugin_events_user_t	user;
	tt_plugin_events_otp_t	otp;
} tt_plugin_events_t;


typedef struct {
	tt_plugin_api_t		api;
	tt_plugin_events_t	events;
} tt_plugin_interface_t;


#endif /* __TT_PLUGIN_INTERFACE_H__ */

