#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <tokentube.h>
#include <plymouth-1/ply-boot-client/ply-boot-client.h>
#include <plymouth-1/ply/ply-event-loop.h>
#include <plymouth-1/ply/ply-logger.h>
#include <plymouth-1/ply/ply-utils.h>

#include "pba.h"

typedef struct {
	ply_event_loop_t	*loop;
	ply_boot_client_t	*client;
	tt_status_t	*	status;

	char*			user;
	size_t*			user_size;
	char*			pass;
	size_t*			pass_size;
	const char*		prompt_user;
	const char*		prompt_pass;
} state_t;


static void on_noop(void *user_data, ply_boot_client_t *client) {
	state_t* state = (state_t*)user_data;

	(void)client; /* unused */
	(void)state; /* unused */
}


static void on_done(void *user_data, ply_boot_client_t *client) {
	state_t* state = (state_t*)user_data;

	(void)client; /* unused */
	ply_event_loop_exit( state->loop, TT_OK );
}


static void on_fail(void *user_data, ply_boot_client_t *client) {
	state_t* state = (state_t*)user_data;

	(void)client; /* unused */
	ply_event_loop_exit( state->loop, TT_ERR );
}


static void on_username(void *user_data, const char *answer, ply_boot_client_t *client) {
	state_t* state = (state_t*)user_data;

	(void)client; /*unused*/
	if( answer == NULL || answer[0] == '\0' ) {
		*(state->status) = TT_NO;
		ply_event_loop_exit( state->loop, TT_OK );
		return;
	}
	strncpy( state->user, answer, *(state->user_size) );
	*(state->user_size) = strnlen( state->user, *(state->user_size) );
	ply_event_loop_exit( state->loop, TT_OK );
}


static void on_password(void *user_data, const char *answer, ply_boot_client_t *client) {
	state_t* state = (state_t*)user_data;

	(void)client; /*unused*/
	if( answer == NULL || answer[0] == '\0' ) {
		*(state->status) = TT_NO;
		ply_event_loop_exit( state->loop, TT_OK );
		return;
        }
	*(state->status) = TT_YES;
	strncpy( state->pass, answer, *(state->pass_size) );
	*(state->pass_size) = strnlen( state->pass, *(state->pass_size) );
	ply_event_loop_exit( state->loop, TT_OK );
}


int pba_plymouth_user(const char* message_user, const char* message_pass, const char* prompt_user, const char* prompt_pass, char* user, size_t* user_size, char* pass, size_t* pass_size, tt_status_t* status) {
	state_t  state;
	int      result = TT_ERR;

	state.user = user;
	state.user_size = user_size;
	state.pass = pass;
	state.pass_size = pass_size;
	state.prompt_user = prompt_user;
	state.prompt_pass = prompt_pass;
	state.status = status;

	state.loop = ply_event_loop_new();
	state.client = ply_boot_client_new();
	if( ply_boot_client_connect( state.client, on_fail, &state ) ) {
		result = TT_OK;
		if( state.user[0] == '\0' ) {
			ply_boot_client_attach_to_event_loop( state.client, state.loop );
			ply_boot_client_tell_daemon_to_display_message( state.client, message_user, on_noop, on_noop, NULL );
			ply_boot_client_ask_daemon_question( state.client, prompt_user, on_username, on_fail, &state );
			result = ply_event_loop_run( state.loop );
		}
		if( result == TT_OK && state.user[0] != '\0' ) {
			ply_boot_client_attach_to_event_loop( state.client, state.loop );
			ply_boot_client_tell_daemon_to_display_message( state.client, message_pass, on_noop, on_noop, NULL );
			ply_boot_client_ask_daemon_for_password( state.client, prompt_pass, on_password, on_fail, &state );
			result = ply_event_loop_run( state.loop );
		}
		ply_boot_client_attach_to_event_loop( state.client, state.loop );
		ply_boot_client_tell_daemon_to_display_message( state.client, "", on_done, on_done, &state );
		result = ply_event_loop_run( state.loop );
	}
	if( state.client != NULL ) {
		ply_boot_client_free( state.client );
	}
	if( state.loop != NULL ) {
		ply_event_loop_free( state.loop );
	}
	return result;
}

