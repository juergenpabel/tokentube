lua_State*      g_LUA; 
cfg_t*          g_cfg; 
tt_plugin_t     g_self;


int lua__get_script(cfg_t* cfg, const char* hook, char* out, size_t out_size);

int lua__storage_load(tt_file_t file, const char* identifier, char* key, size_t* key_size);
int lua__storage_save(tt_file_t file, const char* identifier, const char* key, size_t key_size);

