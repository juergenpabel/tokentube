#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <tokentube.h>
#include <plymouth-1/ply-boot-client/ply-boot-client.h>
#include <plymouth-1/ply/ply-event-loop.h>
#include <plymouth-1/ply/ply-logger.h>
#include <plymouth-1/ply/ply-utils.h>


typedef struct {
	ply_event_loop_t	*loop;
	ply_boot_client_t	*client;

	char**			user;
	size_t*			user_len;
	char**			pass;
	size_t*			pass_len;
} state_t;


static void on_ping(void *user_data, ply_boot_client_t *client) {
	(void)user_data; /* unused */
	(void)client; /* unused */
}


static void on_failure(void *user_data, ply_boot_client_t *client) {
	state_t* state = (state_t*)user_data;

	(void)client; /* unused */
	ply_event_loop_exit( state->loop, TT_ERR );
}


static void on_username(void *user_data, const char *answer, ply_boot_client_t *client) {
	state_t* state = (state_t*)user_data;

	(void)client; /* unused */
	if( answer != NULL && answer[0] != ('\100'^'C') ) {
		*(state->user_len) = strlen( answer );
		*(state->user) = malloc( *(state->user_len)+1 );
		if( *(state->user) != NULL ) {
			strncpy( *(state->user), answer, *(state->user_len) );
		} else {
			exit(-1);
		}
	} else {
		ply_event_loop_exit( ((state_t*)state)->loop, TT_ERR );
	}
}


static void on_password(void *user_data, const char *answer, ply_boot_client_t *client) {
	state_t* state = (state_t*)user_data;
	int result = TT_ERR;

	(void)client; /* unused */
	if( answer != NULL && answer[0] != ('\100'^'C') ) {
		*(state->pass_len) = strlen( answer );
		*(state->pass) = malloc( *(state->pass_len)+1 );
		if( *(state->pass) != NULL ) {
			strncpy( *(state->pass), answer, *(state->pass_len) );
			result = TT_OK;
		} else {
			exit(-1);
		}
        }
	ply_event_loop_exit( ((state_t*)state)->loop, result );
}

int pba_plymouth(const char* prompt_user, const char* prompt_pass, char** user, size_t* user_len, char** pass, size_t* pass_len) {
	state_t state;
	int result = TT_ERR;

	state.user = user;
	state.user_len = user_len;
	state.pass = pass;
	state.pass_len = pass_len;

	state.loop = ply_event_loop_new();
	state.client = ply_boot_client_new();
	if( ply_boot_client_connect( state.client, on_failure, &state ) ) {
		ply_boot_client_attach_to_event_loop( state.client, state.loop );
		ply_boot_client_ping_daemon( state.client, on_ping, on_failure, &state );
		if( *state.user == NULL ) {
			ply_boot_client_ask_daemon_question( state.client, prompt_user, on_username, on_failure, &state );
		}
		ply_boot_client_ask_daemon_for_password( state.client, prompt_pass, on_password, on_failure, &state );
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

