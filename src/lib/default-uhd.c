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
int default__api__uhd_create(const char* identifier, tt_status_t* status) {
	dflt_uhd_t  uhd = TT_UHD__UNDEFINED;
	dflt_otp_t       otp = TT_OTP__UNDEFINED;
	size_t           sys_id_size = 0;

	TT_TRACE( "plugin/default", "%s(identifier='%s')", __FUNCTION__, identifier );
	if( identifier == NULL || status == NULL ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameters in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	*status = TT_STATUS__UNDEFINED;
	if( default__impl__uhd_storage_exists( identifier, status ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "default__impl__uhd_storage_exists() failed for identifier '%s' in %s()", identifier, __FUNCTION__ );
		return TT_ERR;
	}
	if( *status == TT_STATUS__YES ) {
		*status = TT_STATUS__NO;
		return TT_OK;
	}
	if( default__impl__otp_storage_load( identifier, &otp ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "default__impl__otp_storage_load() failed for identifier '%s' in %s()", identifier, __FUNCTION__ );
		return TT_ERR;
	}
	sys_id_size = sizeof(uhd.sys_id);
	if( libtokentube_runtime_get_sysid( uhd.sys_id, &sys_id_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "API:get_sysid() failed for identifier '%s' in %s()", identifier, __FUNCTION__ );
		return TT_ERR;
	}
	uhd.key_data_size = sizeof(uhd.key_data);
	if( libtokentube_plugin__storage_load( TT_FILE__KEY, identifier, uhd.key_data, &uhd.key_data_size ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_plugin__storage_load() failed for identifier '%s' in %s()", identifier, __FUNCTION__ );
		return TT_ERR;
	}
	strncpy( uhd.key_id, identifier, sizeof(uhd.key_id)-1 );
	strncpy( uhd.otp_hash, otp.hash, sizeof(uhd.otp_hash)-1 );
	uhd.otp_bits = otp.bits;
	uhd.otp_iter = 1;
	if( default__impl__uhd_storage_save( identifier, &uhd ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "default__impl__uhd_storage_save() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	*status = TT_YES;
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int default__api__uhd_modify(const char* identifier, tt_modify_t action, void* data, tt_status_t* status) {
	TT_TRACE( "library/plugin", "%s(identifier='%s',action=%zd,data=%p,status=%p)", __FUNCTION__, identifier, action, data, status );
	if( identifier == NULL || identifier[0] == '\0' || action == TT_MODIFY__UNDEFINED || data == NULL || status == NULL ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	return TT_IGN;
}


__attribute__ ((visibility ("hidden")))
int default__api__uhd_delete(const char* identifier, tt_status_t* status) {

	TT_TRACE( "plugin/default", "%s(identifier='%s',status=%p)", __FUNCTION__, identifier, status );
	if( identifier == NULL || identifier[0] == '\0' || status == NULL ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameters in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	*status = TT_STATUS__UNDEFINED;
	if( default__impl__uhd_storage_exists( identifier, status ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "default__impl__uhd_storage_exists() failed for identifier '%s' in %s()", identifier, __FUNCTION__ );
		return TT_ERR;
	}
	if( *status == TT_STATUS__YES ) {
		if( default__impl__uhd_storage_delete( identifier, status ) != TT_OK ) {
			TT_LOG_ERROR( "plugin/default", "default__impl__uhd_storage_delete() failed in %s()", __FUNCTION__ );
			return TT_ERR;
		}
	}
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int default__api__uhd_exists(const char* identifier, tt_status_t* status) {
	TT_TRACE( "plugin/default", "%s(identifier='%s',status=%p)", __FUNCTION__, identifier, status );
	if( identifier == NULL || identifier[0] == '\0' || status == NULL ) {
		TT_LOG_ERROR( "plugin/default", "invalid parameters in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( libtokentube_plugin__storage_exists( TT_FILE__UHD, identifier, status ) != TT_OK ) {
		TT_LOG_ERROR( "plugin/default", "libtokentube_uhd__storage_exists() failed in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	return TT_OK;
}

