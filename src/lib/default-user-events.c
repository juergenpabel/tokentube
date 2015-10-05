#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <confuse.h>
#include <gcrypt.h>
#include "libtokentube.h"


__attribute__ ((visibility ("hidden")))
void default__event__user_created(const char* identifier) {
//TODO	tt_status_t  status = TT_STATUS__UNDEFINED;

	TT_TRACE( "plugin/default", "%s(identifier='%s')", __FUNCTION__, identifier );
	if( identifier == NULL ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameters in %s()", __FUNCTION__ );
	}
//TODO: create symlink in /etc/tokentube/users
}


__attribute__ ((visibility ("hidden")))
void default__event__user_deleted(const char* identifier) {
//TODO	tt_status_t  status = TT_STATUS__UNDEFINED;

	TT_TRACE( "plugin/default", "%s(identifier='%s')", __FUNCTION__, identifier );
	if( identifier == NULL ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameters in %s()", __FUNCTION__ );
	}
//TODO: delete symlink in /etc/tokentube/users
}

