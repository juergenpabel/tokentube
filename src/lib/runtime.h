#ifndef __LIBTOKENTUBE_RUNTIME_H__
#define __LIBTOKENTUBE_RUNTIME_H__


#include <stdio.h>
#include <stdarg.h>
#include <tokentube/defines.h>
#include <tokentube/enums.h>


/*[TT_OK|TT_ERR]*/ int libtokentube_runtime_get_sysid(char* sysid, size_t* sysid_size);

/*[TT_OK|TT_ERR]*/ int libtokentube_runtime_get_type(tt_runtime_t* type);
/*[TT_OK|TT_ERR]*/ int libtokentube_runtime_get_bootdevice(char* bootdevice, size_t bootdevice_size);

/*[TT_OK|TT_ERR]*/ int libtokentube_runtime_is_pba(tt_status_t* status);
/*[TT_OK|TT_ERR]*/ int libtokentube_runtime_is_standard(tt_status_t* status);

/*[TT_OK|TT_ERR]*/ int libtokentube_runtime_broadcast(tt_event_t event, const char* identifier);

#define TT_LOG( level, name, message, args... ) libtokentube_runtime_log( level, name, message, ##args )
#define TT_LOG_FATAL( name, message, args... ) libtokentube_runtime_log( TT_LOG__FATAL, name, message, ##args )
#define TT_LOG_ERROR( name, message, args... ) libtokentube_runtime_log( TT_LOG__ERROR, name, message, ##args )
#define TT_LOG_WARN( name, message, args... ) libtokentube_runtime_log( TT_LOG__WARN, name, message, ##args )
#define TT_LOG_INFO( name, message, args... ) libtokentube_runtime_log( TT_LOG__INFO, name, message, ##args )

int libtokentube_runtime_log( tt_loglevel_t level, const char* source, const char* message, ... );
int libtokentube_runtime_vlog( tt_loglevel_t level, const char* source, const char* message, va_list args );

int libtokentube_runtime_debug( tt_debuglevel_t level, const char* source, const char* message, ... );
int libtokentube_runtime_vdebug( tt_debuglevel_t level, const char* source, const char* message, va_list args );
#define TT_DEBUG1( source, message, args... )	libtokentube_runtime_debug( TT_DEBUG__VERBOSITY1, source, message, ##args )
#define TT_DEBUG2( source, message, args... )	libtokentube_runtime_debug( TT_DEBUG__VERBOSITY2, source, message, ##args )
#define TT_DEBUG3( source, message, args... )	libtokentube_runtime_debug( TT_DEBUG__VERBOSITY3, source, message, ##args )
#define TT_DEBUG4( source, message, args... )	libtokentube_runtime_debug( TT_DEBUG__VERBOSITY4, source, message, ##args )
#define TT_DEBUG5( source, message, args... )	libtokentube_runtime_debug( TT_DEBUG__VERBOSITY5, source, message, ##args )
#define TT_TRACE( ... )				do {} while(0)


#ifdef TRACE
#undef TT_TRACE
#define TT_TRACE( source, message, args... )	libtokentube_runtime_trace( source, message, ##args )
static inline void libtokentube_runtime_trace( const char* source, const char* message, ... ) {
	va_list args;

	va_start( args, message );
	fprintf( stderr, "TRACE  [%-20s] ", source );
	vfprintf( stderr, message, args );
	fprintf( stderr, "\n" );
	fflush( stderr );
	va_end( args );
}
#endif


/*[TT_ERR|TT_OK]*/ int libtokentube_conf_print(char* buffer, size_t* buffer_size);
/*[TT_ERR|TT_OK]*/ int libtokentube_conf_get_filename(char* buffer, size_t* buffer_size);
/*[TT_ERR|TT_OK]*/ int libtokentube_conf_read_int(const char* name, int* value);
/*[TT_ERR|TT_OK]*/ int libtokentube_conf_read_size(const char* name, size_t* value);
/*[TT_ERR|TT_OK]*/ int libtokentube_conf_read_bool(const char* name, int* value);
/*[TT_ERR|TT_OK]*/ int libtokentube_conf_read_str(const char* name, char* value, size_t* value_size);
/*[TT_ERR|TT_OK]*/ int libtokentube_conf_read_list(const char* name, size_t index, char* value, size_t* value_size);

/*[TT_ERR|TT_OK]*/ int libtokentube_conf_read_plugin_library(const char* name, char* value, size_t* value_size);
/*[TT_ERR|TT_OK]*/ int libtokentube_conf_read_plugin_config(const char* name, char* value, size_t* value_size);
/*[TT_ERR|TT_OK]*/ int libtokentube_conf_read_plugin_filter_api(const char* plugin, size_t index, char* value, size_t* value_size);
/*[TT_ERR|TT_OK]*/ int libtokentube_conf_read_plugin_filter_event(const char* plugin, size_t index, char* value, size_t* value_size);


typedef struct cfg_t cfg_t;
typedef struct cfg_opt_t cfg_opt_t;
int  libtokentube_cfg_include(cfg_t* cfg, cfg_opt_t* opt, int argc, const char** argv);
int  libtokentube_cfg_print(cfg_t* cfg, char* buffer, size_t* buffer_size);
void libtokentube_cfg_error_log(cfg_t *cfg, const char *fmt, va_list ap);

/*[TT_ERR|TT_OK]*/ int libtokentube_runtime_install_pba(const char* type, const char* data);

#endif /* __LIBTOKENTUBE_RUNTIME_H__ */
