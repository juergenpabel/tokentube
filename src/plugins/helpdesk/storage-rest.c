#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <confuse.h>
#include <tokentube.h>
#include <tokentube/plugin.h>
#include "plugin.h"


extern tt_plugin_t 	g_self;


int helpdesk__impl__storage_rest_save(const char* identifier, const char* data, size_t data_size) {

	if( identifier == NULL || data == NULL || data_size == 0 ) {
		return TT_ERR;
	}
	return TT_OK;
}

