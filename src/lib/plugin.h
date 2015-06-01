#ifndef __LIBTOKENTUBE_PLUGIN_H__
#define __LIBTOKENTUBE_PLUGIN_H__


typedef struct {
        const char*	name;
        void*		elf;
        tt_plugin_t*	plugin;
} tt_module_t;


#define MAX_PLUGINS 31
extern tt_module_t* g_modules[MAX_PLUGINS+1];


/*[TT_OK|TT_ERR]*/ int libtokentube_plugin__pba_install_pre(const char* type, const char* path);
/*[TT_OK|TT_ERR]*/ int libtokentube_plugin__pba_install(const char* type, const char* path);
/*[TT_OK|TT_ERR]*/ int libtokentube_plugin__pba_install_post(const char* type, const char* path);

/*[TT_OK|TT_ERR]*/ int libtokentube_plugin__runtime_get_type(tt_runtime_t* result);
/*[TT_OK|TT_ERR]*/ int libtokentube_plugin__runtime_get_sysid(char* sysid, size_t* sysid_size);
/*[TT_OK|TT_ERR]*/ int libtokentube_plugin__runtime_get_bootdevice(char* result, size_t result_size);

/*[TT_OK|TT_ERR]*/ int libtokentube_plugin__file_load(tt_file_t type, const char* identifier, char* buffer, size_t* buffer_size);
/*[TT_OK|TT_ERR]*/ int libtokentube_plugin__file_save(tt_file_t type, const char* identifier, const char* buffer, size_t buffer_size);
/*[TT_OK|TT_ERR]*/ int libtokentube_plugin__file_exists(tt_file_t type, const char* identifier, tt_status_t* status);
/*[TT_OK|TT_ERR]*/ int libtokentube_plugin__file_delete(tt_file_t type, const char* identifier, tt_status_t* status);

/*[TT_OK|TT_ERR]*/ int libtokentube_plugin__user_create(const char* username, const char* password);
/*[TT_OK|TT_ERR]*/ int libtokentube_plugin__user_update(const char* username, const char* old_password, const char* new_password, tt_status_t* status);
/*[TT_OK|TT_ERR]*/ int libtokentube_plugin__user_delete(const char* username, tt_status_t* status);
/*[TT_OK|TT_ERR]*/ int libtokentube_plugin__user_exists(const char* username, tt_status_t* status);

/*[TT_OK|TT_ERR]*/ int libtokentube_plugin__user_key_add(const char* username, const char* password, const char* identifier, tt_status_t* status);
/*[TT_OK|TT_ERR]*/ int libtokentube_plugin__user_key_del(const char* username, const char* password, const char* identifier, tt_status_t* status);

/*[TT_OK|TT_ERR]*/ int libtokentube_plugin__user_execute_loadkey( const char* username, const char* password, const char* key_name, char* key, size_t* key_size );
/*[TT_OK|TT_ERR]*/ int libtokentube_plugin__user_execute_verify( const char* username, const char* password, tt_status_t* status );
/*[TT_OK|TT_ERR]*/ int libtokentube_plugin__user_execute_autoenrollment(const char* username, const char* password, tt_status_t* status);

/*[TT_OK|TT_ERR]*/ int libtokentube_plugin__otp_create(const char* identifier);
/*[TT_OK|TT_ERR]*/ int libtokentube_plugin__otp_exists(const char* identifier, tt_status_t* status);
/*[TT_OK|TT_ERR]*/ int libtokentube_plugin__otp_delete(const char* identifier, tt_status_t* status);
/*[TT_OK|TT_ERR]*/ int libtokentube_plugin__otp_execute_challenge(const char* identifier, char* challenge, size_t* challenge_size);
/*[TT_OK|TT_ERR]*/ int libtokentube_plugin__otp_execute_response(const char* identifier, const char* challenge, char* response, size_t* response_size);
/*[TT_OK|TT_ERR]*/ int libtokentube_plugin__otp_execute_apply(const char* identifier, const char* challenge, const char* response, char* key, size_t* key_size);


/*[TT_OK|TT_ERR]*/ int libtokentube_plugin__event_broadcast(tt_event_t event, const char* identifier);


#endif /* __LIBTOKENTUBE_PLUGIN_H__ */

