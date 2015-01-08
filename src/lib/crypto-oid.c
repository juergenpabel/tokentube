#include <string.h>
#include "libtokentube.h"


static const char* name2oid[] = {
	"MD5", "1.2.840.113549.2.5",
	"SHA-1", "1.3.14.3.2.26",
	"SHA-256", "2.16.840.1.101.3.4.2.1",
	"SHA-384", "2.16.840.1.101.3.4.2.2",
	"SHA-512", "2.16.840.1.101.3.4.2.3",
	"AES-128-CBC", "2.16.840.1.101.3.4.1.2",
	"AES-128-OFB", "2.16.840.1.101.3.4.1.3",
	"AES-128-CFB", "2.16.840.1.101.3.4.1.4",
	"AES-192-CBC", "2.16.840.1.101.3.4.1.22",
	"AES-192-OFB", "2.16.840.1.101.3.4.1.23",
	"AES-192-CFB", "2.16.840.1.101.3.4.1.24",
	"AES-256-CBC", "2.16.840.1.101.3.4.1.42",
	"AES-256-OFB", "2.16.840.1.101.3.4.1.43",
	"AES-256-CFB", "2.16.840.1.101.3.4.1.44",
	NULL
};


__attribute__ ((visibility ("hidden")))
const char* libtokentube_name2oid(const char* name) {
	int	i = 0;

	while( name2oid[i] != NULL ) {
		if( strncasecmp( name, name2oid[i], 32 ) == 0 ) {
			return	name2oid[i+1];
		}
		i += 2;
	}
	return name;
}

