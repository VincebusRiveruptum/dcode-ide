/*
    DCODE Engine by Vincebus Riveruptum

    (C) 2026 Vincebus Riveruptum

    This library is for educational or entertainment purposes only.
    It is not intended for use in production environments.
    So, there are still some issues, unfortunatelly
*/

#include "MAIN.H"

int main(int argc, char *argv[]){
    char c;
    int ticks = 0;
    
    endProgram = false;
    // So, the steps are:
    // 1. Set VGA mode 2
    log_init();
    mem_init();
    v_init_video();
    initKeyboard();
    
    logger("[main]: %d %s", argc, argv[1]);
    ed_initConfig(argc, argv);
    t_initTests();
    
    ed_renderEvent = true;
    
    while(endProgram == false){

        // ACTION KEYS HANDLING
        // This is uses ISR approach, not getch()
        if(isKeyDown(KEY_ESC)) f_triggerClose();
        if(isKeyDown(KEY_F1)) {v_set25Lines(); ed_renderEvent = true;}
        if(isKeyDown(KEY_F2)) {v_set50Lines(); ed_renderEvent = true;}
        if(isKeyDown(KEY_F3)) {v_set43Lines(); ed_renderEvent = true;}
        
        if(keysPressed(3, KEY_LCTRL, KEY_LSHIFT, KEY_S)) f_saveFile();
        if(isKeyDown(KEY_SPACE)) ed_renderEvent = true;
        
        if(isKeyReleased(KEY_DELETE)) ed_supr();
        
        
        // NEW FILE
        if(keysPressed(2, KEY_LCTRL, KEY_N)){
            f_newFile();
            logger("[main]: User created %s ,a new file.", currentFileArena->file->name);
        }
        

        // Hice mi propio editor, ke wea!!
        // CLOSE FILE (Alt+F4)
        if(keysPressed(2, KEY_LALT, KEY_F4)){
            logger("[main]: User closed file.");
            f_triggerClose();
        }
        
        // Getch approach, why? Because getch() reads and uses DOS routines for handling the keyboard
        // so it translates the input scancode to the correct codepage value.
        if(kbhit()){
            c = getch();
            
            if(c == 0 || (unsigned char)c == 0xE0){
                // CURSOR ARROW HANDLING
                c = getch(); /* Consume extended byte and arrows */

                /*
                    If the cursor is currently inside the text area, we are in TEXT MODE
                    if the cursor sits outside means we are in GUI MODE
                */
                
                if(c == KEY_UP) ed_moveCursor(0, -1);
                if(c == KEY_DOWN) ed_moveCursor(0, 1);
                if(c == KEY_LEFT) ed_moveCursor(-1, 0);
                if(c == KEY_RIGHT) ed_moveCursor(1, 0);
                
            } else {
                /* Ignore action keys based on ASCII values */
                // TYPING
         

                if(c == CHAR_BACKSPACE){
                    ed_backspace();
                }

                if(c == CHAR_ENTER){                    
                    ed_newLine();
                }

                if(!(c == CHAR_ESCAPE ||
                    c == CHAR_BACKSPACE ||
                    c == CHAR_TAB ||
                    c == CHAR_ENTER ||
                    c == CHAR_DELETE)){
                        
                        //dw_char(textmemptr, c);
                        //el_renderElements();
                        // Tick counting by user activity, not globally
                        //dw_writeBuffer(textmemptr, "Hello World %d", 5, 6, 20, 10, COLOR_WHITE, COLOR_BLACK, ticks);
                        
                        ed_typeChar(c);

                        ticks++;
                    }
                }
            }
        // Cursor coordinates for testing

        // This is kinda a statusbar
        // The line and column are relative and assuming that the text area is full screen, in the future they will be resizable so
        // I have to store and calculate relative by size and position
        //dw_writeBuffer(textmemptr, "Modified : %d", 0, VIDEO_ROWS - 1, 60, VIDEO_ROWS - 1, COLOR_BLACK, COLOR_LIGHT_GRAY, currentFileArena->file->isModified);
        dw_writeBuffer(textmemptr, "Line %d, Col %d %c", 0, VIDEO_ROWS - 1, 39, VIDEO_ROWS - 1, COLOR_BLACK, COLOR_LIGHT_GRAY, currentFileArena->file->scrollY + currentCursorY + 1, currentCursorX - LINE_COUNTER_WIDTH + 1, 179);
        dw_writeBuffer(textmemptr, " %s", 40, VIDEO_ROWS - 1, VIDEO_COLS, VIDEO_ROWS - 1, COLOR_BLACK, COLOR_LIGHT_GRAY, currentFileArena->file->name);
        ed_updateCursor();

        if(ed_renderEvent == true){
            ed_renderElements();
            ed_renderEvent = false;
        }

        updateKeyboard();
    }

    
    closeKeyboard();
    dw_cls(textmemptr);
    v_set25Lines();
    mem_shutdown();
    log_shutdown();
    printf("96 Tears...\n");
    return 0;
}


