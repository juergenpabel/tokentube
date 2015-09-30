#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <confuse.h>
#include <curl/curl.h>
#include <tokentube.h>
#include <tokentube/plugin.h>
#include "plugin.h"


extern tt_plugin_t 	g_self;


int helpdesk__impl__storage_rest_save(const char* identifier, const char* data, size_t data_size) {
	CURL*    curl = NULL;
	CURLcode code;

	if( identifier == NULL || data == NULL || data_size == 0 ) {
		return TT_ERR;
	}
	curl = curl_easy_init();
	if( curl == NULL ) {
	}
	curl_easy_setopt( curl, CURLOPT_URL, g_conf_rest_url );
	curl_easy_setopt( curl, CURLOPT_POSTFIELDS, data );
	curl_easy_setopt( curl, CURLOPT_POSTFIELDSIZE, (long)data_size );
	code = curl_easy_perform( curl );
	if( code != CURLE_OK ) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/helpdesk", "curl_easy_perform() failed: %s", curl_easy_strerror( code ) );
		curl_easy_cleanup( curl );
		return TT_ERR;
	}
	curl_easy_cleanup( curl );
	return TT_OK;
}

