#define  _FILE_OFFSET_BITS 64
#define  LARGEFILE_SOURCE 1
#define   _LARGEFILE64_SOURCE 1
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <locale.h>
#include <netinet/in.h>
#include <resolv.h>
#include <netdb.h>
#include <confuse.h>
#include <gpgme.h>
#include <gcrypt.h>
#include <syslog.h>
#include <tokentube.h>
#include <tokentube/plugin.h>
#include "plugin.h"


static int initialize();
static int configure(const char* filename);
static int finalize();


__attribute__ ((visibility ("hidden")))
int		g_conf_enabled = 0;

__attribute__ ((visibility ("hidden")))
char*		g_conf_smtp_server = NULL;
__attribute__ ((visibility ("hidden")))
char*		g_conf_smtp_from = NULL;
__attribute__ ((visibility ("hidden")))
char*		g_conf_smtp_to = NULL;
__attribute__ ((visibility ("hidden")))
char*		g_conf_smtp_subject = NULL;

__attribute__ ((visibility ("hidden")))
char*		g_conf_rest_url = NULL;


__attribute__ ((visibility ("hidden")))
tt_plugin_t	g_self;
TT_PLUGIN_REGISTER( g_self, initialize, configure, finalize )


cfg_opt_t opt_config_helpdesk_rest[] = {
	CFG_STR("url",  "null", CFGF_NONE),
	CFG_STR("x509",  "null", CFGF_NONE),
	CFG_END()
};

cfg_opt_t opt_config_helpdesk_smtp[] = {
	CFG_STR("server",  "null", CFGF_NONE),
	CFG_STR("from",    "null", CFGF_NONE),
	CFG_STR("to",      "null", CFGF_NONE),
	CFG_STR("subject", "null", CFGF_NONE),
	CFG_END()
};

cfg_opt_t opt_config_helpdesk[] = {
	CFG_STR_LIST("enabled", NULL, CFGF_NONE),
	CFG_SEC("rest", opt_config_helpdesk_rest, CFGF_NODEFAULT),
	CFG_SEC("smtp", opt_config_helpdesk_smtp, CFGF_NODEFAULT),
	CFG_END()
};

cfg_opt_t opt_config[] = {
	CFG_SEC("helpdesk", opt_config_helpdesk, CFGF_NODEFAULT),
	CFG_END()
};


static int initialize() {
	return TT_OK;
}


static int configure(const char* filename) {
	const char* item = NULL;
	size_t      index = 0;
	cfg_t*      cfg = NULL;

	if( filename == NULL ) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/helpdesk", "INIT1" );
		return TT_ERR;
	}
	cfg = cfg_init( opt_config, CFGF_NONE );
	if( cfg_parse( cfg, filename ) != 0 ) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/helpdesk", "cfg_parse() failed for '%s'", filename );
		return TT_ERR;
	}
	item = cfg_getnstr( cfg, "helpdesk|enabled", index );
	while( item != NULL ) {
		if( strncasecmp( item, "smtp", 5 ) == 0 ) {
			g_conf_enabled |= HELPDESK_SMTP;
		}
		if( strncasecmp( item, "rest", 5 ) == 0 ) {
			g_conf_enabled |= HELPDESK_REST;
		}
		index++;
		item = cfg_getnstr( cfg, "helpdesk|enabled", index );
	}
	g_conf_smtp_server = strndup( cfg_getstr( cfg, "helpdesk|smtp|server" ), 128 );
	g_conf_smtp_from = strndup( cfg_getstr( cfg, "helpdesk|smtp|from" ), 128 );
	g_conf_smtp_to = strndup( cfg_getstr( cfg, "helpdesk|smtp|to" ), 128 );
	g_conf_smtp_subject = strndup( cfg_getstr( cfg, "helpdesk|smtp|subject" ), 128 );
	g_conf_rest_url = strndup( cfg_getstr( cfg, "helpdesk|rest|url" ), 2048 );
	g_self.interface.api.storage.save = helpdesk__api__storage_save;
	cfg_free( cfg );
	return TT_OK;
}


static int finalize() {
	free( g_conf_smtp_server );
	free( g_conf_smtp_from );
	free( g_conf_smtp_to );
	free( g_conf_smtp_subject );
	free( g_conf_rest_url );
	g_conf_smtp_server = NULL;
	g_conf_smtp_from = NULL;
	g_conf_smtp_to = NULL;
	g_conf_smtp_subject = NULL;
	g_conf_rest_url = NULL;
	return TT_OK;
}

