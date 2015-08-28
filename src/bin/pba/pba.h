int pba_initialize(tt_library_t*, const char*, const char*);

int pba_user_loadkey(tt_library_t* api, const char*, size_t, const char*, size_t, const char*, char*, size_t*);

int pba_plymouth(const char*, const char*, char*, size_t*, char*, size_t*);

int pba_sso_start(const char*, const char*, const char*);
int pba_sso_stop(const char*);
int pba_sso_credentials(const char*, const char*, const char*);

int pba_otp(tt_library_t*, const char*, char*, size_t*);
int pba_otp_plymouth(const char* challenge, char* response);

#include <confuse.h>
int pba_cfg_include(cfg_t* cfg, cfg_opt_t* opt, int argc, const char** argv);
