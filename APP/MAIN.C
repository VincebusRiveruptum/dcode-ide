/*
    DCODE Engine by Vincebus Riveruptum

    (C) 2026 Vincebus Riveruptum

    This library is for educational or entertainment purposes only.
    It is not intended for use in production environments.
*/

#include "MAIN.H"

int main(){
    bool endProgram = false;
    char c;
    int ticks = 0;
    
    // So, the steps are:
    // 1. Set VGA mode 2
    // 2. Fill screen with color 
    // 3. Write something on screen with bg color, foreground, blinking
    
    // This goes this way:
    // 1. Keyboard ISR listens keyboard strokes
    // 2. If all strokes are not ESC, then we go to INSERT mode
    // 3. After inserting a single character we go back to COMMAND mode
    // 4. And so on...
    log_init();
    mem_init();
    v_init_video();
    initKeyboard();
    t_initTests();

    
    while(endProgram == false){
        // ACTION KEYS HANDLING
        // This is uses ISR approach, not getch()
        if(keyboardTable[KEY_ESC] == true) endProgram = true;

        if(keyboardTable[KEY_F1] == true) v_set25Lines();
        if(keyboardTable[KEY_F2] == true) v_set50Lines();
        if(keyboardTable[KEY_F3] == true) v_set43Lines();
        if(keyboardTable[KEY_LCTRL] == true && keyboardTable[KEY_LSHIFT] == true && keyboardTable[KEY_S] == true) ed_triggerSave(); 
        if(keyboardTable[KEY_SPACE] == true) ed_renderEvent = true;
        
        // Getch approach, why? Because getch() reads and uses DOS routines for handling the keyboard
        // so it translates the input scancode to the correct codepage value.
        if(kbhit()){
            c = getch();
            
            if(c == 0 || (unsigned char)c == 0xE0){
                // CURSOR ARROW HANDLING
                c = getch(); /* Consume extended byte and arrows */
                if(c == KEY_UP) ed_moveCursor(0, -1);
                if(c == KEY_DOWN) ed_moveCursor(0, 1);
                if(c == KEY_LEFT) ed_moveCursor(-1, 0);
                if(c == KEY_RIGHT) ed_moveCursor(1, 0);
            } else {
                /* Ignore action keys based on ASCII values */
                // TYPING

                if(c == CHAR_BACKSPACE){
                    ed_moveCursor(-1, 0);
                    dw_char(textmemptr, ' ');
                    ed_moveCursor(-1, 0);
                }

                if(c == CHAR_ENTER){
                    currentCursorX = 0;
                    if(currentCursorY < VIDEO_ROWS - 1){
                        currentCursorY++;
                        ed_putCursor(currentCursorX, currentCursorY);
                    }
                }

                if(!(c == CHAR_ESCAPE ||
                    c == CHAR_BACKSPACE ||
                    c == CHAR_TAB ||
                    c == CHAR_ENTER ||
                    c == CHAR_DELETE)){
                        
                        dw_char(textmemptr, c);
                        //el_renderElements();
                        // Tick counting by user activity, not globally
                        dw_writeBuffer(textmemptr, "Hello World %d", 5, 6, 20, 10, COLOR_WHITE, COLOR_BLACK, ticks);
                        
                        ticks++;
                    }
                }
            }
        // Cursor coordinates for testing
        dw_writeBuffer(textmemptr, "X: %d Y: %d", 0, 0, 10, 0, COLOR_WHITE, COLOR_BLACK, currentCursorX, currentCursorY);
        ed_updateCursor();

        if(ed_renderEvent == true){
            ed_renderElements();
            ed_renderEvent = false;
        }
    }
    
    closeKeyboard();
    v_set25Lines();
    dw_cls(textmemptr);
    mem_shutdown();
    log_shutdown();
    printf("96 Tears...\n");
    return 0;
}
