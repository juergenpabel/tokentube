#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <syslog.h>
#include <errno.h>
#include <confuse.h>
#include <tokentube.h>
#include "pba.h"


int pba_plain(cfg_t* cfg, tt_library_t* library, const char* identifier, char* key, size_t* key_size) {
	const char*     prompt_password = NULL;

(void)identifier;//TODO
	prompt_password = cfg_getstr( cfg, "plain|prompt-password" );
	if( pba_plymouth_plain( "message-plain", prompt_password, key, key_size) != TT_OK ) {
		library->api.runtime.system.log( TT_LOG__ERROR, "pba", "pba_plymouth_plain() returned TT_ERR" );
		return TT_ERR;
	}
	return TT_OK;
}

