#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <syslog.h>
#include <limits.h>
#include <tokentube/plugin.h>


static int initialize();
static int finalize();

int	test__impl__get_filename(tt_file_t type, const char* identifier, char* buffer, size_t* buffer_size);
int	test__impl__posix_load(const char* filename, char* buffer, size_t* buffer_size);
int	test__impl__posix_save(const char* filename, const char* buffer, const size_t buffer_size);
int	test__impl__posix_exists(const char* filename, tt_status_t* status);
int	test__impl__posix_delete(const char* filename, tt_status_t* status);


__attribute__ ((visibility ("hidden")))
tt_plugin_t	g_self;

TT_PLUGIN_REGISTER( g_self, initialize, NULL, finalize )


static int test__api__file_load(tt_file_t type, const char* identifier, char* buffer, size_t* buffer_size) {
	char	filename[1024] = {0};
	size_t	filename_size = sizeof(filename);
        int	result = TT_ERR;

	g_self.library.api.runtime.debug( TT_DEBUG__VERBOSITY3, "plugin/test", "test__api__file_load() invoked for identifier '%s'", identifier );
	if( test__impl__get_filename( type, identifier, filename, &filename_size ) == TT_OK ) {
		result = test__impl__posix_load( filename, buffer, buffer_size );
	} else {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/test", "test__impl__get_filename() failed for '%s' in test__api__file_load", identifier );
	}
	return result;
}


static int test__api__file_save(tt_file_t type, const char* identifier, const char* buffer, size_t buffer_size) {
	char	filename[1024] = {0};
	size_t	filename_size = sizeof(filename);
        int result = TT_ERR;

	g_self.library.api.runtime.debug( TT_DEBUG__VERBOSITY3, "plugin/test", "test__api__file_save() invoked for identifier '%s'", identifier );
	if( test__impl__get_filename( type, identifier, filename, &filename_size ) == TT_OK ) {
		result = test__impl__posix_save( &filename[0], buffer, buffer_size );
	} else {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/test", "test__impl__get_filename() failed for '%s' in test__api__filesave", identifier );
	}
	return result;
}


static int test__api__file_exists(tt_file_t type, const char* identifier, tt_status_t* status) {
	char	filename[1024] = {0};
	size_t	filename_size = sizeof(filename);
        int    result = TT_ERR;

	g_self.library.api.runtime.debug( TT_DEBUG__VERBOSITY3, "plugin/test", "test__api__file_exists() invoked for identifier '%s'", identifier );
	if( test__impl__get_filename( type, identifier, filename, &filename_size ) == TT_OK ) {
		result = test__impl__posix_exists( filename, status );
	} else {
		g_self.library.api.runtime.debug( TT_DEBUG__VERBOSITY2, "plugin/test", "test__impl__get_filename() failed for '%s' in test__api__file_exists", identifier );
	}
	return result;
}


static int test__api__file_delete(tt_file_t type, const char* identifier, tt_status_t* status) {
	char	filename[1024] = {0};
	size_t	filename_size = sizeof(filename);
        int result = TT_ERR;

	g_self.library.api.runtime.debug( TT_DEBUG__VERBOSITY3, "plugin/test", "test__api__file_delete() invoked for identifier '%s'", identifier );
	if( test__impl__get_filename( type, identifier, filename, &filename_size ) == TT_OK ) {
		result = test__impl__posix_delete( filename, status );
	} else {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/test", "test__impl__get_filename() failed for '%s' in test__api__file_delete", identifier );
	}
	return result;
}


static int test__api__user_execute_autoenrollment(const char* username, const char* password, tt_status_t* status) {
	if( g_self.library.api.user.create( username, password ) != TT_OK ) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/default", "API:user.create failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	*status = TT_STATUS__YES;
	return TT_OK;
}


static int initialize() {
	g_self.interface.api.storage.file_load = test__api__file_load;
	g_self.interface.api.storage.file_save = test__api__file_save;
	g_self.interface.api.storage.file_delete = test__api__file_delete;
	g_self.interface.api.storage.file_exists = test__api__file_exists;
	g_self.interface.api.user.execute_autoenrollment = test__api__user_execute_autoenrollment;
	return TT_OK;
}


static int finalize() {
	return TT_OK;
}

