#include "junk/jnk_input.h"

const char *key_names[JNK_KEY_MAX] = {
    [JNK_KEY_UNKNOWN] = "Unknown",
    [JNK_KEY_A] = "A",
    [JNK_KEY_B] = "B",
    [JNK_KEY_C] = "C",
    [JNK_KEY_D] = "D",
    [JNK_KEY_E] = "E",
    [JNK_KEY_F] = "F",
    [JNK_KEY_G] = "G",
    [JNK_KEY_H] = "H",
    [JNK_KEY_I] = "I",
    [JNK_KEY_J] = "J",
    [JNK_KEY_K] = "K",
    [JNK_KEY_L] = "L",
    [JNK_KEY_M] = "M",
    [JNK_KEY_N] = "N",
    [JNK_KEY_O] = "O",
    [JNK_KEY_P] = "P",
    [JNK_KEY_Q] = "Q",
    [JNK_KEY_R] = "R",
    [JNK_KEY_S] = "S",
    [JNK_KEY_T] = "T",
    [JNK_KEY_U] = "U",
    [JNK_KEY_V] = "V",
    [JNK_KEY_W] = "W",
    [JNK_KEY_X] = "X",
    [JNK_KEY_Y] = "Y",
    [JNK_KEY_Z] = "Z",
    [JNK_KEY_1] = "1",
    [JNK_KEY_2] = "2",
    [JNK_KEY_3] = "3",
    [JNK_KEY_4] = "4",
    [JNK_KEY_5] = "5",
    [JNK_KEY_6] = "6",
    [JNK_KEY_7] = "7",
    [JNK_KEY_8] = "8",
    [JNK_KEY_9] = "9",
    [JNK_KEY_0] = "0",
    [JNK_KEY_F1] = "F1",
    [JNK_KEY_F2] = "F2",
    [JNK_KEY_F3] = "F3",
    [JNK_KEY_F4] = "F4",
    [JNK_KEY_F5] = "F5",
    [JNK_KEY_F6] = "F6",
    [JNK_KEY_F7] = "F7",
    [JNK_KEY_F8] = "F8",
    [JNK_KEY_F9] = "F9",
    [JNK_KEY_F10] = "F10",
    [JNK_KEY_F11] = "F11",
    [JNK_KEY_F12] = "F12",
    [JNK_KEY_ESCAPE] = "Escape",
    [JNK_KEY_ENTER] = "Enter",
    [JNK_KEY_TAB] = "Tab",
    [JNK_KEY_BACKSPACE] = "Backspace",
    [JNK_KEY_INSERT] = "Insert",
    [JNK_KEY_DELETE] = "Delete",
    [JNK_KEY_HOME] = "Home",
    [JNK_KEY_END] = "End",
    [JNK_KEY_PAGEUP] = "Pageup",
    [JNK_KEY_PAGEDOWN] = "Pagedown",
    [JNK_KEY_LEFT] = "Left",
    [JNK_KEY_RIGHT] = "Right",
    [JNK_KEY_UP] = "Up",
    [JNK_KEY_DOWN] = "Down",
    [JNK_KEY_LSHIFT] = "Lshift",
    [JNK_KEY_RSHIFT] = "Rshift",
    [JNK_KEY_LCONTROL] = "Lcontrol",
    [JNK_KEY_RCONTROL] = "Rcontrol",
    [JNK_KEY_LALT] = "Lalt",
    [JNK_KEY_RALT] = "Ralt",
    [JNK_KEY_LSUPER] = "Lsuper",
    [JNK_KEY_RSUPER] = "Rsuper",
    [JNK_KEY_CAPS] = "Caps",
    [JNK_KEY_NUM] = "Num",
    [JNK_KEY_KP_0] = "Kp 0",
    [JNK_KEY_KP_1] = "Kp 1",
    [JNK_KEY_KP_2] = "Kp 2",
    [JNK_KEY_KP_3] = "Kp 3",
    [JNK_KEY_KP_4] = "Kp 4",
    [JNK_KEY_KP_5] = "Kp 5",
    [JNK_KEY_KP_6] = "Kp 6",
    [JNK_KEY_KP_7] = "Kp 7",
    [JNK_KEY_KP_8] = "Kp 8",
    [JNK_KEY_KP_9] = "Kp 9",
    [JNK_KEY_KP_DECIMAL] = "Kp Decimal",
    [JNK_KEY_KP_ENTER] = "Kp Enter",
    [JNK_KEY_KP_ADD] = "Kp Add",
    [JNK_KEY_KP_SUBSTRACT] = "Kp Substract",
    [JNK_KEY_KP_MULTIPLY] = "Kp Multiply",
    [JNK_KEY_KP_DIVIDE] = "Kp Divide",
    [JNK_KEY_SPACE] = "Space",
    [JNK_KEY_MINUS] = "Minus",
    [JNK_KEY_EQUAL] = "Equal",
    [JNK_KEY_LBRACKET] = "Lbracket",
    [JNK_KEY_RBRACKET] = "Rbracket",
    [JNK_KEY_LBRACE] = "Lbrace",
    [JNK_KEY_RBRACE] = "Rbrace",
    [JNK_KEY_BACKSLASH] = "Backslash",
    [JNK_KEY_SEMICOLON] = "Semicolon",
    [JNK_KEY_APOSTROPHE] = "Apostrophe",
    [JNK_KEY_GRAVE] = "Grave",
    [JNK_KEY_COMMA] = "Comma",
    [JNK_KEY_PERIOD] = "Period",
    [JNK_KEY_SLASH] = "Slash",
    [JNK_KEY_PRINT_SCREEN] = "Print Screen",
    [JNK_KEY_SCROLL_LOCK] = "Scroll Lock",
    [JNK_KEY_PAUSE] = "Pause",
};

const char *keycode_to_str(jnk_keys_t key) {
    if (key >= 0 && key < JNK_KEY_MAX) {
        const char *s = key_names[key];
        return s ? s : "Unknown";
    }
    return "Invalid";
}

#if JNK_LINUX
#    include <X11/keysym.h>

jnk_keys_t keycode_translate(u32 keycode) {
    switch (keycode) {
        // Alphanumeric
        case XK_a:
        case XK_A: return JNK_KEY_A;
        case XK_b:
        case XK_B: return JNK_KEY_B;
        case XK_c:
        case XK_C: return JNK_KEY_C;
        case XK_d:
        case XK_D: return JNK_KEY_D;
        case XK_e:
        case XK_E: return JNK_KEY_E;
        case XK_f:
        case XK_F: return JNK_KEY_F;
        case XK_g:
        case XK_G: return JNK_KEY_G;
        case XK_h:
        case XK_H: return JNK_KEY_H;
        case XK_i:
        case XK_I: return JNK_KEY_I;
        case XK_j:
        case XK_J: return JNK_KEY_J;
        case XK_k:
        case XK_K: return JNK_KEY_K;
        case XK_l:
        case XK_L: return JNK_KEY_L;
        case XK_m:
        case XK_M: return JNK_KEY_M;
        case XK_n:
        case XK_N: return JNK_KEY_N;
        case XK_o:
        case XK_O: return JNK_KEY_O;
        case XK_p:
        case XK_P: return JNK_KEY_P;
        case XK_q:
        case XK_Q: return JNK_KEY_Q;
        case XK_r:
        case XK_R: return JNK_KEY_R;
        case XK_s:
        case XK_S: return JNK_KEY_S;
        case XK_t:
        case XK_T: return JNK_KEY_T;
        case XK_u:
        case XK_U: return JNK_KEY_U;
        case XK_v:
        case XK_V: return JNK_KEY_V;
        case XK_w:
        case XK_W: return JNK_KEY_W;
        case XK_x:
        case XK_X: return JNK_KEY_X;
        case XK_y:
        case XK_Y: return JNK_KEY_Y;
        case XK_z:
        case XK_Z: return JNK_KEY_Z;

        case XK_1: return JNK_KEY_1;
        case XK_2: return JNK_KEY_2;
        case XK_3: return JNK_KEY_3;
        case XK_4: return JNK_KEY_4;
        case XK_5: return JNK_KEY_5;
        case XK_6: return JNK_KEY_6;
        case XK_7: return JNK_KEY_7;
        case XK_8: return JNK_KEY_8;
        case XK_9: return JNK_KEY_9;
        case XK_0: return JNK_KEY_0;

        // Function Keys
        case XK_F1: return JNK_KEY_F1;
        case XK_F2: return JNK_KEY_F2;
        case XK_F3: return JNK_KEY_F3;
        case XK_F4: return JNK_KEY_F4;
        case XK_F5: return JNK_KEY_F5;
        case XK_F6: return JNK_KEY_F6;
        case XK_F7: return JNK_KEY_F7;
        case XK_F8: return JNK_KEY_F8;
        case XK_F9: return JNK_KEY_F9;
        case XK_F10: return JNK_KEY_F10;
        case XK_F11: return JNK_KEY_F11;
        case XK_F12: return JNK_KEY_F12;

        case XK_Escape: return JNK_KEY_ESCAPE;
        case XK_Return: return JNK_KEY_ENTER;
        case XK_Tab: return JNK_KEY_TAB;
        case XK_BackSpace: return JNK_KEY_BACKSPACE;
        case XK_Insert: return JNK_KEY_INSERT;
        case XK_Delete: return JNK_KEY_DELETE;
        case XK_Home: return JNK_KEY_HOME;
        case XK_End: return JNK_KEY_END;
        case XK_Page_Up: return JNK_KEY_PAGEUP;
        case XK_Page_Down: return JNK_KEY_PAGEDOWN;

        case XK_Left: return JNK_KEY_LEFT;
        case XK_Right: return JNK_KEY_RIGHT;
        case XK_Up: return JNK_KEY_UP;
        case XK_Down: return JNK_KEY_DOWN;

        // Modifier Keys
        case XK_Shift_L: return JNK_KEY_LSHIFT;
        case XK_Shift_R: return JNK_KEY_RSHIFT;
        case XK_Control_L: return JNK_KEY_LCONTROL;
        case XK_Control_R: return JNK_KEY_RCONTROL;
        case XK_Alt_L: return JNK_KEY_LALT;
        case XK_Alt_R: return JNK_KEY_RALT;
        case XK_Super_L: return JNK_KEY_LSUPER;
        case XK_Super_R: return JNK_KEY_RSUPER;
        case XK_Caps_Lock: return JNK_KEY_CAPS;
        case XK_Num_Lock: return JNK_KEY_NUM;

        // Numpad
        case XK_KP_0: return JNK_KEY_KP_0;
        case XK_KP_1: return JNK_KEY_KP_1;
        case XK_KP_2: return JNK_KEY_KP_2;
        case XK_KP_3: return JNK_KEY_KP_3;
        case XK_KP_4: return JNK_KEY_KP_4;
        case XK_KP_5: return JNK_KEY_KP_5;
        case XK_KP_6: return JNK_KEY_KP_6;
        case XK_KP_7: return JNK_KEY_KP_7;
        case XK_KP_8: return JNK_KEY_KP_8;
        case XK_KP_9: return JNK_KEY_KP_9;
        case XK_KP_Decimal: return JNK_KEY_KP_DECIMAL;
        case XK_KP_Enter: return JNK_KEY_KP_ENTER;
        case XK_KP_Add: return JNK_KEY_KP_ADD;
        case XK_KP_Subtract: return JNK_KEY_KP_SUBSTRACT;
        case XK_KP_Multiply: return JNK_KEY_KP_MULTIPLY;
        case XK_KP_Divide: return JNK_KEY_KP_DIVIDE;

        // Symbols
        case XK_space: return JNK_KEY_SPACE;
        case XK_minus: return JNK_KEY_MINUS;
        case XK_equal: return JNK_KEY_EQUAL;
        case XK_bracketleft: return JNK_KEY_LBRACKET;
        case XK_bracketright: return JNK_KEY_RBRACKET;
        case XK_braceleft: return JNK_KEY_LBRACE;
        case XK_braceright: return JNK_KEY_RBRACE;
        case XK_backslash: return JNK_KEY_BACKSLASH;
        case XK_semicolon: return JNK_KEY_SEMICOLON;
        case XK_apostrophe: return JNK_KEY_APOSTROPHE;
        case XK_grave: return JNK_KEY_GRAVE;
        case XK_comma: return JNK_KEY_COMMA;
        case XK_period: return JNK_KEY_PERIOD;
        case XK_slash: return JNK_KEY_SLASH;

        // Misc
        case XK_Print: return JNK_KEY_PRINT_SCREEN;
        case XK_Scroll_Lock: return JNK_KEY_SCROLL_LOCK;
        case XK_Pause: return JNK_KEY_PAUSE;

        default: return JNK_KEY_UNKNOWN;
    }
}

#elif JNK_WINDOWS
#    include <windows.h>

jnk_keys_t keycode_translate(u32 keycode) {
    switch (keycode) {
        // Alphanumeric
        case 'A': return JNK_KEY_A;
        case 'B': return JNK_KEY_B;
        case 'C': return JNK_KEY_C;
        case 'D': return JNK_KEY_D;
        case 'E': return JNK_KEY_E;
        case 'F': return JNK_KEY_F;
        case 'G': return JNK_KEY_G;
        case 'H': return JNK_KEY_H;
        case 'I': return JNK_KEY_I;
        case 'J': return JNK_KEY_J;
        case 'K': return JNK_KEY_K;
        case 'L': return JNK_KEY_L;
        case 'M': return JNK_KEY_M;
        case 'N': return JNK_KEY_N;
        case 'O': return JNK_KEY_O;
        case 'P': return JNK_KEY_P;
        case 'Q': return JNK_KEY_Q;
        case 'R': return JNK_KEY_R;
        case 'S': return JNK_KEY_S;
        case 'T': return JNK_KEY_T;
        case 'U': return JNK_KEY_U;
        case 'V': return JNK_KEY_V;
        case 'W': return JNK_KEY_W;
        case 'X': return JNK_KEY_X;
        case 'Y': return JNK_KEY_Y;
        case 'Z': return JNK_KEY_Z;

        case '1': return JNK_KEY_1;
        case '2': return JNK_KEY_2;
        case '3': return JNK_KEY_3;
        case '4': return JNK_KEY_4;
        case '5': return JNK_KEY_5;
        case '6': return JNK_KEY_6;
        case '7': return JNK_KEY_7;
        case '8': return JNK_KEY_8;
        case '9': return JNK_KEY_9;
        case '0': return JNK_KEY_0;

        // Function Keys
        case VK_F1: return JNK_KEY_F1;
        case VK_F2: return JNK_KEY_F2;
        case VK_F3: return JNK_KEY_F3;
        case VK_F4: return JNK_KEY_F4;
        case VK_F5: return JNK_KEY_F5;
        case VK_F6: return JNK_KEY_F6;
        case VK_F7: return JNK_KEY_F7;
        case VK_F8: return JNK_KEY_F8;
        case VK_F9: return JNK_KEY_F9;
        case VK_F10: return JNK_KEY_F10;
        case VK_F11: return JNK_KEY_F11;
        case VK_F12: return JNK_KEY_F12;

        case VK_ESCAPE: return JNK_KEY_ESCAPE;
        case VK_RETURN: return JNK_KEY_ENTER;
        case VK_TAB: return JNK_KEY_TAB;
        case VK_BACK: return JNK_KEY_BACKSPACE;
        case VK_INSERT: return JNK_KEY_INSERT;
        case VK_DELETE: return JNK_KEY_DELETE;
        case VK_HOME: return JNK_KEY_HOME;
        case VK_END: return JNK_KEY_END;
        case VK_PRIOR: return JNK_KEY_PAGEUP;  // Page Up
        case VK_NEXT: return JNK_KEY_PAGEDOWN; // Page Down

        case VK_LEFT: return JNK_KEY_LEFT;
        case VK_RIGHT: return JNK_KEY_RIGHT;
        case VK_UP: return JNK_KEY_UP;
        case VK_DOWN: return JNK_KEY_DOWN;

        // Modifier Keys
        case VK_LSHIFT: return JNK_KEY_LSHIFT;
        case VK_RSHIFT: return JNK_KEY_RSHIFT;
        case VK_LCONTROL: return JNK_KEY_LCONTROL;
        case VK_RCONTROL: return JNK_KEY_RCONTROL;
        case VK_LMENU: return JNK_KEY_LALT;   // Left Alt
        case VK_RMENU: return JNK_KEY_RALT;   // Right Alt
        case VK_LWIN: return JNK_KEY_LSUPER;  // Left Windows key
        case VK_RWIN: return JNK_KEY_RSUPER;  // Right Windows key
        case VK_CAPITAL: return JNK_KEY_CAPS; // Caps Lock
        case VK_NUMLOCK:
            return JNK_KEY_NUM; // Num Lock

        // Numpad
        case VK_NUMPAD0: return JNK_KEY_KP_0;
        case VK_NUMPAD1: return JNK_KEY_KP_1;
        case VK_NUMPAD2: return JNK_KEY_KP_2;
        case VK_NUMPAD3: return JNK_KEY_KP_3;
        case VK_NUMPAD4: return JNK_KEY_KP_4;
        case VK_NUMPAD5: return JNK_KEY_KP_5;
        case VK_NUMPAD6: return JNK_KEY_KP_6;
        case VK_NUMPAD7: return JNK_KEY_KP_7;
        case VK_NUMPAD8: return JNK_KEY_KP_8;
        case VK_NUMPAD9: return JNK_KEY_KP_9;
        case VK_DECIMAL: return JNK_KEY_KP_DECIMAL;
        case VK_SEPARATOR: return JNK_KEY_KP_ENTER;
        case VK_ADD: return JNK_KEY_KP_ADD;
        case VK_SUBTRACT: return JNK_KEY_KP_SUBSTRACT;
        case VK_MULTIPLY: return JNK_KEY_KP_MULTIPLY;
        case VK_DIVIDE: return JNK_KEY_KP_DIVIDE;

        // Symbols
        case VK_SPACE: return JNK_KEY_SPACE;
        case VK_OEM_MINUS: return JNK_KEY_MINUS;   // - key
        case VK_OEM_PLUS: return JNK_KEY_EQUAL;    // = key
        case VK_OEM_4: return JNK_KEY_LBRACKET;    // [ key
        case VK_OEM_6: return JNK_KEY_RBRACKET;    // ] key
        case VK_OEM_1: return JNK_KEY_SEMICOLON;   // ; key
        case VK_OEM_7: return JNK_KEY_APOSTROPHE;  // ' key
        case VK_OEM_3: return JNK_KEY_GRAVE;       // ` key
        case VK_OEM_COMMA: return JNK_KEY_COMMA;   // , key
        case VK_OEM_PERIOD: return JNK_KEY_PERIOD; // . key
        case VK_OEM_2: return JNK_KEY_SLASH;       // / key
        case VK_OEM_5: return JNK_KEY_BACKSLASH;   // \ key
        case VK_OEM_102: return JNK_KEY_BACKSLASH;

        // For { and } - these are typically Shift+[ and Shift+]
        // You might need to handle these with shift state

        // Misc
        case VK_SNAPSHOT: return JNK_KEY_PRINT_SCREEN; // Print Screen
        case VK_SCROLL: return JNK_KEY_SCROLL_LOCK;    // Scroll Lock
        case VK_PAUSE: return JNK_KEY_PAUSE;           // Pause/Break

        default: return JNK_KEY_UNKNOWN;
    }
}

#endif
