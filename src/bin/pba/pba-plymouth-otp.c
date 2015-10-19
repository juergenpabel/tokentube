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


typedef struct {
	ply_event_loop_t*   loop;
	ply_boot_client_t*  client;

	const char*         challenge;
	char*               response;
	size_t              response_size;
} state_t;


static void on_done(void *user_data, ply_boot_client_t *client) {
	state_t* state = user_data;

	(void)client; /* unused */
	ply_event_loop_exit( state->loop, TT_OK );
}


static void on_noop(void *user_data, ply_boot_client_t *client) {
	state_t* state = user_data;

	(void)state; /* unused */
	(void)client; /* unused */
}


static void on_failure(void *user_data, ply_boot_client_t *client) {
	state_t* state = user_data;

	(void)client; /* unused */
	ply_event_loop_exit( state->loop, TT_ERR );
}


static void on_response(void *user_data, const char *answer, ply_boot_client_t *client) {
	state_t* state = user_data;
	size_t   i, answer_size = 0;
	int      result = TT_OK;

	if( answer == NULL || answer[0] == ('\100'^'C') ) {
		return;
	}
	answer_size = strnlen( answer, TT_OTP_TEXT_MAX );
	if( answer_size != strnlen( state->challenge, TT_OTP_TEXT_MAX ) ) {
		result = TT_ERR;
	}
	if( (answer_size+1) % 5 != 0 ) {
		result = TT_ERR;
	}
	for( i=0; i<answer_size/5; i++) {
		if( answer[i*5+4] != '-' ) {
			result = TT_ERR;
		}
	}
	if( result == TT_ERR ) {
		ply_boot_client_ask_daemon_question( client, state->challenge, on_response, on_failure, state );
		return;
	}
	strncpy( state->response, answer, state->response_size-1 );
	state->response_size = strnlen( state->response, state->response_size );
	ply_event_loop_exit( state->loop, TT_OK );
}


int pba_plymouth_otp(const char* message, const char* challenge, char* response, size_t response_size) {
	char     buffer[TT_OTP_TEXT_MAX/4*5] = { 0 };
	size_t   i, challenge_size;
	state_t  state;
	int      result = TT_UNDEFINED;

	challenge_size = strnlen( challenge, TT_OTP_TEXT_MAX );
	for( i=0; i<challenge_size/4; i++) {
		buffer[i*5+0] = challenge[i*4+0];
		buffer[i*5+1] = challenge[i*4+1];
		buffer[i*5+2] = challenge[i*4+2];
		buffer[i*5+3] = challenge[i*4+3];
		buffer[i*5+4] = '-';
	}
	buffer[challenge_size/4*5-1] = '\0';

	state.challenge = buffer;
	state.response = buffer;
	state.response_size = sizeof(buffer);
	state.loop = ply_event_loop_new();
	state.client = ply_boot_client_new();
	if( !ply_boot_client_connect( state.client, on_failure, state.loop ) ) {
		ply_boot_client_free( state.client );
		ply_event_loop_free( state.loop );
		return TT_ERR;
	}
	ply_boot_client_attach_to_event_loop( state.client, state.loop );
	if( message != NULL && message[0] != '\0' ) {
		ply_boot_client_tell_daemon_to_display_message( state.client, message, on_noop, on_noop, &state );
	}
	ply_boot_client_ask_daemon_question( state.client, buffer, on_response, on_failure, &state );
	result = ply_event_loop_run( state.loop );
	if( message != NULL && message[0] != '\0' ) {
		ply_boot_client_attach_to_event_loop( state.client, state.loop );
		ply_boot_client_tell_daemon_to_display_message( state.client, "", on_done, on_done, &state );
		ply_event_loop_run( state.loop );
	}
	if( (state.response_size+1)/5*4 >= response_size ) {
	}
	for( i=0; i<(state.response_size+1)/5; i++) {
		response[i*4+0] = state.response[i*5+0];
		response[i*4+1] = state.response[i*5+1];
		response[i*4+2] = state.response[i*5+2];
		response[i*4+3] = state.response[i*5+3];
	}
	response[(state.response_size+1)/5*4-1] = '\0';
	ply_boot_client_free( state.client );
	ply_event_loop_free( state.loop );
	return result;
}

