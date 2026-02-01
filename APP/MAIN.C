/*
    DCODE Engine by Vincebus Riveruptum

    (C) 2026 Vincebus Riveruptum

    This library is for educational or entertainment purposes only.
    It is not intended for use in production environments.
*/

#include "MAIN.H"

int main(int argc, char *argv[]){
    bool endProgram = false;
    char c;
    int ticks = 0;
    
    // So, the steps are:
    // 1. Set VGA mode 2
    log_init();
    mem_init();
    v_init_video();
    initKeyboard();
    t_initTests();

    handleArguments(argc, argv);

    
    while(endProgram == false){

        // ACTION KEYS HANDLING
        // This is uses ISR approach, not getch()
        if(isKeyDown(KEY_ESC)) endProgram = true;
        if(isKeyDown(KEY_F1)) v_set25Lines();
        if(isKeyDown(KEY_F2)) v_set50Lines();
        if(isKeyDown(KEY_F3)) v_set43Lines();
        
        if(keysPressed(3, KEY_LCTRL, KEY_LSHIFT, KEY_S)) ed_triggerSave();
        if(isKeyDown(KEY_SPACE)) ed_renderEvent = true;
        
        // NEW FILE
        if(keysPressed(2, KEY_LCTRL, KEY_N)){
            logger("[main]: User created new file.");
        }
        
        // CLOSE FILE (Alt+F4)
        if(keysPressed(2, KEY_LALT, KEY_F4)){
            logger("[main]: User closed file.");
        }
        
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
                        //dw_writeBuffer(textmemptr, "Hello World %d", 5, 6, 20, 10, COLOR_WHITE, COLOR_BLACK, ticks);
                        
                        ticks++;
                    }
                }
            }
        // Cursor coordinates for testing
        //dw_writeBuffer(textmemptr, "X: %d Y: %d", 0, 0, 10, 0, COLOR_WHITE, COLOR_BLACK, currentCursorX, currentCursorY);
        ed_updateCursor();

        if(ed_renderEvent == true){
            ed_renderElements();
            ed_renderEvent = false;
        }

        updateKeyboard();
    }

    
    closeKeyboard();
    v_set25Lines();
    dw_cls(textmemptr);
    mem_shutdown();
    log_shutdown();
    printf("96 Tears...\n");
    return 0;
}
