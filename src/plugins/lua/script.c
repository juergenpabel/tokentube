#include <string.h>
#include <confuse.h>
#include <syslog.h>
#include <gcrypt.h>
#include <tokentube/plugin.h>

extern tt_plugin_t	g_self;


__attribute__ ((visibility ("hidden")))
int lua__get_script(cfg_t* cfg, const char* hook, char* out, size_t out_len) {
        cfg_t*          section = NULL;
        char*           script = NULL;

        section = cfg_getsec( cfg, "hooks" );
        if( section == NULL ) {
                g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/lua", "configuration error (no hooks configured)" );
                return TT_ERR;
        }
        section = cfg_gettsec( section, "hook", hook );
        if( section == NULL ) {
                g_self.library.api.runtime.log( TT_LOG__WARN, "plugin/lua", "no configuration provided for hook '%s'", hook );
                return TT_ERR;
        }
        script = cfg_getstr( section, "script" );
        if( script == NULL ) {
                g_self.library.api.runtime.log( TT_LOG__WARN, "plugin/lua", "no script specified for hook '%s'", hook );
                return TT_ERR;
        }
	strncpy( out, script, out_len-1 );
	return TT_OK;
}

