#ifndef JUNK_ARENA_ALLOC_H
#define JUNK_ARENA_ALLOC_H

#include "junk/jnk_define.h"

JNK_EXTERN_C_BEGIN

typedef struct {
    u64 total_size;
    u64 prev_offset;
    u64 curr_offset;
    void *memory;
    b8 own_memory;
} arena_alloc_t;

b8 arena_set(u64 total_size, void *memory, arena_alloc_t *arena);
void arena_end(arena_alloc_t *arena);
void *arena_alloc_align(arena_alloc_t *arena, u64 size, u8 alignment);

JNK_API void *arena_alloc(arena_alloc_t *arena, u64 size);
JNK_API void arena_reset(arena_alloc_t *arena);

JNK_EXTERN_C_END
#endif // JUNK_ARENA_ALLOC_H
