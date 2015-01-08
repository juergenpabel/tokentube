#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include <tokentube/version.h>
#include <tokentube/defines.h>
#include <tokentube/enums.h>
#include <tokentube/library.h>


int     g_library_functions;
int	g_debug_level = TT_UNINITIALIZED;


char* libtokentube_name2oid(char* name) {
	return name;
}


int libtokentube_runtime_is_pba( tt_status_t* status ) {
	*status = TT_NO;
	return TT_OK;
}

int libtokentube_plugin__file_load( tt_file_t file, const char* filename, char* data, size_t* data_size) {
	int	fd = -1;
	
	if( file != TT_FILE__CONFIG_PBA && file != TT_FILE__CONFIG_STANDARD ) {
		return TT_ERR;
	}
	fd = open( filename, O_RDONLY );
	if( fd >= 0 ) {
		read( fd, data, *data_size );
		close( fd );
		return TT_OK;
	}
	return TT_ERR;
}


int libtokentube_runtime_debug( tt_debuglevel_t level, const char* name, const char* message, ... ) {
	va_list args;

	(void) level; /* unused */
	(void) name; /* unused */
	va_start( args, message );
	va_end( args );
	return TT_OK;
}


int libtokentube_runtime_log( int priority, const char* name, const char* message, ... ) {
	va_list args;

	(void) priority; /* unused */
	(void) name; /* unused */
	va_start( args, message );
//vprintf( message, args );
//printf("\n");
	va_end( args );
	return TT_OK;
}


int libtokentube_runtime_vlog( int priority, const char* name, const char* message, va_list args ) {
	(void) priority; /* unused */
	(void) name; /* unused */
	(void) message; /* unused */
	(void) args; /* unused */
//vprintf( message, args );
//printf("\n");
	return TT_OK;
}


int __wrap_fprintf(FILE *stream, const char *format, ...) {
        va_list args;

	(void) stream; /* unused */
        va_start( args, format );
        va_end( args );
	return 0;
}


int libtokentube_log_initialize() {
	return TT_OK;
}


int libtokentube_debug_initialize() {
	return TT_OK;
}


int libtokentube_runtime_initialize() {
	return TT_OK;
}


int libtokentube_plugin__initialize() {
	return TT_OK;
}


int libtokentube_event_initialize() {
	return TT_OK;
}


int libtokentube_crypto_initialize() {
	return TT_OK;
}


int libtokentube_user_initialize() {
	return TT_OK;
}


int libtokentube_otp_initialize() {
	return TT_OK;
}


int libtokentube_library_initialize() {
	return TT_OK;
}


int libtokentube_debug_configure() {
	return TT_OK;
}


int libtokentube_log_configure() {
	return TT_OK;
}


int libtokentube_runtime_configure() {
	return TT_OK;
}


int libtokentube_plugin__configure() {
	return TT_OK;
}


int libtokentube_event_configure() {
	return TT_OK;
}


int libtokentube_crypto_configure() {
	return TT_OK;
}


int libtokentube_user_configure() {
	return TT_OK;
}


int libtokentube_otp_configure() {
	return TT_OK;
}


int libtokentube_library_configure() {
	return TT_OK;
}


int libtokentube_log_finalize() {
	return TT_OK;
}


int libtokentube_debug_finalize() {
	return TT_OK;
}


int libtokentube_runtime_finalize() {
	return TT_OK;
}


int libtokentube_plugin__finalize() {
	return TT_OK;
}


int libtokentube_event_finalize() {
	return TT_OK;
}


int libtokentube_crypto_finalize() {
	return TT_OK;
}


int libtokentube_user_finalize() {
	return TT_OK;
}


int libtokentube_otp_finalize() {
	return TT_OK;
}


int libtokentube_library_finalize() {
	return TT_OK;
}


int libtokentube_library_discover( tt_library_t* api ) {
        (void) api; /* unused */
        return TT_OK;
}

