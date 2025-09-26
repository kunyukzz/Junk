#include "arena.h"
#include "junk/jnk_memory.h"
#include "junk/jnk_assert.h"
#include "junk/jnk_log.h"

#define DEFAULT_ALIGNMENT 0x10

u64 arena_calc_req(const char *labels[], u64 *sizes, u32 count) {
    u64 offset = 0;
    for (u32 i = 0; i < count; ++i) {
        u64 aligned_off =
            (offset + (DEFAULT_ALIGNMENT - 1)) & ~(u64)(DEFAULT_ALIGNMENT - 1);
        u64 padding = aligned_off - offset;
        u64 aligned_size = sizes[i] + padding;

        jnk_log_debug(CH_MEMS,
                      "Arena: %-10s req= %4lluB align= %4lluB (pad= %2llu)",
                      labels[i], sizes[i], aligned_size, padding);

        offset = aligned_off + sizes[i];
    }

    jnk_log_debug(CH_MEMS, "Arena total: %lluB", offset);
    return offset;
}

b8 arena_set(u64 total_size, void *memory, arena_alloc_t *arena) {
    if (arena) {
        arena->total_size = total_size;
        arena->curr_offset = 0;
        arena->prev_offset = 0;
        arena->own_memory = memory == 0;

        if (memory) {
            arena->memory = memory;
        } else {
            arena->memory = JMALLOC(total_size, MEM_ARENA);
        }

        if (!arena->memory) {
            jnk_log_fatal(CH_MEMS, "failed to allocate memory (size: %llu)",
                          total_size);
            return false; // or handle gracefully
        }
    }
    return true;
}

void arena_end(arena_alloc_t *arena) {
    if (arena) {
        arena->curr_offset = 0;

        if (arena->memory) {
            jnk_log_info(CH_MEMS, "Arena success with freeing %lluB",
                         arena->total_size);
            JFREE(arena->memory, arena->total_size, MEM_ARENA);
        }

        arena->memory = JNK_NULL;
        arena->total_size = 0;
    }
}

void *arena_alloc_align(arena_alloc_t *arena, u64 size, u8 alignment) {
    jnk_assert_msg((alignment & (alignment - 1)) == 0,
                   "Alignment should be the power of 2");
    if (!arena || !arena->memory) {
        jnk_log_error(CH_MEMS, "provided arena not allocated");
        return JNK_NULL;
    }

    uptr curr_addr = ((uptr)arena->memory + arena->curr_offset);
    uptr align_addr = (curr_addr + (alignment - 1)) & ~(alignment - 1);
    u64 padding = align_addr - curr_addr;
    u64 total_size = size + padding;

    if (arena->curr_offset + total_size > arena->total_size) {
        u64 rem = arena->total_size - arena->curr_offset;
        jnk_log_error(CH_MEMS,
                      "tried to allocate %luB align to %luB. only %luB "
                      "remain",
                      size, alignment, rem);
        return JNK_NULL;
    }

    arena->prev_offset = arena->curr_offset;
    arena->curr_offset += total_size;

    return (void *)align_addr;
}

void *arena_alloc(arena_alloc_t *arena, u64 size) {
    return arena_alloc_align(arena, size, DEFAULT_ALIGNMENT);
}

void arena_reset(arena_alloc_t *arena) {
    if (arena) {
        arena->curr_offset = 0;
        arena->prev_offset = 0;
    }
}
