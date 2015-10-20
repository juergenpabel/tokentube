#include <confuse.h>

#define PBA_PLAIN 1
#define PBA_USER 2
#define PBA_OTP  4


int pba_initialize(tt_library_t*, const char*, const char*);
int pba_cfg_include(cfg_t* cfg, cfg_opt_t* opt, int argc, const char** argv);

int pba_plain(cfg_t* cfg, tt_library_t* library, const char* identifier, char* key, size_t* key_size, tt_status_t* status);
int pba_user(cfg_t*, tt_library_t* library, const char* identifier, char* key, size_t* key_size, tt_status_t* status);
int pba_otp(cfg_t*, tt_library_t* library, const char* identifier, char* key, size_t* key_size, tt_status_t* status);

int pba_plymouth_splash(const char* text, int pba_mode);
int pba_plymouth_plain(const char* message_plain, const char* prompt_plain, char* pass, size_t* pass_size, tt_status_t* status);
int pba_plymouth_user(const char* message_user, const char* message_pass, const char* prompt_user, const char* prompt_pass, char* user, size_t* user_size, char* pass, size_t* pass_size, tt_status_t* status);
int pba_plymouth_otp(const char* message, const char* prompt, char* response, size_t response_size, tt_status_t* status);

int pba_sso_start(const char*, const char*, const char*);
int pba_sso_stop(const char*);
int pba_sso_credentials(const char*, const char*, const char*);

