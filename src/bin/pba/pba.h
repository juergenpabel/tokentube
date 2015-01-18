#define SSOD_EXECUTABLE	"/lib/tokentube/ssod"
#define SSOD_SOCKET	TT_FILENAME__SSOD_INITRAMFS_DIR "/" TT_FILENAME__SSOD_TOKENTUBE_DIR "/" TT_FILENAME__SSOD_SOCKET


int pba_initialize(tt_library_t*, const char*, const char*);

int pba_ext2_readfile(const char*, const char*, char*, size_t*);
int pba_ext2_writefile(const char*, const char*, char*, size_t*);

int pba_user_loadkey(tt_library_t* api, const char*, size_t, const char*, size_t, char*, size_t*);

int pba_plymouth(const char*, const char*, char**, size_t*, char**, size_t*);

int pba_ssod_start(const char*, const char*, const char*);
int pba_ssod_stop(const char*);
int pba_ssod_credentials(const char*, const char*, const char*);

int pba_otp(tt_library_t*, const char*, char*, size_t*);
int pba_otp_plymouth(const char* challenge, char* response);

