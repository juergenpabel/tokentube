#include <stdlib.h>
#include <string.h>
#include "libtokentube.h"


static tt_library_api_t g_library_api;


__attribute__ ((visibility ("hidden")))
int libtokentube_library_initialize() {
	memset( &g_library_api, '\0', sizeof(g_library_api) );
	g_library_api.runtime.log = libtokentube_runtime_log;
	g_library_api.runtime.debug = libtokentube_runtime_debug;
	g_library_api.runtime.sysid = libtokentube_plugin__runtime_get_sysid;
	g_library_api.util.crc16 = libtokentube_util_crc16;
	g_library_api.util.hex_encode = libtokentube_util_hex_encode;
	g_library_api.util.hex_decode = libtokentube_util_hex_decode;
	g_library_api.util.base32_encode = libtokentube_util_base32_encode;
	g_library_api.util.base32_decode = libtokentube_util_base32_decode;
	g_library_api.util.base64_encode = libtokentube_util_base64_encode;
	g_library_api.util.base64_decode = libtokentube_util_base64_decode;
	g_library_api.util.posix_mkdir = libtokentube_util_posix_mkdir;
	g_library_api.util.posix_copy = libtokentube_util_posix_copy;
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_library_configure() {
	g_library_api.runtime.broadcast = libtokentube_plugin__event_broadcast;
	g_library_api.runtime.install_pba = libtokentube_runtime_install_pba;
	g_library_api.runtime.conf_read_int = libtokentube_conf_read_int;
	g_library_api.runtime.conf_read_str = libtokentube_conf_read_str;
	g_library_api.crypto.random = libtokentube_crypto_random;
	g_library_api.crypto.encrypt = libtokentube_crypto_encrypt;
	g_library_api.crypto.decrypt = libtokentube_crypto_decrypt;
	g_library_api.crypto.hash = libtokentube_crypto_hash;
	g_library_api.crypto.kdf = libtokentube_crypto_kdf;
	g_library_api.storage.file_load = libtokentube_plugin__file_load;
	g_library_api.storage.file_save = libtokentube_plugin__file_save;
	g_library_api.storage.file_exists = libtokentube_plugin__file_exists;
	g_library_api.storage.file_delete = libtokentube_plugin__file_delete;
	g_library_api.storage.luks_load = libtokentube_plugin__luks_load;
	g_library_api.user.create = libtokentube_plugin__user_create;
	g_library_api.user.update = libtokentube_plugin__user_update;
	g_library_api.user.delete = libtokentube_plugin__user_delete;
	g_library_api.user.exists = libtokentube_plugin__user_exists;
	g_library_api.user.execute_verify = libtokentube_plugin__user_execute_verify;
	g_library_api.user.execute_load = libtokentube_plugin__user_execute_load;
	g_library_api.user.execute_autoenrollment = libtokentube_plugin__user_execute_autoenrollment;
	g_library_api.otp.create = libtokentube_plugin__otp_create;
	g_library_api.otp.delete = libtokentube_plugin__otp_delete;
	g_library_api.otp.exists = libtokentube_plugin__otp_exists;
	g_library_api.otp.execute_challenge = libtokentube_plugin__otp_execute_challenge;
	g_library_api.otp.execute_response = libtokentube_plugin__otp_execute_response;
	g_library_api.otp.execute_apply = libtokentube_plugin__otp_execute_apply;
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_library_finalize() {
	memset( &g_library_api, '\0', sizeof(g_library_api) );
	return TT_OK;
}


int tt_discover(tt_library_t* library) {
	if( library == NULL ) {
		TT_LOG_ERROR( "library/core", "invalid parameter in %s()", __FUNCTION__ );
		return TT_ERR;
	}
	if( library->version.major == 0 && library->version.minor == 0 && library->version.patch == 0 ) {
		library->version = TT_VERSION;
	}
	library->version = TT_VERSION;
	memcpy( &library->api, &g_library_api, sizeof(g_library_api) );
	return TT_OK;
}

