#ifndef __LIBTOKENTUBE_UTIL_H__
#define __LIBTOKENTUBE_UTIL_H__


static inline char* libtokentube_util_version_to_string(tt_version_t version, char* buffer, size_t buffer_size) {
        snprintf( buffer, buffer_size-1, "%d.%d.%d", version.major, version.minor, version.patch );
        return buffer;
}
#define VERSION_TO_STR(v,s) (libtokentube_util_version_to_string(v, s, sizeof(s)))

int libtokentube_util_posix_mkdir(const char* path);
int libtokentube_util_posix_copy(const char* src, const char* dst);
int libtokentube_util_device_find(dev_t device, char* buffer, size_t* buffer_size);

/*[TT_OK|TT_ERR]*/ int libtokentube_util_crc16(const char const* input, size_t input_size, int* result);

/*[TT_OK|TT_ERR]*/ int libtokentube_util_hex_encode(char const*, size_t, char*, size_t*);
/*[TT_OK|TT_ERR]*/ int libtokentube_util_hex_decode(char const*, size_t, char*, size_t*);

/*[TT_OK|TT_ERR]*/ int libtokentube_util_base32_encode(const char* bytes, size_t bytes_size, char* text, size_t* text_size);
/*[TT_OK|TT_ERR]*/ int libtokentube_util_base32_decode(const char* text, size_t text_size, char* bytes, size_t* bytes_size);

/*[TT_OK|TT_ERR]*/ int libtokentube_util_base64_encode(const char* bytes, size_t bytes_size, char* text, size_t* text_size);
/*[TT_OK|TT_ERR]*/ int libtokentube_util_base64_decode(const char* text, size_t text_size, char* bytes, size_t* bytes_size);


#endif /* __LIBTOKENTUBE_UTIL_H__ */

