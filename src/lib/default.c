#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/kdev_t.h>
#include <pwd.h>
#include <grp.h>
#include "libtokentube.h"


__attribute__ ((visibility ("hidden")))
int default__initialize(tt_plugin_t* plugin) {
	if( plugin == NULL ) {
		TT_LOG_ERROR( "library/default", "internal error in %s at %d", __FUNCTION__, __LINE__ );
		return TT_ERR;
	}
	plugin->interface.api.pba.install_pre	= default__api__pba_install_pre;
	plugin->interface.api.pba.install_run = default__api__pba_install_run;
	plugin->interface.api.pba.install_post 	= default__api__pba_install_post;
	plugin->interface.api.runtime.get_sysid = default__api__runtime_get_sysid;
	plugin->interface.api.runtime.get_type = default__api__runtime_get_type;
	plugin->interface.api.runtime.get_bootdevice = default__api__runtime_get_bootdevice;
	plugin->interface.api.storage.load = default__api__storage_load;
	plugin->interface.api.storage.save = default__api__storage_save;
	plugin->interface.api.storage.exists = default__api__storage_exists;
	plugin->interface.api.storage.delete = default__api__storage_delete;
	plugin->interface.api.database.user.create = default__api__user_create;
	plugin->interface.api.database.user.update = default__api__user_update;
	plugin->interface.api.database.user.delete = default__api__user_delete;
	plugin->interface.api.database.user.exists = default__api__user_exists;
	plugin->interface.api.database.user.key_add = default__api__user_key_add;
	plugin->interface.api.database.user.key_del = default__api__user_key_del;
	plugin->interface.api.database.otp.create = default__api__otp_create;
	plugin->interface.api.database.otp.delete = default__api__otp_delete;
	plugin->interface.api.database.otp.exists = default__api__otp_exists;
	plugin->interface.api.auth.user.autoenrollment = default__api__auth_user_autoenrollment;
	plugin->interface.api.auth.user.verify = default__api__auth_user_verify;
	plugin->interface.api.auth.user.loadkey = default__api__auth_user_loadkey;
	plugin->interface.api.auth.otp.challenge = default__api__auth_otp_challenge;
	plugin->interface.api.auth.otp.response = default__api__auth_otp_response;
	plugin->interface.api.auth.otp.loadkey = default__api__auth_otp_loadkey;
	plugin->interface.events.otp.created = default__event__otp_created;
	plugin->interface.events.otp.deleted = default__event__otp_deleted;
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int default__finalize() {
	return TT_OK;
}

