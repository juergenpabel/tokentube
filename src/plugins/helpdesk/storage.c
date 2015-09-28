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
//	const char* method = NULL;

	if( file != TT_FILE__HELPDESK ) {
		return TT_IGN;
	}
	if( identifier == NULL || data == NULL || data_size == 0 ) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/helpdesk", "invoked %s() without identifier", __FUNCTION__ );
		return TT_ERR;
	}
	if( helpdesk__impl__file( identifier, data, data_size ) != TT_OK ) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/helpdesk", "TODO in %s()", __FUNCTION__ );
		return TT_ERR;
	}
//TODO: iterate over enabled and call functions
	return TT_OK;
}

