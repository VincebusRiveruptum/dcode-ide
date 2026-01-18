/*
    This is a basic TUI library, right now just for testing purposes
    By Vincebus Riveruptum
    2026
*/

#include "TUI.H"

#ifdef TUI_STANDALONE

unsigned char tuiMode = TUI_MODE_COMMAND;

int main(){
    bool endProgram = false;
    char c;
    printf("\nTUI Standalone Test.\nVersion 0.1 - Vincebus Riveruptum, 2026");
    
    // So, the steps are:
    // 1. Set VGA mode 2
    // 2. Fill screen with color 
    // 3. Write something on screen with bg color, foreground, blinking
    
    // This goes this way:
    // 1. Keyboard ISR listens keyboard strokes
    // 2. If all strokes are not ESC, then we go to INSERT mode
    // 3. After inserting a single character we go back to COMMAND mode
    // 4. And so on...
    
    initKeyboard();
    
    while(endProgram == false){
        if(keyboardTable[KEY_ESC] == true) endProgram = true;

        if(kbhit()){
            c = getch();
            putch(c);
        }
    }
    
    closeKeyboard();
    printf("\nUser pressed ESC.");
    return 0;
}

    #endif