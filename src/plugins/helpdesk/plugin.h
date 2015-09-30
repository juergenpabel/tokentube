extern tt_plugin_t  g_self;

extern int          g_conf_enabled;
extern char*        g_conf_file_directory;
extern char*        g_conf_smtp_server;
extern char*        g_conf_smtp_from;
extern char*        g_conf_smtp_to;
extern char*        g_conf_smtp_subject;
extern char*        g_conf_rest_url;

typedef enum { HELPDESK_SMTP = 1, HELPDESK_REST = 2 } enabled_t;

int  helpdesk__api__storage_save(tt_file_t file, const char* identifier, const char* data, size_t data_size);

int  helpdesk__impl__storage_smtp_save(const char* identifier, const char* data, size_t data_size);
int  helpdesk__impl__storage_rest_save(const char* identifier, const char* data, size_t data_size);

