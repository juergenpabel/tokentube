extern tt_plugin_t	g_self;
extern cfg_t*		g_cfg;
extern PyThreadState*	g_python;


int python__exec_script(cfg_t* cfg, const char* action, const char* type, const char* identifier, PyObject** in, PyObject** out);

int python__storage_load(tt_file_t file, const char* identifier, char* data, size_t* data_size);
int python__storage_save(tt_file_t file, const char* identifier, const char* data, size_t data_size);
