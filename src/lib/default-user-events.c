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
	char    source[FILENAME_MAX+1] =  {0};
	size_t  source_size = sizeof(source);
	char    target[FILENAME_MAX+1] =  {0};
	size_t  target_size = sizeof(target);

	TT_TRACE( "plugin/default", "%s(identifier='%s')", __FUNCTION__, identifier );
	if( identifier == NULL ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameters in %s()", __FUNCTION__ );
		return;
	}
	if( libtokentube_conf_read_str( "user|symlink-directory", source, &source_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_conf_read_str() failed in %s()", __FUNCTION__ );
		return;
	}
	if( source[0] != '\0' ) {
		if( source_size + 1 + strnlen( identifier, TT_IDENTIFIER_CHAR_MAX ) >= sizeof(source) ) {
			TT_LOG_ERROR( "plugin/default", "buffer too small in %s()", __FUNCTION__ );
			return;
		}
		source[source_size] = '/';
		strncpy( &source[source_size+1], identifier, sizeof(source)-(source_size+1)-1 );
		if( default__impl__storage_get_filename( TT_FILE__USER, identifier, target, target_size ) != TT_OK ) {
			TT_LOG_ERROR( "plugin/default", "default__impl__storage_get_filename() failed in %s()", __FUNCTION__ );
			return;
		}
		if( symlink( target, source ) < 0 ) {
			TT_LOG_ERROR( "plugin/default", "symlink() failed in %s()", __FUNCTION__ );
			return;
		}
	}
}


__attribute__ ((visibility ("hidden")))
void default__event__user_deleted(const char* identifier) {
	char    source[FILENAME_MAX+1] =  {0};
	size_t  source_size = sizeof(source);

	TT_TRACE( "plugin/default", "%s(identifier='%s')", __FUNCTION__, identifier );
	if( identifier == NULL ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameters in %s()", __FUNCTION__ );
		return;
	}
	if( libtokentube_conf_read_str( "user|symlink-directory", source, &source_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_conf_read_str() failed in %s()", __FUNCTION__ );
		return;
	}
	if( source[0] != '\0' ) {
		if( source_size + 1 + strnlen( identifier, TT_IDENTIFIER_CHAR_MAX ) >= sizeof(source) ) {
			TT_LOG_ERROR( "plugin/default", "buffer too small in %s()", __FUNCTION__ );
			return;
		}
		source[source_size] = '/';
		strncpy( &source[source_size+1], identifier, sizeof(source)-(source_size+1)-1 );
		if( unlink( source ) < 0 ) {
			TT_LOG_ERROR( "plugin/default", "symlink() failed in %s()", __FUNCTION__ );
			return;
		}
	}
}

