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
	plugin->interface.api.pba.install = default__api__pba_install;
	plugin->interface.api.pba.install_post 	= default__api__pba_install_post;
	plugin->interface.api.runtime.get_sysid = default__api__runtime_get_sysid;
	plugin->interface.api.runtime.get_type = default__api__runtime_get_type;
	plugin->interface.api.runtime.get_bootdevice = default__api__runtime_get_bootdevice;
	plugin->interface.api.storage.file_load = default__api__storage_file_load;
	plugin->interface.api.storage.file_save = default__api__storage_file_save;
	plugin->interface.api.storage.file_exists = default__api__storage_file_exists;
	plugin->interface.api.storage.file_delete = default__api__storage_file_delete;
	plugin->interface.api.storage.luks_load = default__api__luks_load;
	plugin->interface.api.user.create = default__api__user_create;
	plugin->interface.api.user.update = default__api__user_update;
	plugin->interface.api.user.delete = default__api__user_delete;
	plugin->interface.api.user.exists = default__api__user_exists;
	plugin->interface.api.user.execute_load = default__api__user_execute_load;
	plugin->interface.api.user.execute_verify = default__api__user_execute_verify;
	plugin->interface.api.user.execute_autoenrollment = default__api__user_execute_autoenrollment;
	plugin->interface.api.otp.create = default__api__otp_create;
	plugin->interface.api.otp.delete = default__api__otp_delete;
	plugin->interface.api.otp.exists = default__api__otp_exists;
	plugin->interface.api.otp.execute_challenge = default__api__otp_execute_challenge;
	plugin->interface.api.otp.execute_response = default__api__otp_execute_response;
	plugin->interface.api.otp.execute_apply = default__api__otp_execute_apply;
	plugin->interface.events.otp.created = default__event__otp_created;
	plugin->interface.events.otp.deleted = default__event__otp_deleted;
	return TT_OK;
}


__attribute__ ((visibility ("hidden")))
int default__finalize() {
	return TT_OK;
}

