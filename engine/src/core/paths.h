#ifndef JUNK_PATH_H
#define JUNK_PATH_H

#include "junk/jnk_define.h"

#define XVX_MAX_PATH 512

JNK_EXTERN_C_BEGIN

typedef enum { JNK_PATH_PHYSICAL, JNK_PATH_VIRTUAL } path_type_t;

typedef struct {
    char buffer[XVX_MAX_PATH];
} path_t;

path_type_t path_classify(const char *paths);

path_t path_normalize(const char *raw_path);
path_t path_join(const char *base, const char *extra);
b8 path_is_absolute(const char *path);
b8 path_is_relative(const char *path);

JNK_EXTERN_C_END
#endif // JUNK_PATH_H
