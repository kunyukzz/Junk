#include "window.h"
#include "junk/jnk_log.h"
#include "junk/jnk_input.h"
#include "event/input.h"
#include "event/event.h"

static window_state_t *g_win = JNK_NULL;

#if JNK_LINUX
#    include <X11/Xlib.h>
#    include <X11/XKBlib.h>

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

    jnk_log_info(CH_CORE, "Window system initialized (X11)");
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
    jnk_log_info(CH_CORE, "Window system kill (X11)");
}

#elif JNK_WINDOWS
#    include <windows.h>

static LRESULT CALLBACK jnk_win32_window_proc(HWND hwnd, UINT msg,
                                              WPARAM wparam, LPARAM lparam) {
    switch (msg) {
        case WM_KEYDOWN:
        case WM_KEYUP: {
            b8 pressed = (msg == WM_KEYDOWN);
            jnk_keys_t key = keycode_translate_win32((u32)wparam);
            input_process_key(key, pressed);
            return 0;
        }

        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP: {
            b8 pressed = (msg == WM_LBUTTONDOWN);
            input_process_button(JNK_MB_LEFT, pressed);
            return 0;
        }

        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP: {
            b8 pressed = (msg == WM_RBUTTONDOWN);
            input_process_button(JNK_MB_RIGHT, pressed);
            return 0;
        }

        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP: {
            b8 pressed = (msg == WM_MBUTTONDOWN);
            input_process_button(JNK_MB_MIDDLE, pressed);
            return 0;
        }

        case WM_MOUSEMOVE: {
            i16 x = GET_X_LPARAM(lparam);
            i16 y = GET_Y_LPARAM(lparam);
            input_process_mouse_move(x, y);
            return 0;
        }

        case WM_SIZE: {
            if (g_win->config->resizable) {
                jnk_event_t e;
                e.data.resize.width = (u16)LOWORD(lparam);
                e.data.resize.height = (u16)HIWORD(lparam);
                event_push(JNK_RESIZE, &e, JNK_NULL);
            }
            return 0;
        }

        case WM_ACTIVATE: {
            if (wparam == WA_INACTIVE) {
                jnk_event_t e;
                event_push(JNK_SUSPEND, &e, JNK_NULL);
            } else {
                jnk_event_t e;
                event_push(JNK_RESUME, &e, JNK_NULL);
            }
            return 0;
        }

        case WM_CLOSE: {
            jnk_event_t e;
            event_push(JNK_QUIT, &e, JNK_NULL);
            return 0;
        }

        case WM_DESTROY: {
            PostQuitMessage(0);
            return 0;
        }

        default: return DefWindowProcA(hwnd, msg, wparam, lparam);
    }
}

b8 window_system_init(u64 *memory_req, void *state) {
    if (memory_req) *memory_req = sizeof(window_state_t);
    if (!state) return true;
    g_win = (window_state_t *)state;

    const jnk_window_config_t *config = g_win->config;
    if (!config) {
        jnk_log_fatal(CH_CORE, "window config was not setup!");
        return false;
    }

    HINSTANCE hInstance = GetModuleHandleA(JNK_NULL);

    WNDCLASSEXA wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXA);
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = jnk_win32_window_proc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursorA(JNK_NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszClassName = "JunkWindowClass";

    if (!RegisterClassExA(&wc)) {
        jnk_log_fatal(CH_CORE, "Failed to register window class");
        return false;
    }

    // Calculate window size with style
    DWORD style = WS_OVERLAPPEDWINDOW;
    DWORD exStyle = WS_EX_APPWINDOW;

    RECT rect = {0, 0, (LONG)config->width, (LONG)config->height};
    AdjustWindowRectEx(&rect, style, 0, exStyle);

    HWND hwnd = CreateWindowExA(exStyle, "JunkWindowClass", config->title,
                                style, CW_USEDEFAULT, CW_USEDEFAULT,
                                rect.right - rect.left, rect.bottom - rect.top,
                                JNK_NULL, JNK_NULL, hInstance, JNK_NULL);

    if (!hwnd) {
        jnk_log_fatal(CH_CORE, "Failed to create window");
        return false;
    }

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    g_win->window.hwnd = (void *)hwnd;
    g_win->window.width = config->width;
    g_win->window.height = config->height;

    jnk_log_info(CH_CORE, "Window system initialized (Win32)");
    return true;
}

b8 window_system_pump(void) {
    MSG msg;
    b8 is_quit = false;

    while (PeekMessageA(&msg, JNK_NULL, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            is_quit = true;
        }

        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    return !is_quit;
}

void window_system_kill(void *state) {
    if (!state) return;

    HWND hwnd = (HWND)g_win->window.hwnd;
    if (hwnd) {
        DestroyWindow(hwnd);
        g_win->window.hwnd = JNK_NULL;
    }

    HINSTANCE hInstance = GetModuleHandleA(JNK_NULL);
    UnregisterClassA("JunkWindowClass", hInstance);

    jnk_log_info(CH_CORE, "Window system kill (Win32).");
}

#endif

window_state_t *window_system_get_window(void) { return g_win; }
