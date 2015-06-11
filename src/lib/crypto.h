#ifndef __LIBTOKENTUBE_CRYPT_H__
#define __LIBTOKENTUBE_CRYPT_H__


extern char g_crypto_cipher[];
extern char g_crypto_hash[];
extern char g_crypto_kdf[];
extern int  g_crypto_kdf_iter;
extern int  g_crypto_otp_bits;


const char* libtokentube_crypto_get_cipher();
const char* libtokentube_crypto_get_hash();
const char* libtokentube_crypto_get_kdf();
int         libtokentube_crypto_get_kdf_iter();


/*[TT_OK|TT_ERR]*/ int libtokentube_crypto_random(const void* data, size_t data_size);
/*[TT_OK|TT_ERR]*/ int libtokentube_crypto_encrypt(void* data, const size_t data_size, const void* key, const size_t key_size, const void* iv, const size_t iv_size);
/*[TT_OK|TT_ERR]*/ int libtokentube_crypto_decrypt(void* data, const size_t data_size, const void* key, const size_t key_size, const void* iv, const size_t iv_size);
/*[TT_OK|TT_ERR]*/ int libtokentube_crypto_hash(const void* in, size_t in_size, void* out, size_t* out_size);
/*[TT_OK|TT_ERR]*/ int libtokentube_crypto_hmac(const void* in, size_t in_size, const void* key, size_t key_size, void* out, size_t* out_size);
/*[TT_OK|TT_ERR]*/ int libtokentube_crypto_kdf(const void* salt, size_t salt_size, const void* in, size_t in_size, void* out, size_t* out_size);


typedef enum {
        CRYPT_UNKNOWN = 0,
        CRYPT_ENCRYPT = 1,
        CRYPT_DECRYPT = 2
} tt_cryptmode_t;


int libtokentube_crypto_crypt_impl(tt_cryptmode_t mode, const char* oid, void* data, const size_t data_size, const void* key, const size_t keylen, const void* iv, const size_t ivlen);
int libtokentube_crypto_hash_impl(const char* oid, const void* in, size_t in_size, void* out, size_t* out_size);
int libtokentube_crypto_hmac_impl(const char* oid, const void* in, size_t in_size, const void* key, size_t key_size, void* out, size_t* out_size);
int libtokentube_crypto_kdf_impl(const char* kdf, size_t kdf_iter, const char* oid, const void* salt, size_t salt_size, const void* in, size_t in_size, void* out, size_t* out_size);

const char* libtokentube_name2oid(const char* name);

#endif /* __LIBTOKENTUBE_CRYPT_H__ */

