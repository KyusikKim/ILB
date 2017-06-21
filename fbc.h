#ifndef FBC_H

#include <pthread.h>

#define FBC_DEV_FB		"/dev/fb0"
#define FBC_DIR_FBCACHE	"/root/.ilb_cache"

#define FBC_MAX_URL_REQUESTED 1024
#define FBC_MAX_PATH 1024

#define FBC_TTY_SETTING_MODE 0
#define FBC_TTY_SETTING_KEYBOARD 1

#ifdef FBC_ENABLED 
extern pthread_t fbc_t_capturer;
#endif

extern char fbc_url_requested[FBC_MAX_URL_REQUESTED];
extern bool fbc_failed;

void fbc_debug(const char id[], const char msg[]);
void fbc_debug_time(const char id[]);

#ifdef FBC_ENABLED
bool fbc_store(char url[]);
bool fbc_restore(char url[]);
#endif 

#endif

