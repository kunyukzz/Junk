#include "paths.h"
#include "junk/jnk_log.h"

#include <string.h> // strlen

path_t path_normalize(const char *raw_path) {
    path_t out;
    char temp[XVX_MAX_PATH];
    u32 len = 0;

    for (u32 i = 0; raw_path[i] && i < XVX_MAX_PATH - 1; ++i) {
        temp[i] = (raw_path[i] == '\\') ? '/' : raw_path[i];
        len = i + 1;
    }
    temp[len] = '\0';

    char *tokens[128];
    u32 token_count = 0;

    const char *token = temp;
    while (*token && token_count < 128) {
        while (*token == '/') {
            ++token;
        }
        if (*token == '\0') break;

        char *token_start = (char *)token;
        while (*token && *token != '/') {
            ++token;
        }

        if (*token) {
            *((char *)token) = '\0';
            ++token;
        }

        if (strcmp(token_start, ".") == 0) {
            // skip it.
        } else if (strcmp(token_start, "..") == 0) {
            if (token_count > 0) {
                token_count--;
            }
        } else {
            tokens[token_count++] = token_start;
        }
    }

    len = 0;
    for (u32 i = 0; i < token_count; ++i) {
        u32 token_len = (u32)strlen(tokens[i]);

        if (len + token_len + 1 < XVX_MAX_PATH) {
            out.buffer[len++] = '/';
            memcpy(&out.buffer[len], tokens[i], token_len);
            len += token_len;
        }
    }

    if (len == 0) {
        out.buffer[0] = '/';
        len = 1;
    }

    out.buffer[len] = '\0';
    return out;
}

path_t path_join(const char *base, const char *extra) {
    if (extra == NULL || extra[0] == '\0') {
        jnk_log_fatal(CH_CORE, "called with empty subpath");
        path_t empty = {0};
        return empty;
    }

    char combined[XVX_MAX_PATH];
    u32 len = 0;

    for (; *base && len < XVX_MAX_PATH - 1; ++base) {
        combined[len++] = *base;
    }

    if (len > 0 && combined[len - 1] != '/') {
        combined[len++] = '/';
    }

    for (; *extra && len < XVX_MAX_PATH - 1; ++extra) {
        combined[len++] = *extra;
    }

    combined[len] = '\0';
    return path_normalize(combined);
}

b8 path_is_absolute(const char *path) {
#ifdef JNK_LINUX
    return path[0] == '/';
#elif JNK_WINDOWS
    return (path[0] && path[1] == ':') || (path[0] == '\\' || path[0] == '/');
#endif
}

b8 path_is_relative(const char *path) { return !path_is_absolute(path); }
