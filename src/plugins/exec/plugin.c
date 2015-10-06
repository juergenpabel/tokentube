#define _GNU_SOURCE
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <confuse.h>
#include <tokentube.h>
#include <tokentube/plugin.h>


static int initialize();
static int configure(const char* filename);
static int finalize();


static cfg_t*		g_cfg = NULL;

static tt_plugin_t	g_self;
TT_PLUGIN_REGISTER( g_self, initialize, configure, finalize )


static cfg_opt_t opt_exec_events_event[] = {
	CFG_STR("exec",  NULL, CFGF_NODEFAULT),
	CFG_BOOL("detach",  cfg_false, CFGF_NONE),
	CFG_END()
};

static cfg_opt_t opt_exec_events[] = {
	CFG_SEC("event", opt_exec_events_event, CFGF_MULTI|CFGF_TITLE),
	CFG_END()
};

static cfg_opt_t opt_exec_api_exec[] = {
	CFG_STR("exec",  NULL, CFGF_NODEFAULT),
	CFG_STR("parameter",  NULL, CFGF_NONE),
	CFG_END()
};

static cfg_opt_t opt_exec_api[] = {
	CFG_SEC("runtime", opt_exec_api_exec, CFGF_MULTI|CFGF_TITLE),
	CFG_SEC("storage", opt_exec_api_exec, CFGF_MULTI|CFGF_TITLE),
	CFG_END()
};

static cfg_opt_t opt_exec[] = {
	CFG_SEC("api", opt_exec_api, CFGF_NONE),
	CFG_SEC("events", opt_exec_events, CFGF_NONE),
	CFG_END()
};


static int exec__exec( cfg_t* cfg, const char* event, const char* identifier ) {
        cfg_t*		section = NULL;
	char*		exec = NULL;
	char*           argv[6] = { NULL, "tokentube", "plugin/exec", (char*)event, (char*)identifier, NULL };
	char*           envp[1] = { NULL };
	struct stat	st;
	int		fd, pid = 0;
	int		result = TT_ERR;

	section = cfg_getsec( cfg, "events" );
	if( section == NULL ) {
		g_self.library.api.runtime.system.log( TT_LOG__ERROR, "plugin/exec", "cfg_getsec() failed for event '%s'", event );
		return TT_ERR;
	}
	section = cfg_gettsec( section, "event", event );
	if( section == NULL ) {
		g_self.library.api.runtime.system.log( TT_LOG__INFO, "plugin/exec", "no configuration for event '%s'", event );
		return TT_OK;
	}
	exec = cfg_getstr( section, "exec" );
	if( exec == NULL ) {
		g_self.library.api.runtime.system.log( TT_LOG__WARN, "plugin/exec", "no executable configured for event '%s'", event );
		return TT_ERR;
	}
	fd = open( exec, O_RDONLY );
	if( fd < 0 ) {
		g_self.library.api.runtime.system.log( TT_LOG__ERROR, "plugin/exec", "open() failed for '%s' for event '%s'", exec, event );
		return TT_ERR;
	}
	if( fstat( fd, &st ) < 0 ) {
		g_self.library.api.runtime.system.log( TT_LOG__ERROR, "plugin/exec", "fstat() failed for '%s' for event '%s'", exec, event );
		close( fd );
		return TT_ERR;
	}
	if( (st.st_mode & S_IEXEC) == 0 ) {
		g_self.library.api.runtime.system.log( TT_LOG__ERROR, "plugin/exec", "file ('%s') not executable for event '%s'", exec, event );
		close( fd );
		return TT_ERR;
	}
	argv[0] = exec;
	pid = fork();
	switch( pid ) {
		case -1:
			g_self.library.api.runtime.system.log( TT_LOG__ERROR, "plugin/exec", "failed to create new process" );
			close( fd );
			break;
		case 0:
			g_self.library.api.runtime.system.debug( TT_DEBUG__VERBOSITY4, "plugin/exec", "fork()ed as child in exec__exec()" );
			if( cfg_getbool( section, "detach" ) == cfg_true ) {
				g_self.library.api.runtime.system.debug( TT_DEBUG__VERBOSITY4, "plugin/exec", "detaching from parent in %s()", __FUNCTION__ );
				close( 2 );
				close( 1 );
				close( 0 );
				setsid();
			}
			g_self.library.api.runtime.system.debug( TT_DEBUG__VERBOSITY3, "plugin/exec", "executing '%s' as child in exec__exec()", exec );
			if( fexecve( fd, argv, envp ) < 0 )  {
				switch( errno ) {
					case EINVAL:
						g_self.library.api.runtime.system.log( TT_LOG__WARN, "plugin/exec", "fexecve(%s) failed with EINVAL in exec__exec()", exec );
						break;
					case ENOSYS:
						g_self.library.api.runtime.system.log( TT_LOG__WARN, "plugin/exec", "fexecve(%s) failed with ENOSYS in exec__exec()", exec );
						break;
					default:
						g_self.library.api.runtime.system.log( TT_LOG__WARN, "plugin/exec", "fexecve(%s) failed with %d in exec__exec()", exec, errno );
					}
			}
			close( fd );
			execve( argv[0], argv, envp );
			g_self.library.api.runtime.system.log( TT_LOG__ERROR, "plugin/exec", "execve(%s) failed in exec__exec()", exec );
			exit( -1 );
		default:
			g_self.library.api.runtime.system.debug( TT_DEBUG__VERBOSITY5, "plugin/exec", "fork()ed as parent in exec__exec()" );
			close( fd );
			if( cfg_getbool( section, "detach" ) == cfg_false ) {
				g_self.library.api.runtime.system.debug( TT_DEBUG__VERBOSITY3, "plugin/exec", "waiting on child (pid=%d) in exec__exec()...", pid );
				waitpid( pid, NULL, 0 );
				g_self.library.api.runtime.system.debug( TT_DEBUG__VERBOSITY3, "plugin/exec", "...child (pid=%d) finished in exec__exec()", pid );
			} else {
				g_self.library.api.runtime.system.debug( TT_DEBUG__VERBOSITY3, "plugin/exec", "letting child (pid=%d) do on its own in exec__exec()", pid );
			}
			result = TT_OK;
	}
	return result;
}


static void exec__event__user_created(const char* identifier) {
	g_self.library.api.runtime.system.log( TT_LOG__INFO, "plugin/exec", "event 'USER-CREATED' received for identifier '%s'", identifier );
	exec__exec( g_cfg, "USER-CREATED", identifier );
}


static void exec__event__user_modified(const char* identifier) {
	g_self.library.api.runtime.system.log( TT_LOG__INFO, "plugin/exec", "event 'USER-MODIFIED' received for identifier '%s'", identifier );
	exec__exec( g_cfg, "USER-MODIFIED", identifier );
}


static void exec__event__user_deleted(const char* identifier) {
	g_self.library.api.runtime.system.log( TT_LOG__INFO, "plugin/exec", "event 'USER-DELETED' received for identifier '%s'", identifier );
	exec__exec( g_cfg, "USER-DELETED", identifier );
}


static void exec__event__otp_created(const char* identifier) {
	g_self.library.api.runtime.system.log( TT_LOG__INFO, "plugin/exec", "event 'OTP-CREATED' received for identifier '%s'", identifier );
	exec__exec( g_cfg, "OTP-CREATED", identifier );
}


static void exec__event__otp_modified(const char* identifier) {
	g_self.library.api.runtime.system.log( TT_LOG__INFO, "plugin/exec", "event 'OTP-MODIFIED' received for identifier '%s'", identifier );
	exec__exec( g_cfg, "OTP-MODIFIED", identifier );
}


static void exec__event__otp_deleted(const char* identifier) {
	g_self.library.api.runtime.system.log( TT_LOG__INFO, "plugin/exec", "event 'OTP-DELETED' received for identifier '%s'", identifier );
	exec__exec( g_cfg, "OTP-DELETED", identifier );
}


static void exec__event__uhd_created(const char* identifier) {
	g_self.library.api.runtime.system.log( TT_LOG__INFO, "plugin/exec", "event 'UHD-CREATED' received for identifier '%s'", identifier );
	exec__exec( g_cfg, "UHD-CREATED", identifier );
}


static void exec__event__uhd_modified(const char* identifier) {
	g_self.library.api.runtime.system.log( TT_LOG__INFO, "plugin/exec", "event 'UHD-MODIFIED' received for identifier '%s'", identifier );
	exec__exec( g_cfg, "UHD-MODIFIED", identifier );
}


static void exec__event__uhd_deleted(const char* identifier) {
	g_self.library.api.runtime.system.log( TT_LOG__INFO, "plugin/exec", "event 'UHD-DELETED' received for identifier '%s'", identifier );
	exec__exec( g_cfg, "UHD-DELETED", identifier );
}


static void exec__event__auth_user(const char* identifier) {
	g_self.library.api.runtime.system.log( TT_LOG__INFO, "plugin/exec", "event 'AUTH-USER' received for identifier '%s'", identifier );
	exec__exec( g_cfg, "AUTH-USER", identifier );
}


static void exec__event__auth_otp(const char* identifier) {
	g_self.library.api.runtime.system.log( TT_LOG__INFO, "plugin/exec", "event 'AUTH-OTP' received for identifier '%s'", identifier );
	exec__exec( g_cfg, "AUTH-OTP", identifier );
}


static int initialize() {
	g_cfg = cfg_init( opt_exec, CFGF_NONE );
	if( g_cfg == NULL ) {
		return TT_ERR;
	}
	return TT_OK;
}


static int configure(const char* filename) {
	if( cfg_parse( g_cfg, filename ) != 0 ) {
		g_self.library.api.runtime.system.log( TT_LOG__ERROR, "plugin/exec", "cfg_parse() failed for '%s'", filename );
		return TT_ERR;
	}
	g_self.interface.events.user.created = exec__event__user_created;
	g_self.interface.events.user.modified = exec__event__user_modified;
	g_self.interface.events.user.deleted = exec__event__user_deleted;
	g_self.interface.events.otp.created = exec__event__otp_created;
	g_self.interface.events.otp.modified = exec__event__otp_modified;
	g_self.interface.events.otp.deleted = exec__event__otp_deleted;
	g_self.interface.events.uhd.created = exec__event__uhd_created;
	g_self.interface.events.uhd.modified = exec__event__uhd_modified;
	g_self.interface.events.uhd.deleted = exec__event__uhd_deleted;
	g_self.interface.events.auth.user = exec__event__auth_user;
	g_self.interface.events.auth.otp = exec__event__auth_otp;
	return TT_OK;
}


static int finalize() {
	if( g_cfg != NULL ) {
		cfg_free( g_cfg );
		g_cfg = NULL;
	}
	return TT_OK;
}

