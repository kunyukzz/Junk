#include "window.h"
#include "junk/jnk_log.h"
#include "junk/jnk_input.h"
#include "event/input.h"
#include "event/event.h"

#if JNK_LINUX
#    include <X11/Xlib.h>
#    include <X11/XKBlib.h>

static window_state_t *g_win = JNK_NULL;

b8 window_system_init(u64 *memory_req, void *state) {
    if (memory_req) *memory_req = sizeof(window_state_t);
    if (!state) return true;
    g_win = (window_state_t *)state;

    const jnk_window_config_t *config = g_win->config;
    if (!config) {
        jnk_log_fatal(CH_CORE, "window config was not setup!");
        return false;
    }

    Display *dpy = XOpenDisplay(JNK_NULL);
    if (!dpy) {
        jnk_log_fatal(CH_CORE, "XOpenDisplay failed.");
        return false;
    }

    i32 screen = DefaultScreen(dpy);

    XSetWindowAttributes attrs;
    attrs.background_pixel = BlackPixel(dpy, screen);
    attrs.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
                       StructureNotifyMask | ButtonPressMask |
                       ButtonReleaseMask;

    u32 value_mask = CWBackPixel | CWEventMask;

    Window win = XCreateWindow(dpy, RootWindow(dpy, screen), 0, 0,
                               config->width, config->height, 0, CopyFromParent,
                               InputOutput, CopyFromParent, value_mask, &attrs);

    if (!win) {
        jnk_log_fatal(CH_CORE, "XCreateWindow failed.");
        XCloseDisplay(dpy);
        return false;
    }

    Atom wm_delete = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(dpy, win, &wm_delete, 1);
    XStoreName(dpy, win, config->title);
    XMapWindow(dpy, win);
    XFlush(dpy);

    g_win->window.dpy = (void *)dpy;
    g_win->window.win = (uptr)win;
    g_win->window.atom = (uptr)wm_delete;
    g_win->window.width = config->width;
    g_win->window.height = config->height;

    jnk_log_info(CH_CORE, "Window system initialized.");
    return true;
}

b8 window_system_pump(void) {
    XEvent ev;
    b8 is_quit = false;

    while (XPending(g_win->window.dpy)) {
        XNextEvent(g_win->window.dpy, &ev);

        switch (ev.type) {
            case KeyPress:
            case KeyRelease: {
                b8 pressed = ev.type == KeyPress;
                KeySym ks =
                    XkbKeycodeToKeysym(g_win->window.dpy,
                                       (KeyCode)ev.xkey.keycode, 0,
                                       (ev.xkey.state & ShiftMask) ? 1 : 0);
                jnk_keys_t key = keycode_translate((u32)ks);
                input_process_key(key, pressed);
            } break;

            case ButtonPress:
            case ButtonRelease: {
                b8 pressed = ev.type == ButtonPress;
                jnk_button_t mb = JNK_MB_MAX;
                switch (ev.xbutton.button) {
                    case Button1: mb = JNK_MB_LEFT; break;
                    case Button2: mb = JNK_MB_MIDDLE; break;
                    case Button3: mb = JNK_MB_RIGHT; break;
                }
                if (mb != JNK_MB_MAX) input_process_button(mb, pressed);
            } break;

            case MotionNotify: {
                input_process_mouse_move((i16)ev.xmotion.x, (i16)ev.xmotion.y);
            } break;

            case ConfigureNotify: {
                const jnk_window_config_t *config = g_win->config;
                if (config->resizable) {
                    XConfigureEvent *ce = (XConfigureEvent *)&ev;
                    jnk_event_t e;
                    e.data.resize.width = (u16)ce->width;
                    e.data.resize.height = (u16)ce->height;

                    event_push(JNK_RESIZE, &e, JNK_NULL);
                } else {
                    jnk_log_debug(CH_INPUT, "window not set resizable!");
                }
            } break;

            case UnmapNotify: {
                jnk_event_t e;
                event_push(JNK_SUSPEND, &e, JNK_NULL);
            } break;

            case MapNotify: {
                jnk_event_t e;
                event_push(JNK_RESUME, &e, JNK_NULL);
            } break;

            case ClientMessage: {
                XClientMessageEvent *cm = (XClientMessageEvent *)&ev;

                if ((Atom)cm->data.l[0] == g_win->window.atom) {
                    jnk_event_t e;
                    e.data.raw[0] = (u8)cm->window;
                    event_push(JNK_QUIT, &e, JNK_NULL);
                    is_quit = true;
                }
            } break;

            default: break;
        }
        return !is_quit;
    }
    return true;
}

void window_system_kill(void *state) {
    if (!state) return;
    Display *dpy = (Display *)g_win->window.dpy;
    Window win = (Window)g_win->window.win;

    if (win) {
        XDestroyWindow(dpy, win);
        g_win->window.win = 0;
    }
    if (dpy) {
        XCloseDisplay(dpy);
        g_win->window.dpy = JNK_NULL;
    }
    jnk_log_info(CH_CORE, "Window system kill.");
}

#elif JNK_WINDOWS

#endif

window_state_t *window_system_get_window(void) { return g_win; }
