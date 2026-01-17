/*
    This is a basic TUI library, right now just for testing purposes
    By Vincebus Riveruptum
    2026
*/

#include "TUI.H"

#ifdef TUI_STANDALONE

int main(){
    printf("\nTUI Standalone Test.\nVersion 0.1 - Vincebus Riveruptum, 2026");

    // So, the steps are:
    // 1. Set VGA mode 2
    // 2. Fill screen with color 
    // 3. Write something on screen with bg color, foreground, blinking
    while(keyboardTable[KEY_ESC] != true){

    }

    printf("User pressed ESC.");
    return 0;
}

#endif