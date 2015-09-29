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
int helpdesk__api__storage_load(tt_file_t file, const char* identifier, char* data, size_t* data_size) {
	if( file != TT_FILE__HELPDESK ) {
		return TT_IGN;
	}
	if( identifier == NULL || data == NULL || data_size == 0 ) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/helpdesk", "invoked %s() without identifier", __FUNCTION__ );
		return TT_ERR;
	}
	if( g_conf_enabled & HELPDESK_FILE ) {
		if( helpdesk__impl__storage_file_load( identifier, data, data_size ) != TT_OK ) {
			g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/helpdesk", "helpdesk__impl__storage_file_load() failed for '%s' in %s()", identifier, __FUNCTION__ );
			return TT_ERR;
		}
	}
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int helpdesk__api__storage_save(tt_file_t file, const char* identifier, const char* data, size_t data_size) {
	if( file != TT_FILE__HELPDESK ) {
		return TT_IGN;
	}
	if( identifier == NULL || data == NULL || data_size == 0 ) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/helpdesk", "invoked %s() without identifier", __FUNCTION__ );
		return TT_ERR;
	}
	if( g_conf_enabled & HELPDESK_FILE ) {
		if( helpdesk__impl__storage_file_save( identifier, data, data_size ) != TT_OK ) {
			g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/helpdesk", "helpdesk__impl__storage_file_save() failed for '%s' in %s()", identifier, __FUNCTION__ );
		}
	}
	if( g_conf_enabled & HELPDESK_SMTP ) {
		if( helpdesk__impl__storage_smtp_save( identifier, data, data_size ) != TT_OK ) {
			g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/helpdesk", "helpdesk__impl__storage_smtp_save() failed for '%s' in %s()", identifier, __FUNCTION__ );
		}
	}
	return TT_OK;
}

