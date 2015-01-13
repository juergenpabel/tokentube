#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <plymouth-1/ply-boot-client/ply-boot-client.h>
#include <plymouth-1/ply/ply-event-loop.h>
#include <plymouth-1/ply/ply-logger.h>
#include <plymouth-1/ply/ply-utils.h>
#include <tokentube.h>


static ply_event_loop_t* g_loop = NULL;


static void on_ping(void *user_data, ply_boot_client_t *client) {
	(void)user_data; /* unused */
	(void)client; /* unused */
}


static void on_failure(void *user_data, ply_boot_client_t *client) {
	(void)user_data; /* unused */
	(void)client; /* unused */
	ply_event_loop_exit( g_loop, -1 );
}


static void on_response(void *user_data, const char *answer, ply_boot_client_t *client) {
	char*	response = (char*)user_data;
	int	result = TT_ERR;

	(void)client; /* unused */
	if( answer != NULL && answer[0] != ('\100'^'C') ) {
		if( response != NULL ) {
			strncpy( response, answer, TT_OTP_TEXT_MAX );
			result = TT_OK;
		}
	}
	ply_event_loop_exit( g_loop, result );
}


int pba_otp_plymouth(const char* challenge, char* response) {
	ply_boot_client_t*	client;

	g_loop = ply_event_loop_new();
	client = ply_boot_client_new();
	if( !ply_boot_client_connect( client, on_failure, g_loop ) ) {
		ply_boot_client_free( client );
		ply_event_loop_free( g_loop );
		return TT_ERR;
	}
	ply_boot_client_attach_to_event_loop( client, g_loop );
	ply_boot_client_ping_daemon( client, on_ping, on_failure, g_loop );
	ply_boot_client_ask_daemon_question( client, challenge, on_response, on_failure, response );
	if( ply_event_loop_run( g_loop ) != TT_OK ) {
		ply_boot_client_free( client );
		ply_event_loop_free( g_loop );
		return TT_ERR;
	}
	ply_boot_client_free( client );
	ply_event_loop_free( g_loop );
	return TT_OK;
}

