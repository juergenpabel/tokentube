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

	char*			user;
	size_t*			user_size;
	char*			pass;
	size_t*			pass_size;
	int			pba_mode;
} state_t;


static void on_noop(void *user_data, ply_boot_client_t *client) {
	state_t* state = (state_t*)user_data;

	(void)client; /* unused */
	(void)state; /* unused */
}

static void on_fail(void *user_data, ply_boot_client_t *client) {
	state_t* state = (state_t*)user_data;

	(void)client; /* unused */
	ply_event_loop_exit( state->loop, TT_ERR );
}


static void on_key(void *user_data, const char* input, ply_boot_client_t *client) {
	state_t*  state = (state_t*)user_data;
	int       result = 0;

	if( input == NULL ) {
		ply_event_loop_exit( state->loop, TT_ERR );
	}
	ply_boot_client_ask_daemon_to_watch_for_keystroke( client, NULL, on_key, on_fail, state );
	if( state->pba_mode/PBA_PLAIN + state->pba_mode/PBA_USER + state->pba_mode/PBA_OTP > 1 ) {
		if( input[0] == ' ' || input[0] == '\0' ) {
			if( (state->pba_mode & PBA_USER) == 0 && (state->pba_mode & PBA_PLAIN) == PBA_PLAIN ) {
				result = PBA_PLAIN;
			}
			if( (state->pba_mode & PBA_USER) == PBA_USER ) {
				result = PBA_USER;
			}
		}
		if( input[0] == '#' ) {
			if( (state->pba_mode & PBA_OTP) == PBA_OTP ) {
				result = PBA_OTP;
			}
		}
	}
	if( state->pba_mode == PBA_PLAIN ) {
		result = PBA_PLAIN;
	}
	if( result != 0 ) {
		ply_event_loop_exit( state->loop, result );
	}
}


int pba_plymouth_splash(const char* text, int pba_mode) {
	state_t  state;

	state.pba_mode = pba_mode;
	state.loop = ply_event_loop_new();
	state.client = ply_boot_client_new();
	if( ply_boot_client_connect( state.client, on_fail, &state ) ) {
		ply_boot_client_attach_to_event_loop( state.client, state.loop );
		ply_boot_client_tell_daemon_to_display_message( state.client, text, on_noop, on_noop, NULL );
		ply_boot_client_ask_daemon_to_watch_for_keystroke( state.client, NULL, on_key, on_fail, &state );
		state.pba_mode = ply_event_loop_run( state.loop );
	}
	if( state.client != NULL ) {
		ply_boot_client_free( state.client );
	}
	if( state.loop != NULL ) {
		ply_event_loop_free( state.loop );
	}
	return state.pba_mode;
}

