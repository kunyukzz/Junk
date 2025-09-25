#ifndef JUNK_WINDOW_TYPE_H
#define JUNK_WINDOW_TYPE_H

#include "junk/jnk_define.h"

JNK_EXTERN_C_BEGIN

typedef struct {
#if JNK_LINUX
    void *dpy;
    uptr win;
    uptr atom;
#elif JNK_WINDOWS
    void *hwnd;
#endif

    const char *title;
    uptr width;
    uptr height;
} jnk_window_t;

JNK_EXTERN_C_END
#endif // JUNK_WINDOW_TYPE_H
