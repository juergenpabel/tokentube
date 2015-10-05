#include <stdlib.h>
#include <string.h>
#include "libtokentube.h"


static tt_library_api_t g_library_api;


__attribute__ ((visibility ("hidden")))
int libtokentube_library_initialize() {
	memset( &g_library_api, '\0', sizeof(g_library_api) );
	g_library_api.runtime.system.log = libtokentube_runtime_log;
	g_library_api.runtime.system.debug = libtokentube_runtime_debug;
	g_library_api.runtime.system.sysid = libtokentube_plugin__runtime_get_sysid;
	g_library_api.runtime.util.crc16 = libtokentube_util_crc16;
	g_library_api.runtime.util.hex_encode = libtokentube_util_hex_encode;
	g_library_api.runtime.util.hex_decode = libtokentube_util_hex_decode;
	g_library_api.runtime.util.base32_encode = libtokentube_util_base32_encode;
	g_library_api.runtime.util.base32_decode = libtokentube_util_base32_decode;
	g_library_api.runtime.util.base64_encode = libtokentube_util_base64_encode;
	g_library_api.runtime.util.base64_decode = libtokentube_util_base64_decode;
	g_library_api.runtime.util.posix_mkdir = libtokentube_util_posix_mkdir;
	g_library_api.runtime.util.posix_copy = libtokentube_util_posix_copy;
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_library_configure() {
	g_library_api.runtime.system.broadcast = libtokentube_plugin__event_broadcast;
	g_library_api.runtime.system.install_pba = libtokentube_runtime_install_pba;
	g_library_api.runtime.system.conf_read_int = libtokentube_conf_read_int;
	g_library_api.runtime.system.conf_read_str = libtokentube_conf_read_str;
	g_library_api.runtime.crypto.random = libtokentube_crypto_random;
	g_library_api.runtime.crypto.encrypt = libtokentube_crypto_encrypt;
	g_library_api.runtime.crypto.decrypt = libtokentube_crypto_decrypt;
	g_library_api.runtime.crypto.hash = libtokentube_crypto_hash;
	g_library_api.runtime.crypto.hmac = libtokentube_crypto_hmac;
	g_library_api.runtime.crypto.kdf = libtokentube_crypto_kdf;
	g_library_api.storage.load = libtokentube_plugin__storage_load;
	g_library_api.storage.save = libtokentube_plugin__storage_save;
	g_library_api.storage.exists = libtokentube_plugin__storage_exists;
	g_library_api.storage.delete = libtokentube_plugin__storage_delete;
	g_library_api.database.user.create = libtokentube_plugin__user_create;
	g_library_api.database.user.update = libtokentube_plugin__user_update;
	g_library_api.database.user.delete = libtokentube_plugin__user_delete;
	g_library_api.database.user.exists = libtokentube_plugin__user_exists;
	g_library_api.database.user.key_add = libtokentube_plugin__user_key_add;
	g_library_api.database.user.key_del = libtokentube_plugin__user_key_del;
	g_library_api.database.otp.create = libtokentube_plugin__otp_create;
	g_library_api.database.otp.update = libtokentube_plugin__otp_update;
	g_library_api.database.otp.delete = libtokentube_plugin__otp_delete;
	g_library_api.database.otp.exists = libtokentube_plugin__otp_exists;
	g_library_api.auth.user.verify = libtokentube_plugin__auth_user_verify;
	g_library_api.auth.user.loadkey = libtokentube_plugin__auth_user_loadkey;
	g_library_api.auth.user.autoenrollment = libtokentube_plugin__auth_user_autoenrollment;
	g_library_api.auth.otp.challenge = libtokentube_plugin__auth_otp_challenge;
	g_library_api.auth.otp.response = libtokentube_plugin__auth_otp_response;
	g_library_api.auth.otp.loadkey = libtokentube_plugin__auth_otp_loadkey;
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

