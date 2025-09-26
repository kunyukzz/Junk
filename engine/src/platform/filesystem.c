#include "filesystem.h"
#include "core/paths.h"
#include "platform/platform.h"

#include "junk/jnk_log.h"

#include <stdio.h>    // fopen, fclose, fseek, rewind, fread
#include <string.h>   // memset
#include <sys/stat.h> // struct stat

typedef struct {
    path_t base_path;
    b8 is_available;
} filesys_state_t;

static filesys_state_t *g_filesys = JNK_NULL;

b8 filesys_init(u64 *memory_req, void *state) {
    if (memory_req) *memory_req = sizeof(filesys_state_t);
    if (!state) return true;
    g_filesys = (filesys_state_t *)state;

    char cwd[XVX_MAX_PATH];
    if (!platform_get_current_dir(cwd, sizeof(cwd))) {
        jnk_log_fatal(CH_CORE, "failed to get current working directory");
        return false;
    }

    /* try './assets' directory */
    path_t path_1 = path_join(cwd, "assets");
    if (filesys_exist(path_1.buffer)) {
        g_filesys->base_path = path_1;
    } else {
        /* try '../assets' directory (if run from './bin' directory */
        path_t parent_dir = path_join(cwd, "..");
        path_t path_2 = path_join(parent_dir.buffer, "assets");
        if (filesys_exist(path_2.buffer)) {
            g_filesys->base_path = path_2;
        } else {
            jnk_log_warn(CH_CORE, "could not locate assets folder (cwd=%s)",
                         cwd);
            g_filesys->base_path = (path_t){0};
            g_filesys->is_available = false;
            return false;
        }
    }

    g_filesys->is_available = true;
    jnk_log_info(CH_CORE, "Filesystem initialized base path = %s",
                 g_filesys->base_path.buffer);
    return true;
}

void filesys_kill(void *state) {
    if (state) {
        memset(state, 0, sizeof(filesys_state_t));
    }
}

b8 filesys_exist(const char *path) {
    struct stat buffer;
    return stat(path, &buffer) == 0;
}

b8 filesys_open(const char *path, filemode_t mode, file_t *handle) {
    handle->is_valid = false;
    handle->handle = JNK_NULL;
    const char *mode_str = JNK_NULL;

    b8 is_read = (mode & (READ_TEXT | READ_BINARY)) != 0;
    b8 is_write = (mode & (WRITE_TEXT | WRITE_BINARY)) != 0;
    b8 is_binary = (mode & (READ_BINARY | WRITE_BINARY)) != 0;

    if (is_read && is_write) {
        jnk_log_error(CH_CORE,
                      "cannot open file '%s' in both read & write mode", path);
        return false;
    }

    if (is_read) {
        mode_str = is_binary ? "rb" : "r";
    } else if (is_write) {
        mode_str = is_binary ? "wb" : "w";
    }

    if (!mode_str) {
        jnk_log_error(CH_CORE, "invalid file mode flags for '%s'", path);
        return false;
    }

    char full_path[XVX_MAX_PATH];
    path_t joined = path_join(g_filesys->base_path.buffer, path);
    strncpy(full_path, joined.buffer, XVX_MAX_PATH);
    FILE *file = fopen(full_path, mode_str);
    if (!file) {
        jnk_log_error(CH_CORE, "error open file '%s'", full_path);
        return false;
    }

    handle->handle = file;
    handle->is_valid = true;
    return true;
}

void filesys_close(file_t *handle) {
    if (handle->is_valid && handle->handle) {
        fclose((FILE *)handle->handle);
        handle->handle = JNK_NULL;
        handle->is_valid = false;
    }
}

b8 filesys_size(file_t *handle, u64 *size) {
    if (handle->handle) {
        fseek((FILE *)handle->handle, 0, SEEK_END);
        *size = (u64)ftell((FILE *)handle->handle);
        rewind((FILE *)handle->handle);
        return true;
    }
    return false;
}

b8 filesys_read_all_text(file_t *handle, char *text, u64 *out_read) {
    if (handle->handle && text && out_read) {
        u64 size = 0;

        if (!filesys_size(handle, &size)) {
            return false;
        }

        *out_read = fread(text, 1, size, (FILE *)handle->handle);
        return *out_read == size;
    }
    return false;
}

b8 filesys_read_all_binary(file_t *handle, u8 *out_byte, u64 *out_read) {
    if (handle->handle && out_byte && out_read) {
        u64 size = 0;

        if (!filesys_size(handle, &size)) {
            return false;
        }

        *out_read = fread(out_byte, 1, size, (FILE *)handle->handle);
        return *out_read == size;
    }
    return false;
}
