extern tt_plugin_t	g_self;
extern cfg_t*		g_cfg;
extern PyThreadState*	g_python;


int python__exec_script(cfg_t* cfg, const char* hook, PyObject**);

int python__storage_load(tt_file_t file, const char* identifier, char* key, size_t* key_len);
