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
void default__event__otp_created(const char* identifier) {
//TODO	tt_status_t  status = TT_STATUS__UNDEFINED;

	TT_TRACE( "plugin/default", "%s(identifier='%s')", __FUNCTION__, identifier );
	if( identifier == NULL ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameters in %s()", __FUNCTION__ );
	}
	if( default__api__helpdesk_create( identifier ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "default__api__helpdesk_create failed in %s()", __FUNCTION__ );
	}
}


__attribute__ ((visibility ("hidden")))
void default__event__otp_deleted(const char* identifier) {
	tt_status_t  status = TT_STATUS__UNDEFINED;

	TT_TRACE( "plugin/default", "%s(identifier='%s')", __FUNCTION__, identifier );
	if( identifier == NULL ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameters in %s()", __FUNCTION__ );
	}
	if( default__api__helpdesk_delete( identifier, &status ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "default__api__helpdesk_create failed in %s()", __FUNCTION__ );
	}
}

