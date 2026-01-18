#include "INPUT.H"

// BIOS Data Area (BDA) addresses for 32-bit protected mode
volatile unsigned char keyboardTable[256];          // Stores the current keys status
volatile unsigned char prevKeyboardTable[256];      // Stores the previous keys status

const unsigned char *keyboardMap[256] = {
    NULL,
    "KEY_ESC",                  // 1
    "KEY_1",                    // 2
    "KEY_2",                    // 3
    "KEY_3",                    // 4
    "KEY_4",                    // 5
    "KEY_5",                    // 6
    "KEY_6",                    // 7
    "KEY_7",                    // 8
    "KEY_8",                    // 9
    "KEY_9",                    // 10
    "KEY_0",                    // 11
    "KEY_MINUS",                // 12
    "KEY_EQUALS",               // 13
    "KEY_BACKSPACE",            // 14
    "KEY_TAB",                  // 15
    "KEY_Q",                    // 16
    "KEY_W",                    // 17
    "KEY_E",                    // 18
    "KEY_R",                    // 19
    "KEY_T",                    // 20
    "KEY_Y",                    // 21
    "KEY_U",                    // 22
    "KEY_I",                    // 23
    "KEY_O",                    // 24
    "KEY_P",                    // 25
    "KEY_LBRACKET",             // 26
    "KEY_RBRACKET",             // 27
    "KEY_ENTER",                // 28
    "KEY_LCTRL",                // 29
    "KEY_A",                    // 30
    "KEY_S",                    // 31
    "KEY_D",                    // 32
    "KEY_F",                    // 33
    "KEY_G",                    // 34
    "KEY_H",                    // 35
    "KEY_J",                    // 36
    "KEY_K",                    // 37
    "KEY_L",                    // 38
    "KEY_SEMICOLON",            // 39
    "KEY_APOSTROPHE",           // 40
    "KEY_GRAVE",                // 41
    "KEY_LSHIFT",               // 42
    "KEY_BACKSLASH",            // 43
    "KEY_Z",                    // 44
    "KEY_X",                    // 45
    "KEY_C",                    // 46
    "KEY_V",                    // 47
    "KEY_B",                    // 48
    "KEY_N",                    // 49
    "KEY_M",                    // 50
    "KEY_COMMA",                // 51
    "KEY_PERIOD",               // 52
    "KEY_SLASH",                // 53
    "KEY_RSHIFT",               // 54
    "KEY_KP_MULTIPLY",          // 55
    "KEY_LALT",                 // 56
    "KEY_SPACE",                // 57
    "KEY_CAPSLOCK",             // 58
    "KEY_F1",                   // 59
    "KEY_F2",                   // 60
    "KEY_F3",                   // 61
    "KEY_F4",                   // 62
    "KEY_F5",                   // 63
    "KEY_F6",                   // 64
    "KEY_F7",                   // 65
    "KEY_F8",                   // 66
    "KEY_F9",                   // 67
    "KEY_F10",                  // 68
    "KEY_NUMLOCK",              // 69
    "KEY_SCROLLLOCK",           // 70
    "KEY_KP_7",                 // 71
    "KEY_KP_8",                 // 72
    "KEY_KP_9",                 // 73
    "KEY_KP_MINUS",             // 74
    "KEY_KP_4",                 // 75
    "KEY_KP_5",                 // 76
    "KEY_KP_6",                 // 77
    "KEY_KP_PLUS",              // 78
    "KEY_KP_1",                 // 79
    "KEY_KP_2",                 // 80
    "KEY_KP_3",                 // 81
    "KEY_KP_0",                 // 82
    "KEY_KP_PERIOD",            // 83
    "KEY_F11",                  // 87
    "KEY_F12",                  // 88
    "KEY_KP_ENTER",             // 28
    "KEY_RCTRL",                // 29
    "KEY_KP_DIVIDE",            // 53
    "KEY_RALT",                 // 56
    "KEY_HOME",                 // 71 - 224 - 211
    "KEY_UP",                   // 72 - 224 - 211
    "KEY_PAGEUP",               // 73 - 224 - 211
    "KEY_LEFT",                 // 75 - 224 - 211
    "KEY_RIGHT",                // 77 - 224 - 211
    "KEY_END",                  // 79 - 224 - 211
    "KEY_DOWN",                 // 80 - 224 - 211
    "KEY_PAGEDOWN",             // 81 - 224 - 211
    "KEY_INSERT",               // 82 - 224 - 211
    "KEY_DELETE",               // 83 - 224 - 211
    NULL
};

// Variables managed by ISR
unsigned char scanCode;

void ( __interrupt __far *oldKeyISR)();

/*
HOW DOES THIS WORK?

Basically, is that we are manipulating the BIOS's interrupt vector by replacing my own ISR from 
the original one, we save the old one then restore after the program ends,
 but also we chain my ISR so it does its job and then pass the turn to the old one to also do its work 
 which is used by kbhit and getch... 

 Keyboard → PIC → CPU
           ↓
    Triggers INT 09h
           ↓
    Jumps to my ISR (keyISR)
           ↓
    My code: scanCode = inPortb(0x60);
               keyboardTable[scanCode] = 1;
           ↓
    I call: oldKeyISR();  // Chain to BIOS
           ↓
    BIOS ISR runs:
      - Converts scan code to ASCII
      - Updates BIOS keyboard buffer
      - Handles Caps/Num Lock LEDs
      - Sends EOI to PIC
           ↓
    Returns to my ISR → Returns to interrupted program
    
*/
static void __interrupt __far keyISR() {
    unsigned char status;
    
    // Read scan code
    scanCode = inPortb(0x60);
    // Update table
    if (scanCode < 128) {
        keyboardTable[scanCode] = 1;        
    } else {
        keyboardTable[scanCode - 128] = 0;
    }

    // Acknowledge the PIC (Programmable Interrupt Controller)
    //outPortb(0x20, 0x20); // This is not chaining the interrupts
    oldKeyISR();            // This is chaining the interrupts
}

void initKeyboard() {
    int i;
    for (i = 0; i < 256; i++) keyboardTable[i] = 0;
    oldKeyISR = _dos_getvect(0x09);
    _dos_setvect(0x09, keyISR);
}

void closeKeyboard() {
    _dos_setvect(0x09, oldKeyISR);
}

#ifdef STANDALONE  
// Standalone test for input
int main(){
    int i=0;
    printf("Input System Test (ISR)\n");
    printf("Press ESC to exit.\n\n");

    initKeyboard();

    while(keyboardTable[KEY_ESC] == false) {
        for(i = 0; i < 256; i++) {
            // Only act if the state CHANGED since the last check
            if(keyboardTable[i] != prevKeyboardTable[i]) {
                if (keyboardMap[i] != NULL) {
                    if (keyboardTable[i] == true) {
                        printf("Key %s: PRESSED\n", keyboardMap[i]);
                    } else {
                        printf("Key %s: RELEASED\n", keyboardMap[i]);
                    }
                }
                prevKeyboardTable[i] = keyboardTable[i]; // Update the tracker
            }
        }
    }
    closeKeyboard();
    return 0;
}

#endif