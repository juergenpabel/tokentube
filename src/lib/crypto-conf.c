#include "libtokentube.h"


__attribute__ ((visibility ("hidden")))
const char* libtokentube_crypto_get_cipher() {
	return libtokentube_name2oid(g_crypto_cipher);
}


__attribute__ ((visibility ("hidden")))
const char* libtokentube_crypto_get_hash() {
	return libtokentube_name2oid(g_crypto_hash);
}


__attribute__ ((visibility ("hidden")))
const char* libtokentube_crypto_get_kdf() {
	return g_crypto_kdf;
}


__attribute__ ((visibility ("hidden")))
int libtokentube_crypto_get_kdf_iter() {
	return g_crypto_kdf_iter;
}

