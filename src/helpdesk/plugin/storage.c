#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string.h>
#include <syslog.h>
#include <confuse.h>
#include <tokentube.h>
#include <tokentube/plugin.h>
#include "plugin.h"


extern tt_plugin_t 	g_self;


__attribute__ ((visibility ("hidden")))
int helpdesk__api__storage_save(tt_file_t file, const char* identifier, const char* data, size_t data_size) {
	if( file != TT_FILE__UHD ) {
		return TT_IGN;
	}
	if( identifier == NULL || data == NULL || data_size == 0 ) {
		g_self.library.api.runtime.system.log( TT_LOG__ERROR, "plugin/helpdesk", "invoked %s() without identifier", __FUNCTION__ );
		return TT_ERR;
	}
	if( g_conf_enabled & HELPDESK_SMTP ) {
		if( helpdesk__impl__storage_smtp_save( identifier, data, data_size ) != TT_OK ) {
			g_self.library.api.runtime.system.log( TT_LOG__ERROR, "plugin/helpdesk", "helpdesk__impl__storage_smtp_save() failed for '%s' in %s()", identifier, __FUNCTION__ );
		}
	}
	if( g_conf_enabled & HELPDESK_REST ) {
		if( helpdesk__impl__storage_rest_save( identifier, data, data_size ) != TT_OK ) {
			g_self.library.api.runtime.system.log( TT_LOG__ERROR, "plugin/helpdesk", "helpdesk__impl__storage_rest_save() failed for '%s' in %s()", identifier, __FUNCTION__ );
		}
	}
	return TT_OK;
}

