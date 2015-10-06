#ifndef __TT_PLUGIN_INTERFACE_H__
#define __TT_PLUGIN_INTERFACE_H__


typedef struct {
	/*[TT_OK|TT_ERR]*/ int (*get_sysid)(char* sysid, size_t* sysid_size);
	/*[TT_OK|TT_ERR]*/ int (*get_type)(tt_runtime_t* type);
	/*[TT_OK|TT_ERR]*/ int (*get_bootdevice)(char* bootdevice, size_t bootdevice_size);
} tt_plugin_api_runtime_t;


typedef struct {
	/*[TT_OK|TT_ERR]*/ int (*install_pre)(const char* type, const char* data);
	/*[TT_OK|TT_ERR]*/ int (*install_run)(const char* type, const char* data);
	/*[TT_OK|TT_ERR]*/ int (*install_post)(const char* type, const char* data);
} tt_plugin_api_pba_t;


typedef struct {
	tt_plugin_api_runtime_t    runtime;
	tt_plugin_api_pba_t        pba;
	tt_library_api_storage_t   storage;
	tt_library_api_database_t  database;
	tt_library_api_auth_t      auth;
} tt_plugin_api_t;


typedef struct {
	void (*created)(const char* identifier);
	void (*modified)(const char* identifier);
	void (*deleted)(const char* identifier);
} tt_plugin_events_user_t;


typedef struct {
	void (*created)(const char* identifier);
	void (*modified)(const char* identifier);
	void (*deleted)(const char* identifier);
} tt_plugin_events_otp_t;


typedef struct {
	void (*user_verified)(const char* identifier);
	void (*otp_applied)(const char* identifier);
} tt_plugin_events_auth_t;

typedef struct {
	tt_plugin_events_user_t  user;
	tt_plugin_events_otp_t   otp;
	tt_plugin_events_auth_t  auth;
} tt_plugin_events_t;


typedef struct {
	tt_plugin_api_t		api;
	tt_plugin_events_t	events;
} tt_plugin_interface_t;


#endif /* __TT_PLUGIN_INTERFACE_H__ */

