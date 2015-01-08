extern tt_plugin_t	g_self;
extern char*		g_conf_smtp_server;
extern char*		g_conf_smtp_from;
extern char*		g_conf_smtp_to;
extern char*		g_conf_smtp_subject;

int  helpdesk__api__file_save(tt_file_t file, const char* identifier, const char* data, size_t data_size);

