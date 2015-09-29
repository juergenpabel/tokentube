extern tt_plugin_t  g_self;

extern int          g_conf_enabled;
extern char*        g_conf_file_directory;
extern char*        g_conf_smtp_server;
extern char*        g_conf_smtp_from;
extern char*        g_conf_smtp_to;
extern char*        g_conf_smtp_subject;

typedef enum { HELPDESK_FILE = 1, HELPDESK_SMTP = 2, HELPDESK_REST = 4 } enabled_t;

int  helpdesk__api__storage_load(tt_file_t file, const char* identifier, char* data, size_t* data_size);
int  helpdesk__api__storage_save(tt_file_t file, const char* identifier, const char* data, size_t data_size);

int  helpdesk__impl__storage_file_load(const char* identifier, char* data, size_t* data_size);
int  helpdesk__impl__storage_file_save(const char* identifier, const char* data, size_t data_size);
int  helpdesk__impl__storage_smtp_save(const char* identifier, const char* data, size_t data_size);
int  helpdesk__impl__storage_rest_save(const char* identifier, const char* data, size_t data_size);

