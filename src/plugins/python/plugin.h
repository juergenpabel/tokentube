extern tt_plugin_t	g_self;
extern cfg_t*		g_cfg;
extern PyThreadState*	g_python;


int python__exec_script(cfg_t* cfg, const char* hook, PyObject**);

int python__luks_load(char* key, size_t* key_len);
