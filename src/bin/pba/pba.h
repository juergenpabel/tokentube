#include <tokentube.h>

int pba_initialize(tt_library_t*, const char*, const char*);

int pba_ext2_readfile(const char*, const char*, char*, unsigned int);
int pba_ext2_writefile(const char*, const char*, char*, unsigned int);
int pba_user_loadkey(tt_library_t* api, const char*, size_t, const char*, size_t, char*, size_t*);

int pba_plymouth(const char*, const char*, char**, size_t*, char**, size_t*);

int pba_ssod_start();
int pba_ssod_stop();
int pba_ssod_credentials(const char*, const char*);

int pba_otp(tt_library_t*, const char*, char*, size_t*);
