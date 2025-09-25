#ifndef JUNK_MEMORY_H
#define JUNK_MEMORY_H

#include "jnk_define.h"

JNK_EXTERN_C_BEGIN

typedef enum {
    MEM_UNKNOWN = 0x00,
    MEM_ENGINE,
    MEM_GAME,
    MEM_ARENA,
    MEM_RENDER,
    MEM_AUDIO,
    MEM_ARRAY,
    MEM_DYNARRAY,
    MEM_STRING,
    MEM_RESOURCE,
    MEM_MAX_TAG
} jnk_memtag_t;

#define JMALLOC(size, tag) alloc_dbg(size, tag, __FILE__, __LINE__)
#define JFREE(block, size, tag) alloc_free(block, size, tag)

b8 memory_system_init(u64 total_size);

void memory_system_kill(void);

JNK_API void *alloc_dbg(u64 size, jnk_memtag_t tag, const char *file, u32 line);

JNK_API void alloc_free(void *block, u64 size, jnk_memtag_t tag);

char *mem_debug_stat(void);

void memory_report_leaks(void);

JNK_EXTERN_C_END
#endif // JUNK_MEMORY_H
