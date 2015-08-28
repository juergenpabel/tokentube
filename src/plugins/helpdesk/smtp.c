#define  _FILE_OFFSET_BITS 64
#define  LARGEFILE_SOURCE 1
#define   _LARGEFILE64_SOURCE 1
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <locale.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <resolv.h>
#include <netdb.h>
#include <resolv.h>
#include <string.h>
#include <syslog.h>
#include <gpgme.h>
#include <confuse.h>
#include <libesmtp.h>
#include <tokentube.h>
#include <tokentube/plugin.h>
#include "plugin.h"


extern tt_plugin_t 	g_self;


static int helpdesk__impl__mx(const char *name, char *mx, size_t mx_size) {
	unsigned char response[NS_PACKETSZ];
	char dispbuf[4096];
	ns_msg handle;
	ns_rr rr;
	int i, len;

	len = res_search( name, C_IN, T_MX, response, sizeof(response) );
	if( len < 0 ) {
		return TT_ERR;
	}

	if( ns_initparse( response, len, &handle ) < 0 ) {
		return TT_ERR;
	}

	len = ns_msg_count( handle, ns_s_an );
	if( len < 0 ) {
		return TT_ERR;
	}

	for( i=0; i < len; i++ ) {
		if( ns_parserr( &handle, ns_s_an, i, &rr ) ) {
			continue;
		}
		ns_sprintrr( &handle, &rr, NULL, NULL, dispbuf, sizeof(dispbuf)-1 );
		if( ns_rr_class(rr) == ns_c_in && ns_rr_type(rr) == ns_t_mx ) {
			dn_expand( ns_msg_base(handle), ns_msg_base(handle)+ns_msg_size(handle), ns_rr_rdata(rr) + NS_INT16SZ, mx, mx_size );
		}
	}
	return TT_OK;
}


static int helpdesk__impl__send(const char* data) {
	char			mx[256] = { 0 };
	char*			server;
	smtp_session_t		session;
	smtp_message_t		message;
	const smtp_status_t*	status = NULL;
	int			result = TT_ERR;

	server = g_conf_smtp_server;
	if( server == NULL ) {
		server = strchr( g_conf_smtp_to, '@' ) + 1;
		g_self.library.api.runtime.debug( TT_DEBUG__VERBOSITY4, "plugins/helpdesk", "resolving MX for '%s'...", server );
		if( helpdesk__impl__mx( server, mx, sizeof(mx)-3-1 ) != TT_OK ) {
			g_self.library.api.runtime.log( TT_LOG__ERROR, "plugins/helpdesk", "failed to resolve MX for '%s'", server );
			return TT_ERR;
		}
		strncat( mx, ":25", 3 );
		server = mx;
	}
	g_self.library.api.runtime.debug( TT_DEBUG__VERBOSITY5, "plugins/helpdesk", "using '%s' as MTA", server );
	g_self.library.api.runtime.debug( TT_DEBUG__VERBOSITY5, "plugins/helpdesk", "using '%s' as TO", g_conf_smtp_to );
	g_self.library.api.runtime.debug( TT_DEBUG__VERBOSITY5, "plugins/helpdesk", "using '%s' as FROM", g_conf_smtp_from );
	session = smtp_create_session();
	message = smtp_add_message( session );
	smtp_set_server( session, server );
	smtp_add_recipient( message, g_conf_smtp_to );
	smtp_set_reverse_path( message, g_conf_smtp_from );
	smtp_set_header( message, "To", g_conf_smtp_to, g_conf_smtp_to );
	smtp_set_header( message, "From", g_conf_smtp_from, g_conf_smtp_from );
	smtp_set_header( message, "Subject", g_conf_smtp_subject );
	smtp_set_message_str( message, (char*)data );

	if( smtp_start_session( session ) ) {
		status = smtp_message_transfer_status( message );
		switch( status->code ) {
			case 250:
				result = TT_OK;
				g_self.library.api.runtime.debug( TT_DEBUG__VERBOSITY2,  "plugin/helpdesk", "E-Mail send in helpdesk__impl__send()" );
				break;
			default:
				g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/helpdesk", "MTA return error:%d %s", status->code, status->text ); 
		}
	} else {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/helpdesk", "SMTP session failed" );
	}
	smtp_destroy_session( session );
	return result;
}


__attribute__ ((visibility ("hidden")))
int helpdesk__api__file_save(tt_file_t file, const char* identifier, const char* data, size_t data_size) {
	char*			ciphertext = NULL;
	size_t			ciphertext_size = 0;
	char			key[TT_KEY_BITS_MAX/8] = { 0 };
	size_t			key_size = sizeof(key);
	char			sysid[64] = { 0 };
	size_t			sysid_size = sizeof(sysid);
	off_t			offset = 0;
	gpgme_ctx_t		gpg_ctx;
	gpgme_data_t		gpg_in, gpg_out;
	gpgme_key_t		gpg_key[2] = { NULL, NULL };
	gpgme_encrypt_result_t	gpg_result;

	if( file != TT_FILE__HELPDESK ) {
		return TT_IGN;
	}
	if( identifier == NULL ) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/helpdesk", "invoked helpdesk__api__file_save() without identifier" );
		return TT_ERR;
	}
	g_self.library.api.runtime.debug( TT_DEBUG__VERBOSITY2, "plugin/helpdesk", "invoked helpdesk__api__file_save('%s')", identifier );
	if( g_self.library.api.runtime.sysid( sysid, &sysid_size ) != TT_OK ) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/helpdesk", "error reading host-id" );
		return TT_ERR;
	}
	if( g_self.library.api.storage.load( TT_FILE__KEY, identifier, key, &key_size ) != TT_OK ) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/helpdesk", "error loading luks-key" );
		return TT_ERR;
	}

	gpgme_check_version( NULL );      
	if( gpgme_new( &gpg_ctx ) != 0 ) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/helpdesk", "gpgme_new() failed in helpdesk__api__file_save()" );
		return TT_ERR;
	}
	gpgme_set_armor( gpg_ctx, 1 );
	if( gpgme_data_new_from_mem( &gpg_in, data, data_size, 0 ) != 0 ) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/helpdesk", "gpgme_data_new_from_mem() failed in helpdesk__api__file_save()" );
	}
	if( gpgme_data_new( &gpg_out ) != 0 ) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/helpdesk", "gpgme_data_new() failed in helpdesk__api__file_save()" );
	}
	if( gpgme_get_key( gpg_ctx, g_conf_smtp_to, &gpg_key[0], 0 ) != 0 ) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/helpdesk", "gpgme_get_key() failed in helpdesk__api__file_save()" );
		return TT_ERR;
	}
	if( gpgme_op_encrypt( gpg_ctx, gpg_key, GPGME_ENCRYPT_ALWAYS_TRUST, gpg_in, gpg_out ) != 0 ) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/helpdesk", "gpgme_op_encrypt() failed in helpdesk__api__file_save()" );
		return TT_ERR;
	}
	gpg_result = gpgme_op_encrypt_result( gpg_ctx );
	if( gpg_result == NULL ) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/helpdesk", "gpgme_op_encrypt_result() failed in helpdesk__api__file_save()" );
		return TT_ERR;
	}
	if( gpg_result->invalid_recipients ) {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/helpdesk", "gpgme_op_encrypt_result() has invalid recipients in helpdesk__api__file_save()" );
		return TT_ERR;
	}
	ciphertext_size = gpgme_data_seek( gpg_out, 0, SEEK_END );
	ciphertext = malloc( 2+ciphertext_size+1 );
	if( ciphertext != NULL ) {
		memset( ciphertext, '\0', 2+ciphertext_size+1 );
		gpgme_data_seek( gpg_out, 0, SEEK_SET );
		offset = gpgme_data_read( gpg_out, ciphertext+2, ciphertext_size );
		if( (long)offset == (long)ciphertext_size ) {
			ciphertext[0] = '\r';
			ciphertext[1] = '\n';
			if( helpdesk__impl__send( ciphertext ) != TT_OK ) {
				g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/helpdesk", "helpdesk__impl__send failed in helpdesk__api__file_save()" );
			}
		} else {
			g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/helpdesk", "gpgme_data_read() failed at position %lu in helpdesk__api__file_save()", (unsigned long)offset );
		}
	} else {
		g_self.library.api.runtime.log( TT_LOG__ERROR, "plugin/helpdesk", "malloc() failed in helpdesk__api__file_save()" );
	}
  
	gpgme_key_unref( gpg_key[0] );
	gpgme_data_release( gpg_in );
	gpgme_data_release( gpg_out );
	gpgme_release( gpg_ctx );
	return TT_OK;
}

