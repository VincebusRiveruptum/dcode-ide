#include "INPUT.H"

volatile unsigned char inp_state[256];          /* Current physical status */
volatile unsigned char inp_pressed[256];        /* Event counter: just pressed */
volatile unsigned char inp_released[256];       /* Event counter: just released */
volatile unsigned char inp_justPressed[256];    /* Frame-specific pressed flag */
volatile unsigned char inp_justPressedPending[256]; /* Latch for asynchronous key presses */

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
            inp_justPressedPending[sc] = 1;
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
    if (sc == HAL_KEY_DELETE || sc == (HAL_KEY_DELETE | 0x80)) {
        outPortb(0x20, 0x20); /* Acknowledge PIC */
        return;
    }

    oldKeyISR();
}

void hal_inp_clearKeyboardBuffer(void) {
    int i;
    _disable();
    for (i = 0; i < 256; i++) {
        inp_pressed[i] = 0;
        inp_released[i] = 0;
        inp_justPressed[i] = 0;
        inp_justPressedPending[i] = 0;
    }
    _enable();
    
    /* Drain BIOS keyboard buffer */
    while (kbhit()) {
        getch();
    }
}

void hal_inp_waitForRelease(void){
    char pressed = 0;
    int i=0;

    do{
        pressed = 0;
        _disable();
        for (i = 0; i < 256; i++) {
            pressed = inp_state[i];
            if(pressed) break;
        }
        _enable();
    }while(pressed);

    hal_inp_clearKeyboardBuffer();
}

void hal_inp_initKeyboard(void) {
    int i;
    for (i = 0; i < 256; i++) {
        inp_state[i] = 0;
        inp_pressed[i] = 0;
        inp_released[i] = 0;
        inp_justPressed[i] = 0;
        inp_justPressedPending[i] = 0;
    }
    oldKeyISR = _dos_getvect(0x09);
    _dos_setvect(0x09, keyISR);
}

void hal_inp_closeKeyboard(void) {
    _dos_setvect(0x09, oldKeyISR);
}

void hal_inp_updateKeyboard(void) {
    int i;
    _disable();
    for(i = 0; i < 256; i++) {
        /* Latch pending presses to be consumed during the next frame */
        inp_justPressed[i] = inp_justPressedPending[i];
        inp_justPressedPending[i] = 0;

        /* Only clear keys that aren't usually used as shortcuts */
        if (inp_pressed[i] > 10) inp_pressed[i] = 0;
        inp_released[i] = 0; 
    }
    _enable();
}

bool hal_inp_isKeyDown(unsigned char key) {
    return inp_state[key];
}

bool hal_inp_isKeyPressed(unsigned char key) {
    if (inp_pressed[key] > 0) {
        _disable();
        inp_pressed[key]--;
        _enable();
        return true;
    }
    return false;
}

bool hal_inp_isKeyReleased(unsigned char key) {
    if (inp_released[key] > 0) {
        _disable();
        inp_released[key]--;
        _enable();
        return true;
    }
    return false;
}

unsigned char hal_inp_keysPressed(int triggerType, int nKeys, ...) {
    int i;
    int allDown = 1;
    int anyJustPressed = 0;
    unsigned char key;
    va_list args;
    
    va_start(args, nKeys);
    
    for(i = 0; i < nKeys; i++) {
        key = (unsigned char)va_arg(args, int);
        
        if(inp_state[key] == false && inp_justPressed[key] == false) {
            allDown = 0;
        }
        
        if(inp_justPressed[key] == true) {
            anyJustPressed = 1;
        }
    }
    
    va_end(args);

    if (triggerType == HAL_INP_TRIGGER_LEVEL) {
        return (unsigned char)allDown;
    }
    
    return (unsigned char)(allDown && anyJustPressed);
}

unsigned char hal_inp_keysDown(int nKeys, ...) {
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
