#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>
#include "libtokentube.h"

extern tt_debuglevel_t g_debug_level;


typedef struct lq {
	tt_loglevel_t	level;
	char*		source;
	char*		message;
	struct lq*	next;
} log_queue;


static int		g_env_log_fd = TT_UNINITIALIZED;
static tt_loglevel_t	g_env_log_level = TT_UNINITIALIZED;
static tt_loglevel_t	g_conf_log_level = TT_UNINITIALIZED;


static log_queue* g_syslog_queue = NULL;
static log_queue* g_target_queue = NULL;


static char* g_levels[] = { "undefined", "fatal", "error", "warn", "info" };
static int   log2syslog[] = { LOG_PRIMASK, LOG_CRIT, LOG_ERR, LOG_WARNING, LOG_INFO };


__attribute__ ((visibility ("hidden")))
int libtokentube_log_initialize() {
	const char*	env_log_level = NULL;
	const char*	env_log_target = NULL;
	FILE*		fp;
	size_t		i;

	openlog( NULL, LOG_PID|LOG_CONS, LOG_USER );
	if( getuid() != geteuid() ) {
		fprintf( stderr, "WARNING: IGNORING TT_LOG_LEVEL DUE TO setuid-exec in %s()", __FUNCTION__ );
		return TT_OK;
	}
	env_log_level = getenv( "TT_LOG_LEVEL" );
	if( env_log_level != NULL ) {
		g_env_log_level = atoi( env_log_level );
		if( g_env_log_level <= TT_LOG__UNDEFINED ) {
			for( i=0; i<sizeof(g_levels)/sizeof(g_levels[0]); i++ ) {
				if( strncasecmp( env_log_level, g_levels[i], 10 ) == 0) {
					g_env_log_level = i;
				}
			}
		} else {
			if( g_env_log_level >= sizeof(g_levels)/sizeof(g_levels[0]) ) {
				g_env_log_level = TT_LOG__INFO;
			}
		}
	}

	if( g_env_log_level > TT_LOG__UNDEFINED ) {
		env_log_target = getenv( "TT_LOG_TARGET" );
		if( env_log_target != NULL ) {
			if( strncasecmp( env_log_target, "FILE", 5 ) == 0) {
				env_log_target = getenv( "TT_LOG_TARGET_FILE" );
				if( env_log_target != NULL ) {
					g_env_log_fd = open( env_log_target, O_CREAT|O_WRONLY|O_APPEND, S_IRUSR|S_IWUSR );
					if( g_env_log_fd < 0 ) {
						fprintf( stderr, "TokenTube: open() failed for TT_LOG_TARGET_FILE=%s\n", env_log_target);
						return TT_ERR;
					}
				}
			} else if( strncasecmp( env_log_target, "EXEC", 5 ) == 0) {
				env_log_target = getenv( "TT_LOG_TARGET_EXEC" );
				if( env_log_target != NULL ) {
					fp = popen( env_log_target, "w" );
					if( fp == NULL ) {
						fprintf( stderr, "TokenTube: popen() failed for TT_LOG_TARGET_EXEC=%s\n", env_log_target);
						return TT_ERR;
					}
					g_env_log_fd = dup( fileno( fp ) );
					pclose( fp );
				}
			} else {
				g_env_log_level = TT_LOG__UNDEFINED;
			}
		}
	}
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_log_configure() {
	char		level[128] = { 0 };
	size_t		level_size = sizeof(level);
	log_queue*	iter = NULL;
	log_queue*	current = NULL;
	size_t		i = 0;

	if( libtokentube_conf_read_str( "runtime|log|level", level, &level_size ) != TT_OK ) {
		TT_LOG_ERROR( "library/runtime", "configuration error in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( level_size > 0 && level[0] != '\0' ) {
		for( i=0; i<sizeof(g_levels)/sizeof(g_levels[0]); i++ ) {
			if( strncasecmp( level, g_levels[i], level_size ) == 0) {
				TT_DEBUG1( "library/runtime", "setting syslog filter to '%s' for libtokentube_log()", level );
				g_conf_log_level = i;
			}
		}
	}
	if( (int)g_conf_log_level == TT_UNINITIALIZED ) {
		TT_LOG_INFO( "library/runtime", "no syslog filter configured for libtokentube_log()" );
		g_conf_log_level = TT_LOG__INFO;
	}
	iter = g_syslog_queue;
	while( iter != NULL ) {
		current = iter;
		iter = current->next;
		if( current->level > TT_LOG__UNDEFINED && current->level <= g_conf_log_level ) {
			syslog( log2syslog[current->level], "TokenTube[%s]: %s", current->source, current->message );
		}
		free( current->source );
		free( current->message );
		free( current );
	}
	g_syslog_queue = NULL;
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_log_finalize() {
	closelog();
	if( g_env_log_fd >= 0 ) {
		close( g_env_log_fd );
		g_env_log_fd = TT_UNINITIALIZED;
	}
	g_env_log_level = TT_LOG__UNDEFINED;
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_runtime_vlog( tt_loglevel_t level, const char* source, const char* message, va_list args ) {
	char		buffer[4096];
	log_queue**	current = NULL;
	va_list		args2;

	va_copy( args2, args );
	if( vsnprintf( buffer, sizeof(buffer)-1, message, args ) > 0 ) {
		if( g_env_log_level == TT_LOG__UNDEFINED ) {
			current = &g_target_queue;
			while( *current != NULL ) {
				current = &(*current)->next;
			}
			*current = malloc( sizeof(log_queue) );
			if( *current != NULL ) {
				(*current)->level = level;
				(*current)->source = strndup( source, 64 );
				(*current)->message = strndup( buffer, 1024 );
				(*current)->next = NULL;
			}
		} else {
			if( level <= g_env_log_level ) {
				dprintf( g_env_log_fd, "[%-5s] TokenTube[%s]: %s\n", g_levels[level], source, buffer );
			}
		}
		if( g_conf_log_level == TT_LOG__UNDEFINED ) {
			current = &g_syslog_queue;
			while( *current != NULL ) {
				current = &(*current)->next;
			}
			*current = malloc( sizeof(log_queue) );
			if( *current != NULL ) {
				(*current)->level = level;
				(*current)->source = strndup( source, 64 );
				(*current)->message = strndup( buffer, 1024 );
				(*current)->next = NULL;
			} else {
				syslog( level, "TokenTube[%s]: %s", source, buffer );
			}
		} else {
			if( level <= g_conf_log_level ) {
				syslog( log2syslog[level], "TokenTube[%s]: %s", source, buffer );
			}
		}
	} else {
		if( g_conf_log_level == TT_LOG__UNDEFINED || level <= g_conf_log_level ) {
			vsyslog( level, message, args2 );
		}
	}
	va_end( args2 );
	
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_runtime_log( tt_loglevel_t level, const char* source, const char* message, ... ) {
	int result = TT_ERR;
	va_list args;
	va_list args2;

	va_start( args, message );
	if( g_debug_level != TT_DEBUG__UNDEFINED ) {
		va_copy( args2, args );
		libtokentube_runtime_vdebug( TT_DEBUG__VERBOSITY1, source, message, args2 );
		va_end( args2 );
	}
	result = libtokentube_runtime_vlog( level, source, message, args );
	va_end( args );
	return result;
}

