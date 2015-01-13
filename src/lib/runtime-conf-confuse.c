#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <confuse.h>
#include <gcrypt.h>
#include "libtokentube.h"


__attribute__ ((visibility ("hidden")))
int libtokentube_cfg_include(cfg_t* cfg, cfg_opt_t* opt, int argc, const char** argv) {
	tt_status_t	status = TT_STATUS__UNDEFINED;
	tt_file_t	filetype = TT_FILE__UNDEFINED;
	char		buffer[TT_CONFIG_MAX] = {0};
	size_t		buffer_size = sizeof(buffer);
	gcry_md_hd_t	digest = NULL;
	void*		digest_result;
	int		hash_id, fd = 0;

	(void)opt; /* unused */
	TT_DEBUG2( "library/conf", "include('%s') invoked for file '%s' in %s()", argv[0], cfg->filename, __FUNCTION__ );
	if( libtokentube_runtime_is_pba( &status ) != TT_OK ) {
		TT_LOG_ERROR( "library/conf", "internal error in %s at %d", __FUNCTION__, __LINE__ );
		return 1;
	}
	if( status == TT_YES ) {
		if( strncmp( argv[0], "/boot/", 6 ) == 0 ) {
			filetype = TT_FILE__CONFIG_PBA;
		} else {
			filetype = TT_FILE__CONFIG_STANDARD;
		}
		if( libtokentube_plugin__file_load( filetype, argv[0], buffer, &buffer_size ) != TT_OK ) {
			TT_LOG_ERROR( "library/conf", "internal error in %s at %d", __FUNCTION__, __LINE__ );
			return 1;
		}
		if( cfg_parse_buf( cfg, buffer ) != 0 ) {
			TT_LOG_ERROR( "library/conf", "internal error in %s at %d", __FUNCTION__, __LINE__ );
			return 1;
		}
		return 0;
	}

	if( argc != 3 ) {
		TT_LOG_FATAL( "library/conf", "include() requires 3 parameters in %s()", __FUNCTION__ );
		return 1;
	}
	fd = open( argv[0], O_RDONLY );
	if( fd < 0 ) {
		TT_LOG_FATAL( "library/conf", "file '%s' could not be opened in %s()", argv[0], __FUNCTION__ );
		return 1;
	}
	if( read( fd, buffer, sizeof(buffer)-1 ) <= 0 ) {
		TT_LOG_FATAL( "library/conf", "file '%s' could not be read in %s()", argv[0], __FUNCTION__ );
		close ( fd );
		return 1;
	}
	close ( fd );
	hash_id = gcry_md_map_name( libtokentube_name2oid( argv[1] ) );
	if( gcry_md_open( &digest, hash_id, 0 ) != GPG_ERR_NO_ERROR ) {
		TT_LOG_FATAL( "library/conf", "unsupported hash-algorithm '%s' in %s()", (char*)argv[1], __FUNCTION__ );
		return 1;
	}
	gcry_md_write( digest, buffer, strnlen( buffer, sizeof(buffer) ) );
	digest_result = gcry_md_read( digest, 0 );
	if( digest_result == NULL ) {
		TT_LOG_FATAL( "library/conf", "gcry_md_read() failed in %s()", __FUNCTION__ );
		return 1;
	}
	buffer_size = sizeof(buffer);
	if( libtokentube_util_hex_encode( digest_result, gcry_md_get_algo_dlen(hash_id), buffer, &buffer_size ) != TT_OK ) {
		TT_LOG_FATAL( "library/conf", "libtokentube_util_hex_encode() failed in %s()", __FUNCTION__ );
		return 1;
	}
	if( strncmp( argv[2], buffer, buffer_size ) != 0 ) {
		TT_LOG_FATAL( "library/conf", "hash verification error for include('%s') '%s'!='%s'", argv[0], argv[2], cfg->filename, buffer );
		return 1;
	}
	gcry_md_close( digest );

	buffer_size = sizeof(buffer);
	if( libtokentube_plugin__file_load( TT_FILE__CONFIG_STANDARD, argv[0], buffer, &buffer_size ) != TT_OK ) {
		TT_LOG_ERROR( "library/conf", "libtokentube_plugin__file_load('%s') failed in %s()", argv[0], __FUNCTION__ );
		return 1;
	}
	return cfg_parse_buf( cfg, buffer );
}


__attribute__ ((visibility ("hidden")))
void libtokentube_cfg_error_log(cfg_t* cfg, const char *fmt, va_list ap) {
	char buffer[1024] = { 0 };
	(void) cfg;

	if( vsnprintf( buffer, sizeof(buffer)-1, fmt, ap ) > 0 ) {
		TT_LOG_ERROR( "library/conf", buffer );
	} else {
		TT_LOG_ERROR( "library/conf", "libconfuse: '%s' at '%s'", fmt, cfg_title( cfg ) );
	}
}


__attribute__ ((visibility ("hidden")))
int libtokentube_cfg_print(cfg_t* cfg, char* buffer, size_t* buffer_size) {
	FILE*	fp;

	if( cfg == NULL || buffer == NULL || buffer_size == NULL || *buffer_size == 0 ) {
		TT_LOG_ERROR( "library/conf", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	//TODO: fix this hack (fmemopen doesn't work though)
	fp = fopen( "/dev/null", "wb" );
	if( fp == NULL ) {
		TT_LOG_ERROR( "library/conf", "fopen(/dev/null) failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	memset( buffer,'\0', *buffer_size );
	setbuffer( fp, buffer, *buffer_size );
	cfg_print( cfg, fp );
	fclose( fp );
	*buffer_size = strlen( buffer );
	return TT_OK;
}

