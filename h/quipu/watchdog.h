#ifdef LINUX
#include <signal.h>
#endif

void watch_dog (const char *where);
void watch_dog_aux (const char *where, const unsigned secs);
void watch_dog_reset (void);
void watch_dog_final_reset (void);
void watch_dog_final (
    #ifdef LINUX
        __sighandler_t fn
    #else
        int (*fn) (int)
    #endif
);
