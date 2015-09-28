#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <confuse.h>
#include <tokentube.h>
#include <tokentube/plugin.h>
#include "plugin.h"


extern tt_plugin_t 	g_self;


int helpdesk__impl__file(const char* identifier, const char* data, size_t data_size) {
	char filename[255];
	int fd;

	snprintf( filename, sizeof(filename)-1, "/tmp/%s.dat", identifier );
	fd = open( filename, O_CREAT|O_WRONLY|O_TRUNC, 0600 );
	if( fd < 0 ) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/helpdesk", "open() failed for '%s' in %s()", filename, __FUNCTION__ );
		return TT_ERR;
	}
	if( write( fd, data, data_size ) != (ssize_t)data_size ) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/helpdesk", "write() failed for '%s' in %s()", filename, __FUNCTION__ );
		close( fd );
		return TT_ERR;
	}
	close( fd );
	g_self.library.api.runtime.debug( TT_DEBUG__VERBOSITY4, "plugin/helpdesk", "successfully saved '%s' in %s()", filename, __FUNCTION__ );
	return TT_OK;
}

