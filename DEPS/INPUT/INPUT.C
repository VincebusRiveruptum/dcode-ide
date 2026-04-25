#include "INPUT.H"
/*
    Ver 1.2 , 
        - Multi-key press detection with a single function
        - inp_isKeyDown, inp_isKeyPressed, inp_isKeyReleased
        - Robust ISR with E0 handling and latched states
*/
volatile unsigned char inp_state[256];          /* Current physical status */
volatile unsigned char inp_pressed[256];        /* Event counter: just pressed */
volatile unsigned char inp_released[256];       /* Event counter: just released */
volatile unsigned char inp_justPressed[256];    /* Frame-specific pressed flag */

const unsigned char *inp_keyboardMap[256] = {
    NULL,
    "KEY_ESC",                  /* 1 */
    "KEY_1",                    /* 2 */
    "KEY_2",                    /* 3 */
    "KEY_3",                    /* 4 */
    "KEY_4",                    /* 5 */
    "KEY_5",                    /* 6 */
    "KEY_6",                    /* 7 */
    "KEY_7",                    /* 8 */
    "KEY_8",                    /* 9 */
    "KEY_9",                    /* 10 */
    "KEY_0",                    /* 11 */
    "KEY_MINUS",                /* 12 */
    "KEY_EQUALS",               /* 13 */
    "KEY_BACKSPACE",            /* 14 */
    "KEY_TAB",                  /* 15 */
    "KEY_Q",                    /* 16 */
    "KEY_W",                    /* 17 */
    "KEY_E",                    /* 18 */
    "KEY_R",                    /* 19 */
    "KEY_T",                    /* 20 */
    "KEY_Y",                    /* 21 */
    "KEY_U",                    /* 22 */
    "KEY_I",                    /* 23 */
    "KEY_O",                    /* 24 */
    "KEY_P",                    /* 25 */
    "KEY_LBRACKET",             /* 26 */
    "KEY_RBRACKET",             /* 27 */
    "KEY_ENTER",                /* 28 */
    "KEY_LCTRL",                /* 29 */
    "KEY_A",                    /* 30 */
    "KEY_S",                    /* 31 */
    "KEY_D",                    /* 32 */
    "KEY_F",                    /* 33 */
    "KEY_G",                    /* 34 */
    "KEY_H",                    /* 35 */
    "KEY_J",                    /* 36 */
    "KEY_K",                    /* 37 */
    "KEY_L",                    /* 38 */
    "KEY_SEMICOLON",            /* 39 */
    "KEY_APOSTROPHE",           /* 40 */
    "KEY_GRAVE",                /* 41 */
    "KEY_LSHIFT",               /* 42 */
    "KEY_BACKSLASH",            /* 43 */
    "KEY_Z",                    /* 44 */
    "KEY_X",                    /* 45 */
    "KEY_C",                    /* 46 */
    "KEY_V",                    /* 47 */
    "KEY_B",                    /* 48 */
    "KEY_N",                    /* 49 */
    "KEY_M",                    /* 50 */
    "KEY_COMMA",                /* 51 */
    "KEY_PERIOD",               /* 52 */
    "KEY_SLASH",                /* 53 */
    "KEY_RSHIFT",               /* 54 */
    "KEY_KP_MULTIPLY",          /* 55 */
    "KEY_LALT",                 /* 56 */
    "KEY_SPACE",                /* 57 */
    "KEY_CAPSLOCK",             /* 58 */
    "KEY_F1",                   /* 59 */
    "KEY_F2",                   /* 60 */
    "KEY_F3",                   /* 61 */
    "KEY_F4",                   /* 62 */
    "KEY_F5",                   /* 63 */
    "KEY_F6",                   /* 64 */
    "KEY_F7",                   /* 65 */
    "KEY_F8",                   /* 66 */
    "KEY_F9",                   /* 67 */
    "KEY_F10",                  /* 68 */
    "KEY_NUMLOCK",              /* 69 */
    "KEY_SCROLLLOCK",           /* 70 */
    "KEY_KP_7",                 /* 71 */
    "KEY_KP_8",                 /* 72 */
    "KEY_KP_9",                 /* 73 */
    "KEY_KP_MINUS",             /* 74 */
    "KEY_KP_4",                 /* 75 */
    "KEY_KP_5",                 /* 76 */
    "KEY_KP_6",                 /* 77 */
    "KEY_KP_PLUS",              /* 78 */
    "KEY_KP_1",                 /* 79 */
    "KEY_KP_2",                 /* 80 */
    "KEY_KP_3",                 /* 81 */
    "KEY_KP_0",                 /* 82 */
    "KEY_KP_PERIOD",            /* 83 */
    "KEY_F11",                  /* 87 */
    "KEY_F12",                  /* 88 */
    "KEY_KP_ENTER",             /* 28 */
    "KEY_RCTRL",                /* 29 */
    "KEY_KP_DIVIDE",            /* 53 */
    "KEY_RALT",                 /* 56 */
    "KEY_HOME",                 /* 71 */
    "KEY_UP",                   /* 72 */
    "KEY_PAGEUP",               /* 73 */
    "KEY_LEFT",                 /* 75 */
    "KEY_RIGHT",                /* 77 */
    "KEY_END",                  /* 79 */
    "KEY_DOWN",                 /* 80 */
    "KEY_PAGEDOWN",             /* 81 */
    "KEY_INSERT",               /* 82 */
    "KEY_DELETE",               /* 83 */
    NULL
};

/* Variables managed by ISR */
unsigned char inp_scanCode;
static void ( __interrupt __far *oldKeyISR)();
static unsigned char extended = 0;

static void __interrupt __far keyISR() {
    unsigned char sc = inPortb(0x60);
    inp_scanCode = sc;

    if (sc == 0xE0) {
        extended = 1;
        oldKeyISR();
        return;
    }

    if (sc < 128) {
        /* Make code */
        if (!inp_state[sc]) {
            if (inp_pressed[sc] < 255) inp_pressed[sc]++;
            inp_justPressed[sc] = 1;
        }
        inp_state[sc] = 1;
    } else {
        /* Break code */
        unsigned char k = sc - 128;
        inp_state[k] = 0;
        if (inp_released[k] < 255) inp_released[k]++;
    }

    extended = 0;
    
    /* Consume Delete key to avoid BIOS interference/beeping */
    if (sc == KEY_DELETE || sc == (KEY_DELETE | 0x80)) {
        outPortb(0x20, 0x20); /* Acknowledge PIC */
        return;
    }

    oldKeyISR();
}

void inp_initKeyboard() {
    int i;
    for (i = 0; i < 256; i++) {
        inp_state[i] = 0;
        inp_pressed[i] = 0;
        inp_released[i] = 0;
    }
    oldKeyISR = _dos_getvect(0x09);
    _dos_setvect(0x09, keyISR);
}

void inp_closeKeyboard() {
    _dos_setvect(0x09, oldKeyISR);
}

void inp_updateKeyboard() {
    /* With the counter approach, we don't clear pressed/released here 
       because they are consumed by the isKeyPressed/Released functions.
       However, we still want to clear them if they aren't being used 
       to avoid "ghost" events later. */
    int i;
    _disable();
    for(i = 0; i < 256; i++) {
        /* Only clear keys that aren't usually used as shortcuts 
           or if the queue is getting too long. */
        if (inp_pressed[i] > 10) inp_pressed[i] = 0;
        inp_released[i] = 0; 
        inp_justPressed[i] = 0;
    }
    _enable();
}

bool inp_isKeyDown(unsigned char key) {
    return inp_state[key];
}

bool inp_isKeyPressed(unsigned char key) {
    if (inp_pressed[key] > 0) {
        _disable();
        inp_pressed[key]--;
        _enable();
        return true;
    }
    return false;
}

bool inp_isKeyReleased(unsigned char key) {
    if (inp_released[key] > 0) {
        _disable();
        inp_released[key]--;
        _enable();
        return true;
    }
    return false;
}

unsigned char inp_keysPressed(int triggerType, int nKeys, ...) {
    int i;
    int allDown = 1;
    int anyJustPressed = 0;
    unsigned char key;
    va_list args;
    
    va_start(args, nKeys);
    
    for(i = 0; i < nKeys; i++) {
        key = (unsigned char)va_arg(args, int);
        
        /* For the combo to be active, all keys must be PHYSICALLY down 
           OR have been just pressed this frame. */
        if(inp_state[key] == false && inp_justPressed[key] == false) {
            allDown = 0;
        }
        
        if(inp_justPressed[key] == true) {
            anyJustPressed = 1;
        }
    }
    
    va_end(args);

    if (triggerType == INP_TRIGGER_LEVEL) {
        return (unsigned char)allDown;
    }
    
    /* EDGE behavior: requires all down AND at least one just pressed this frame */
    return (unsigned char)(allDown && anyJustPressed);
}

unsigned char inp_keysDown(int nKeys, ...) {
    int i;
    unsigned char key;
    va_list args;
    
    va_start(args, nKeys);
    
    for(i = 0; i < nKeys; i++) {
        key = (unsigned char)va_arg(args, int);
        if(inp_state[key] == false) {
            va_end(args);
            return 0;
        }
    }
    
    va_end(args);
    return 1;
}


#ifdef STANDALONE  
int main(){
    printf("Input System Test (ISR)\n");
    printf("Press ESC to exit.\n\n");

    inp_initKeyboard();

    while(inp_state[KEY_ESC] == false) {
        if (inp_keysPressed(INP_TRIGGER_EDGE, 2, KEY_N, KEY_LCTRL)) {
            printf("CTRL + N combo detected once!\n");
        }
        
        if (inp_isKeyPressed(KEY_DELETE)) {
            printf("DELETE pressed!\n");
        }
        
        inp_updateKeyboard();
    }
    
    inp_closeKeyboard();
    return 0;
}
#endif