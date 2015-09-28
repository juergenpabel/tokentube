extern tt_plugin_t	g_self;
extern char*		g_conf_file_directory;
extern char*		g_conf_smtp_server;
extern char*		g_conf_smtp_from;
extern char*		g_conf_smtp_to;
extern char*		g_conf_smtp_subject;

int  helpdesk__api__storage_save(tt_file_t file, const char* identifier, const char* data, size_t data_size);

int  helpdesk__impl__file(const char* identifier, const char* data, size_t data_size);
int  helpdesk__impl__smtp(const char* identifier, const char* data, size_t data_size);
int  helpdesk__impl__rest(const char* identifier, const char* data, size_t data_size);

