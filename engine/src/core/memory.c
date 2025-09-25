#include "junk/jnk_memory.h"
#include "junk/jnk_log.h"
#include "platform/platform.h"

#include <string.h>
#include <stdio.h>

#define MAX_ALLOC_TRACK 4096
#define BUFFER_SIZE 8192

static const char *tag_str[MEM_MAX_TAG] = {"MEM_UNKNOWN", "MEM_ENGINE",
                                           "MEM_GAME",    "MEM_ARENA",
                                           "MEM_RENDER",  "MEM_AUDIO",
                                           "MEM_ARRAY",   "MEM_DYNARRAY",
                                           "MEM_STRING",  "MEM_RESOURCE"};

struct status {
    u64 total_allocated;
    u64 tag_alloc_count[MEM_MAX_TAG];
    u64 tag_allocation[MEM_MAX_TAG];
};

typedef struct {
    jnk_memtag_t tag;
    u64 size;
    u32 line;
    const char *file;
    void *ptr;
} mem_state;

static struct status g_counter = {0};
static mem_state *g_mem;
static u64 g_mem_count = 0;
static u64 g_mem_capacity = 0;

void memory_report_leaks(void) {
    if (g_mem_count == 0) {
        jnk_log_info(CH_CORE, "No memory leaks detected.");
        return;
    }

    printf("\n");
    jnk_log_warn(CH_CORE, "====== MEMORY LEAKS (%lu) ======", g_mem_count);
    for (u64 i = 0; i < g_mem_count; ++i) {
        const mem_state *m = &g_mem[i];
        jnk_log_warn(CH_CORE, "at %s:%u → %lu bytes [%s]", m->file, m->line,
                     m->size, tag_str[m->tag]);
    }
}

b8 memory_system_init(u64 total_size) {
    g_mem_capacity = total_size / sizeof(mem_state);
    g_mem = platform_allocate(sizeof(mem_state) * g_mem_capacity, false);
    if (!g_mem) return false;

    g_mem_count = 0;
    g_counter = (struct status){0};

    return true;
}

void memory_system_kill(void) {
    if (g_mem) {
        memory_report_leaks();
        platform_free(g_mem, false);
        g_mem = 0;
        g_mem_count = 0;
        jnk_log_info(CH_CORE, "Memory system kill");
    }
}

void *alloc_dbg(u64 size, jnk_memtag_t tag, const char *file, u32 line) {
    void *block = platform_allocate(size, false);
    if (!block) {
        return 0;
    }

    memset(block, 0, size);

    if (g_mem_count < g_mem_capacity) {
        g_mem[g_mem_count++] = (mem_state){
            .ptr = block,
            .size = size,
            .tag = tag,
            .file = file,
            .line = line,
        };
    }

    g_counter.total_allocated += size;
    g_counter.tag_alloc_count[tag]++;
    g_counter.tag_allocation[tag] += size;

    return block;
}

void alloc_free(void *block, u64 size, jnk_memtag_t tag) {
    if (!block) {
        return;
    }

    b8 found = false;
    for (u64 i = 0; i < g_mem_count; ++i) {
        if (g_mem[i].ptr == block) {
            g_mem[i] = g_mem[--g_mem_count];
            found = true;

            break;
        }
    }

    if (!found) {
        jnk_log_warn(CH_MEMS, "attempted to free unknown ptr %p", block);
    }

    platform_free(block, false);

    g_counter.tag_alloc_count[tag]--;
    g_counter.tag_allocation[tag] -= size;
}

char *mem_debug_stat(void) {
    const u64 Gib = 1024 * 1024 * 1024;
    const u64 Mib = 1024 * 1024;
    const u64 Kib = 1024;

    static char buffer[BUFFER_SIZE];
    u64 offset = 0;
    offset += (u64)snprintf(buffer + offset, sizeof(buffer) - offset,
                            "Engine Memory Used:\n");

    for (u32 i = 0; i < MEM_MAX_TAG; ++i) {
        char *unit = "B";
        u32 count = (u32)g_counter.tag_alloc_count[i];
        float amount = (float)g_counter.tag_allocation[i];

        if (count == 0) {
            continue;
        }

        if (amount >= (float)Gib) {
            amount /= (float)Gib;
            unit = "Gib";
        } else if (amount >= (float)Mib) {
            amount /= (float)Mib;
            unit = "Mib";
        } else if (amount >= (float)Kib) {
            amount /= (float)Kib;
            unit = "Kib";
        }
        count = (u32)g_counter.tag_alloc_count[i];

        i32 length =
            snprintf(buffer + offset, sizeof(buffer) - offset,
                     "--> %s: [%u] %.2f%s\n", tag_str[i], count, amount, unit);

        if (length > 0 && (offset + (u32)length < BUFFER_SIZE)) {
            offset += (u32)length;
        } else {
            break;
        }
    }
    return buffer;
}
