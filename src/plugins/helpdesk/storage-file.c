#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <confuse.h>
#include <tokentube.h>
#include <tokentube/plugin.h>
#include "plugin.h"


int helpdesk__impl__storage_file_load(const char* identifier, char* data, size_t* data_size) {
	char filename[FILENAME_MAX+1] = {0};
	int  fd;

	if( identifier == NULL || data == NULL || data_size == NULL || *data_size == 0 ) {
		return TT_ERR;
	}
	snprintf( filename, sizeof(filename)-1, "%s/%s.dat", g_conf_file_directory, identifier );
	fd = open( filename, O_RDONLY );
	if( fd < 0 ) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/helpdesk", "open() failed for '%s' in %s()", filename, __FUNCTION__ );
		return TT_ERR;
	}
	if( read( fd, data, *data_size ) <= 0 ) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/helpdesk", "read() failed for '%s' in %s()", filename, __FUNCTION__ );
		close( fd );
		return TT_ERR;
	}
	*data_size = strnlen( data, *data_size );
	close( fd );
	g_self.library.api.runtime.debug( TT_DEBUG__VERBOSITY4, "plugin/helpdesk", "successfully loaded '%s' in %s()", filename, __FUNCTION__ );
	return TT_OK;
}



int helpdesk__impl__storage_file_save(const char* identifier, const char* data, size_t data_size) {
	char filename[FILENAME_MAX+1] = {0};
	int  fd;

	if( identifier == NULL || data == NULL || data_size == 0 ) {
		return TT_ERR;
	}
	snprintf( filename, sizeof(filename)-1, "%s/%s.dat", g_conf_file_directory, identifier );
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

