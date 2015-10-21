#include <string.h>
#include "libtokentube.h"


typedef struct {
	const char* name;
	const char* oid;
} crypto_cipher_t;


static crypto_cipher_t ciphers[] = {
	{ "MD5", "1.2.840.113549.2.5" },
	{ "SHA-1", "1.3.14.3.2.26" },
	{ "SHA-256", "2.16.840.1.101.3.4.2.1" },
	{ "SHA-384", "2.16.840.1.101.3.4.2.2" },
	{ "SHA-512", "2.16.840.1.101.3.4.2.3" },
	{ "AES-128-CBC", "2.16.840.1.101.3.4.1.2" },
	{ "AES-128-OFB", "2.16.840.1.101.3.4.1.3" },
	{ "AES-128-CFB", "2.16.840.1.101.3.4.1.4" },
	{ "AES-128-GCM", "2.16.840.1.101.3.4.1.6" },
	{ "AES-128-CCM", "2.16.840.1.101.3.4.1.7" },
	{ "AES-192-CBC", "2.16.840.1.101.3.4.1.22" },
	{ "AES-192-OFB", "2.16.840.1.101.3.4.1.23" },
	{ "AES-192-CFB", "2.16.840.1.101.3.4.1.24" },
	{ "AES-192-GCM", "2.16.840.1.101.3.4.1.26" },
	{ "AES-192-CCM", "2.16.840.1.101.3.4.1.27" },
	{ "AES-256-CBC", "2.16.840.1.101.3.4.1.42" },
	{ "AES-256-OFB", "2.16.840.1.101.3.4.1.43" },
	{ "AES-256-CFB", "2.16.840.1.101.3.4.1.44" },
	{ "AES-256-GCM", "2.16.840.1.101.3.4.1.46" },
	{ "AES-256-CCM", "2.16.840.1.101.3.4.1.47" }
};


__attribute__ ((visibility ("hidden")))
const char* libtokentube_name2oid(const char* name) {
	size_t  i = 0;

	for( i=0; i<sizeof(ciphers)/sizeof(crypto_cipher_t); i++ ) {
		if( strncasecmp( name, ciphers[i].name, TT_IDENTIFIER_CHAR_MAX ) == 0 ) {
			return ciphers[i].oid;
		}
	}
	return name;
}

