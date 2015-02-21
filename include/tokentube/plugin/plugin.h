#ifndef __TT_PLUGIN_PLUGIN_H__
#define __TT_PLUGIN_PLUGIN_H__


typedef struct {
	tt_version_t	version;
	/*[TT_OK|TT_ERR]*/ int (*initialize)(void);
	/*[TT_OK|TT_ERR]*/ int (*configure)(const char* filename);
	/*[TT_OK|TT_ERR]*/ int (*finalize)(void);
} tt_plugin_meta_t;


typedef struct {
	tt_plugin_meta_t	meta;
	tt_plugin_interface_t	interface;
	tt_library_t		library;
} tt_plugin_t;


/*[TT_OK|TT_ERR]*/ int tt_plugin_register(tt_plugin_t* plugin);


#endif /* __TT_PLUGIN_PLUGIN_H__ */

