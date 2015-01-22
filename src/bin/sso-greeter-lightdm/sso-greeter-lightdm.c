#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <confuse.h>
#include <syslog.h>
#include <lightdm.h>
#include <tokentube/defines.h>


static cfg_opt_t opt_sso_greeter[] = {
	CFG_STR("original-greeter", "unity-greeter", CFGF_NONE),
	CFG_END()
};

static cfg_opt_t opt_sso_ssod[] = {
	CFG_STR("executable", "", CFGF_NONE),
	CFG_STR("socket", "", CFGF_NONE),
	CFG_END()
};

static cfg_opt_t opt_sso[] = {
	CFG_SEC("ssod", opt_sso_ssod, CFGF_NONE),
	CFG_END()
};


static GMainLoop *g_mainloop = NULL;
static LightDMGreeter *g_greeter = NULL;

static char g_username[TT_USERNAME_CHAR_MAX+1] = {0};
static char g_password[TT_PASSWORD_CHAR_MAX+1] = {0};

static gboolean g_result = FALSE;


static void greeter_log(int level, const char* message) {
	fprintf( stderr, "tokentube[greeter/lightdm]: %s (username='%s')\n", message, g_username );
	syslog( level, "greeter/lightdm: %s (username='%s')", message, g_username );
}


static void exec_original_greeter(int argc, char* argv[]) {
	char*	original_greeter = NULL;
	cfg_t*	cfg = NULL;

	(void)argc;
	memset( g_password, '\0', sizeof(g_password) );
	greeter_log( LOG_INFO, "executing original greeter..." );
        cfg = cfg_init( opt_sso_greeter, CFGF_NONE );
        if( cfg_parse( cfg, "/etc/tokentube/sso/lightdm-greeter.conf" ) != 0 ) {
		greeter_log( LOG_ERR, "failed to start original greeter, terminating." );
		exit(-1);
	}
	original_greeter = cfg_getstr( cfg, "original-greeter" );
	if( original_greeter != NULL ) {
		execvp( original_greeter, argv );
	}
	execv( "/usr/sbin/unity-greeter", argv );
	exit( -1 );
}



static void show_prompt_cb(LightDMGreeter *greeter, const gchar *text, LightDMPromptType type) {
	(void)text;
	(void)type;
	lightdm_greeter_respond( greeter, g_password );
}


static void auth_complete_cb(LightDMGreeter *greeter) {
	if( lightdm_greeter_get_is_authenticated(greeter) ) {
		greeter_log(LOG_INFO, "login successful");
		if( lightdm_greeter_start_session_sync( greeter, NULL, NULL ) ) {
			g_result = TRUE;
		} else {
			greeter_log(LOG_INFO, "start session failed, terminating...");
		}
	} else {
		greeter_log(LOG_INFO, "login failed, terminating...");
	}
	g_main_loop_quit( g_mainloop );
}


int main(int argc, char* argv[]) {
	struct sockaddr_un	sa = {0};
	cfg_t*			cfg;
	size_t			i;
	int			sock;

	cfg = cfg_init( opt_sso, CFGF_NONE );
	if( cfg == NULL ) {
		greeter_log( LOG_ERR, "cfg_init() failed" );
		exit( -1 );
	}
	if( cfg_parse( cfg, "/boot/tokentube/pba/sso.conf" ) != 0 ) {
		greeter_log( LOG_ERR, "cfg_parse() failed for '/boot/tokentube/pba/sso.conf'" );
		exit( -1 );
	}

	sa.sun_family = AF_UNIX;
	strncpy( sa.sun_path, cfg_getstr( cfg, "ssod|socket" ), sizeof(sa.sun_path)-1 );
	sock = socket( AF_UNIX, SOCK_STREAM, 0 );
	if( sock < 0 ) {
		greeter_log( LOG_INFO, "socket() failed for" );
		greeter_log( LOG_INFO, sa.sun_path );
		cfg_free( cfg );
		exec_original_greeter( argc, argv );
	}
	if( connect(sock, (struct sockaddr*)&sa, sizeof(sa.sun_family)+strlen(sa.sun_path)) < 0 ) {
		greeter_log( LOG_INFO, "no ssod socket, executing original greeter" );
		cfg_free( cfg );
		close( sock );
		exec_original_greeter( argc, argv );
	}
	cfg_free( cfg );
	cfg = NULL;

	for( i=0; i<sizeof(g_username)-1; i++ ) {
		if( read(sock, &g_username[i], 1) != 1 ) {
			greeter_log( LOG_ALERT, "unexpected end-of-stream for username from ssod" );
			exit( -1 );
		}
		if( g_username[i] == '\n' ) {
			g_username[i] = '\0';
			greeter_log( LOG_INFO, "received username from ssod" );
			break;
		}
	}
	for( i=0; i<sizeof(g_password)-1; i++ ) {
		if( read(sock, &g_password[i], 1) != 1 ) {
			greeter_log( LOG_ALERT, "unexpected end-of-stream for password from ssod" );
			exit( -1 );
		}
		if( g_password[i] == '\n' ) {
			g_password[i] = '\0';
			greeter_log( LOG_INFO, "received password from ssod" );
			break;
		}
	}
	close( sock );

	greeter_log( LOG_DEBUG, "starting lightdm communication" );
	gtk_init( &argc, &argv );
	g_greeter = lightdm_greeter_new();
	if( g_greeter == NULL) {
		greeter_log( LOG_INFO, "internal error: lightdm_greeter_new() failed" );
		exec_original_greeter( argc, argv );
	}
	g_signal_connect( g_greeter, "show-prompt", G_CALLBACK(show_prompt_cb), NULL );
	g_signal_connect( g_greeter, "authentication-complete", G_CALLBACK(auth_complete_cb), NULL );
	if( lightdm_greeter_connect_sync( g_greeter, NULL ) ) {
		lightdm_greeter_authenticate( g_greeter, g_username );
		g_mainloop = g_main_loop_new( NULL, FALSE );
		g_main_loop_run( g_mainloop );
	}
	memset( g_password, '\0', sizeof(g_password) );
	if( g_result != TT_OK ) {
		greeter_log( LOG_INFO, "login failed" );
		exec_original_greeter( argc, argv );
	}
	greeter_log( LOG_INFO, "completed login procedure" );
	return 0;
}

