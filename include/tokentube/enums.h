#ifndef __TT_LIBRARY_ENUMS_H__
#define __TT_LIBRARY_ENUMS_H__


typedef enum {
	TT_LOG__UNDEFINED = 0,
	TT_LOG__FATAL = 1,
	TT_LOG__ERROR = 2,
	TT_LOG__WARN = 3,
	TT_LOG__INFO = 4
} tt_loglevel_t;


typedef enum {
	TT_DEBUG__UNDEFINED = 0,
	TT_DEBUG__VERBOSITY0 = 0,
	TT_DEBUG__VERBOSITY1 = 1,
	TT_DEBUG__VERBOSITY2 = 2,
	TT_DEBUG__VERBOSITY3 = 3,
	TT_DEBUG__VERBOSITY4 = 4,
	TT_DEBUG__VERBOSITY5 = 5,
	TT_DEBUG__TRACE = 9
} tt_debuglevel_t;


typedef enum {
	TT_STATUS__UNDEFINED = 0,
	TT_STATUS__YES = 1,
	TT_STATUS__NO = 2
} tt_status_t;


typedef enum {
	TT_MODIFY__UNDEFINED = 0,
	TT_MODIFY__USER_PASSWORD = 1,
	TT_MODIFY__USER_KEY_ADD = 2,
	TT_MODIFY__USER_KEY_DEL = 3,
	TT_MODIFY__USER_OTP_KEY = 4
} tt_modify_t;


typedef enum {
	TT_RUNTIME__UNDEFINED = 0,
	TT_RUNTIME__PBA = 1,
	TT_RUNTIME__STANDARD = 2,
	TT_RUNTIME__INVALID = 3
} tt_runtime_t;


typedef enum {
	TT_FILE__UNDEFINED = 0,
	TT_FILE__CONFIG_PBA = 1,
	TT_FILE__CONFIG_STANDARD = 2,
	TT_FILE__KEY = 3,
	TT_FILE__USER = 4,
	TT_FILE__OTP = 5,
	TT_FILE__UHD = 6,
	TT_FILE__INVALID = 7
} tt_file_t;


typedef enum {
	TT_EVENT__UNDEFINED = 0,
	TT_EVENT__USER_CREATED = 1,
	TT_EVENT__USER_MODIFIED = 2,
	TT_EVENT__USER_DELETED = 3,
	TT_EVENT__OTP_CREATED = 4,
	TT_EVENT__OTP_MODIFIED = 5,
	TT_EVENT__OTP_DELETED = 6,
	TT_EVENT__UHD_CREATED = 7,
	TT_EVENT__UHD_MODIFIED = 8,
	TT_EVENT__UHD_DELETED = 9,
	TT_EVENT__AUTH_USER = 10,
	TT_EVENT__AUTH_OTP = 11,
	TT_EVENT__INVALID = 12
} tt_event_t;


#endif /* __TT_LIBRARY_ENUMS_H__ */

