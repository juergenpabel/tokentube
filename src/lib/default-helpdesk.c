#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <confuse.h>
#include <gcrypt.h>
#include "libtokentube.h"


__attribute__ ((visibility ("hidden")))
int default__api__helpdesk_create(const char* identifier, tt_status_t* status) {
	dflt_helpdesk_t  helpdesk = TT_UHD__UNDEFINED;
	dflt_otp_t       otp = TT_OTP__UNDEFINED;
	size_t           sys_id_size = 0;

	TT_TRACE( "plugin/default", "%s(identifier='%s')", __FUNCTION__, identifier );
	if( identifier == NULL || status == NULL ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameters in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	*status = TT_STATUS__UNDEFINED;
	if( default__impl__otp_storage_load( identifier, &otp ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "default__impl__otp_storage_load() failed for identifier '%s' in %s()", identifier, __FUNCTION__ );
		return TT_ERR;
	}
	sys_id_size = sizeof(helpdesk.sys_id);
	if( libtokentube_runtime_get_sysid( helpdesk.sys_id, &sys_id_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "API:get_sysid() failed for identifier '%s' in %s()", identifier, __FUNCTION__ );
		return TT_ERR;
	}
	helpdesk.key_data_size = sizeof(helpdesk.key_data);
	if( libtokentube_plugin__storage_load( TT_FILE__KEY, identifier, helpdesk.key_data, &helpdesk.key_data_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_plugin__storage_load() failed for identifier '%s' in %s()", identifier, __FUNCTION__ );
		return TT_ERR;
	}
	strncpy( helpdesk.key_id, identifier, sizeof(helpdesk.key_id)-1 );
	strncpy( helpdesk.otp_hash, otp.hash, sizeof(helpdesk.otp_hash)-1 );
	helpdesk.otp_bits = otp.bits;
	helpdesk.otp_iter = 1;
	if( default__impl__helpdesk_storage_save( identifier, &helpdesk ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "default__impl__helpdesk_storage_save() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	*status = TT_YES;
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int default__api__helpdesk_modify(const char* identifier, tt_modify_t action, void* data, tt_status_t* status) {
	TT_TRACE( "library/plugin", "%s(identifier='%s',action=%zd,data=%p,status=%p)", __FUNCTION__, identifier, action, data, status );
	if( identifier == NULL || identifier[0] == '\0' || action == TT_MODIFY__UNDEFINED || data == NULL || status == NULL ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	return TT_IGN;
}


__attribute__ ((visibility ("hidden")))
int default__api__helpdesk_delete(const char* identifier, tt_status_t* status) {

	TT_TRACE( "plugin/default", "%s(identifier='%s',status=%p)", __FUNCTION__, identifier, status );
	if( identifier == NULL || identifier[0] == '\0' || status == NULL ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameters in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	*status = TT_STATUS__UNDEFINED;
	if( default__impl__helpdesk_storage_delete( identifier, status ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "default__impl__helpdesk_storage_delete() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int default__api__helpdesk_exists(const char* identifier, tt_status_t* status) {
	TT_TRACE( "plugin/default", "%s(identifier='%s',status=%p)", __FUNCTION__, identifier, status );
	if( identifier == NULL || identifier[0] == '\0' || status == NULL ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameters in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( libtokentube_plugin__storage_exists( TT_FILE__UHD, identifier, status ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_helpdesk__storage_exists() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	return TT_OK;
}

