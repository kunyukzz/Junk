#include "event.h"
#include "container/dyn_array.h"
#include "junk/jnk_log.h"

#include <inttypes.h>

#define MAX_MSG_CODE (JNK_MAX + 1)

typedef struct {
    on_event callback;
    void *recipient;
} ev_handler;

typedef struct {
    ev_handler *events;
} ev_entry;

typedef struct {
    ev_entry reg[MAX_MSG_CODE];
} ev_state;

static ev_state *g_ev = JNK_NULL;

static uptr pointer_offset(const void *base, const void *ptr) {
    return (uptr)ptr - (uptr)base;
}

void log_ptr_offset(const char *label, const void *base, const void *ptr) {
    uintptr_t offset = pointer_offset(base, ptr);

    jnk_log_trace(CH_MEMS,
                  "%s at B=0x%06" PRIxPTR ", T=0x%06" PRIxPTR
                  ", +0x%06" PRIxPTR,
                  label, (uptr)base, (uptr)ptr & 0xFFFFFF, offset);
    (void)label;
    (void)offset;
}

b8 event_system_init(u64 *memory_req, void *state) {
    if (memory_req) *memory_req = sizeof(ev_state);
    if (!state) return true;

    g_ev = (ev_state *)state;
    jnk_log_info(CH_CORE, "Event system initialized.");
    return true;
}

void event_system_kill(void *state) {
    (void)state;
    if (g_ev) {
        for (u32 i = 0; i < JNK_MAX; ++i) {
            if (g_ev->reg[i].events != 0) {
                jnk_darray_kill((void **)&g_ev->reg[i].events);
                g_ev->reg[i].events = JNK_NULL;
            }
        }
    }

    jnk_log_info(CH_INPUT, "Event system kill");
    g_ev = 0;
}

b8 event_reg(u32 type, on_event handler, void *recipient) {
    if (!g_ev) return false;
    if (type >= JNK_MAX) return false;

    if (g_ev->reg[type].events == 0) {
        g_ev->reg[type].events = jnk_darray_init(sizeof(ev_handler));
        // log_ptr_offset("event_reg", g_ev->reg, g_ev->reg[type].events);
    }

    u64 count = jnk_darray_length(g_ev->reg[type].events);
    for (u64 i = 0; i < count; ++i) {
        if (g_ev->reg[type].events[i].recipient == recipient) {
            return false;
        }
    }

    ev_handler evh = {};
    evh.recipient = recipient;
    evh.callback = handler;
    jnk_darray_push((void **)&g_ev->reg[type].events, &evh, sizeof(ev_handler));
    return true;
}

b8 event_unreg(u32 type, on_event handler, void *recipient) {
    if (!g_ev) return false;
    if (g_ev->reg[type].events == 0) return false;
    if (type >= JNK_MAX) return false;

    u64 count = jnk_darray_length(g_ev->reg[type].events);
    for (u64 i = 0; i < count; ++i) {
        ev_handler *evh = &g_ev->reg[type].events[i];

        if (evh->recipient == recipient && evh->callback == handler) {
            ev_handler pops;
            jnk_darray_pop_at(g_ev->reg[type].events, i, &pops);
            // log_ptr_offset("event_unreg", g_ev->reg, g_ev->reg[type].events);
            return true;
        }
    }
    return false;
}

b8 event_push(u32 type, const jnk_event_t *event, void *sender) {
    if (!g_ev) return false;
    if (g_ev->reg[type].events == 0) return false;
    if (type >= JNK_MAX) return false;

    u64 count = jnk_darray_length(g_ev->reg[type].events);
    for (u64 i = 0; i < count; ++i) {
        ev_handler *evh = &g_ev->reg[type].events[i];

        if (evh->callback(type, (jnk_event_t *)event, sender, evh->recipient))
            return true;
    }
    return false;
}
