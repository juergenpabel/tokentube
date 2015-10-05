#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/kdev_t.h>
#include <pwd.h>
#include <grp.h>
#include <syslog.h>
#include <tokentube/plugin.h>


extern tt_plugin_t	g_self;


__attribute__ ((visibility ("hidden")))
int test__impl__get_filename(tt_file_t type, const char* identifier, char* buffer, const size_t* buffer_size) {
	const char*	path = NULL;
        int		result = TT_ERR;

	g_self.library.api.runtime.system.debug( TT_DEBUG__VERBOSITY4, "plugin/test", "processing '%s' in test__get_filename()", identifier );
	switch( type ) {
		case TT_FILE__CONFIG_PBA:
		case TT_FILE__CONFIG_STANDARD:
			if( snprintf( buffer, (*buffer_size)-1, "boot/tokentube/%s", identifier ) > 0 ) {
				g_self.library.api.runtime.system.debug( TT_DEBUG__VERBOSITY5, "plugin/test", "returning '%s' in test__get_filename()", buffer );
				return TT_OK;
			} else {
				g_self.library.api.runtime.system.log( TT_LOG__ERROR, "plugin/test", "snprintf() failed in test__get_filename()" );
				return TT_ERR;
			}
			break;
		case TT_FILE__KEY:
			if( snprintf( buffer, (*buffer_size)-1, "etc/tokentube/keys/%s", identifier ) > 0 ) {
				g_self.library.api.runtime.system.debug( TT_DEBUG__VERBOSITY5, "plugin/test", "returning '%s' in test__get_filename()", buffer );
				return TT_OK;
			} else {
				g_self.library.api.runtime.system.log( TT_LOG__ERROR, "plugin/test", "snprintf() failed in test__get_filename()" );
				return TT_ERR;
			}
			break;
		case TT_FILE__USER:
			path = "boot/tokentube/user";
			break;
		case TT_FILE__OTP:
			path = "boot/tokentube/otp";
			break;
		case TT_FILE__UHD:
			path = "var/spool/tokentube/helpdesk";
			break;
		default:
			memset( buffer, '\0', *buffer_size );
	}
	if( path != NULL ) {
		if( snprintf( buffer, (*buffer_size)-1, "%s/%s.dat", path, identifier ) > 0 ) {
			g_self.library.api.runtime.system.debug( TT_DEBUG__VERBOSITY5, "plugin/test", "returning '%s' in test__get_filename()", buffer );
			result = TT_OK;
		} else {
			g_self.library.api.runtime.system.log( TT_LOG__ERROR, "plugin/test", "snprintf() failed in test__get_filename()" );
		}
	} else {
		g_self.library.api.runtime.system.log( TT_LOG__ERROR, "plugin/test", "unsupported value for type(%d) in test__get_filename()", type );
	}
	return result;
}

