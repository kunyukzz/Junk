#ifndef JUNK_FILESYSTEM_H
#define JUNK_FILESYSTEM_H

#include "junk/jnk_define.h"

JNK_EXTERN_C_BEGIN

typedef struct {
    void *handle;
    b8 is_valid;
} file_t;

typedef enum {
    READ_TEXT = 0x01,
    READ_BINARY = 0x02,
    WRITE_TEXT = 0x04,
    WRITE_BINARY = 0x08
} filemode_t;

b8 filesys_init(u64 *memory_req, void *state);
void filesys_kill(void *state);

b8 filesys_exist(const char *path);
b8 filesys_open(const char *path, filemode_t mode, file_t *handle);
void filesys_close(file_t *handle);

b8 filesys_size(file_t *handle, u64 *size);
b8 filesys_read_all_text(file_t *handle, char *text, u64 *out_read);
b8 filesys_read_all_binary(file_t *handle, u8 *out_byte, u64 *out_read);

JNK_EXTERN_C_END
#endif // JUNK_FILESYSTEM_H
