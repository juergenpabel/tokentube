#ifndef __TT_LIBRARY_DEFINES_H__
#define __TT_LIBRARY_DEFINES_H__


#define TT_UNINITIALIZED	-127
#define TT_UNIMPLEMENTED	-126
#define TT_IGN			-2
#define TT_ERR			-1
#define TT_OK			0

#define TT_UNDEFINED		TT_STATUS__UNDEFINED
#define TT_YES			TT_STATUS__YES
#define TT_NO			TT_STATUS__NO

#define TT_USERNAME_CHAR_MAX	63
#define TT_PASSWORD_CHAR_MAX	255

#define TT_KEY_BITS_MIN		128
#define TT_KEY_BITS_MAX		8192

#define TT_CIPHER_BITS_MIN	128
#define TT_CIPHER_BITS_MAX	1024

#define TT_DIGEST_BITS_MIN	128
#define TT_DIGEST_BITS_MAX	2048

#define TT_OTP_BITS_MIN         64
#define TT_OTP_BITS_MAX         256

#define TT_OTP_TEXT_MIN         (5+(TT_OTP_BITS_MIN/4)+(TT_OTP_BITS_MIN/16)-1)
#define TT_OTP_TEXT_MAX         (5+(TT_OTP_BITS_MAX/4)+(TT_OTP_BITS_MAX/16)-1)

#define TT_CONFIG_MAX		4096

#define TT_FILENAME__TOKENTUBE_CONF	"/etc/tokentube/tokentube.conf"
#define TT_FILENAME__BOOT_CONF		"/etc/tokentube/boot.conf"


#endif /* __TT_LIBRARY_DEFINES_H__ */

