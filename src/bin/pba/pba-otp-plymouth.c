#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>

#include <tokentube.h>

#include <plymouth-1/ply-boot-client/ply-boot-client.h>
#include <plymouth-1/ply/ply-event-loop.h>
#include <plymouth-1/ply/ply-logger.h>
#include <plymouth-1/ply/ply-utils.h>

static ply_event_loop_t* loop = NULL;

int libtokentube_util_base32_decode(const char* const text, const size_t text_len, char* bits, const size_t bits_len, int* result);


static void on_ping(void *user_data, ply_boot_client_t *client) {
	(void)user_data; /* unused */
	(void)client; /* unused */
}


static void on_failure(void *user_data, ply_boot_client_t *client) {
	(void)user_data; /* unused */
	(void)client; /* unused */
	ply_event_loop_exit( loop, -1 );
}


static void on_response(void *user_data, const char *answer, ply_boot_client_t *client) {
	char* response = (char*)user_data;
	int result = -1;

	(void)client; /* unused */
        if( answer != NULL && answer[0] != ('\100'^'C') ) {
		if( response != NULL ) {
			strncpy( response, answer, TT_OTP_TEXT_MAX );
			result = 0;
		}
        }
	ply_event_loop_exit( loop, result );
}


int pba_otp_plymouth(const char* challenge, char* response) {
	ply_boot_client_t *client;
	int result = 0;

	loop = ply_event_loop_new();
	client = ply_boot_client_new();
	if( ply_boot_client_connect( client, on_failure, loop ) ) {
		ply_boot_client_attach_to_event_loop( client, loop );
		ply_boot_client_ping_daemon( client, on_ping, on_failure, loop );
		ply_boot_client_ask_daemon_question( client, challenge, on_response, on_failure, response );
		result = ply_event_loop_run( loop );
	}
	ply_boot_client_free( client );
	ply_event_loop_free( loop );
	if( result != 0 ) {
		return TT_ERR;
	}
	return TT_OK;
}

