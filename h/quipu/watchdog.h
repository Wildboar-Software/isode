#ifdef LINUX
#include <signal.h>
#endif

void watch_dog (char *where);
void watch_dog_aux (char *where, unsigned secs);
void watch_dog_reset (void);
void watch_dog_final_reset (void);
void watch_dog_final (
    #ifdef LINUX
        __sighandler_t fn
    #else
        int (*fn) (int)
    #endif
);
