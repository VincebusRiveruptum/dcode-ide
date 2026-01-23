/*
    This is a basic TUI library, right now just for testing purposes
    By Vincebus Riveruptum
    2026

    Credits osdever.net

    http://www.osdever.net/bkerndev/Docs/printing.htm
*/

#include "TUI.H"

unsigned short *textmemptr;
int attrib = 0x0F;

struct Container *root;

unsigned char TUI_COLS = 80;
unsigned char TUI_ROWS = 25;

void tg_updateCursor(){
    tg_putCursor(currentCursorX, currentCursorY);
}

void tg_putCursor(unsigned char x, unsigned char y){
    unsigned short temp;

    currentCursorX = x;
    currentCursorY = y;
    /* The equation for finding the index in a linear
    *  chunk of memory can be represented by:
    *  Index = [(y * width) + x] */
    temp = currentCursorY * TUI_COLS + currentCursorX;

    /* This sends a command to indicies 14 and 15 in the
    *  CRT Control Register of the VGA controller. These
    *  are the high and low bytes of the index that show
    *  where the hardware cursor is to be 'blinking'. To
    *  learn more, you should look up some VGA specific
    *  programming documents. A great start to graphics:
    *  http://www.brackeen.com/home/vga */
    outPortb(0x3D4, 14);
    outPortb(0x3D5, temp >> 8);
    outPortb(0x3D4, 15);
    outPortb(0x3D5, temp);
}

void tg_moveCursor(short x, short y){
    if(currentCursorX + x >= TUI_COLS || currentCursorY + y >= TUI_ROWS){
        return;
    }

    if(currentCursorX + x < 0 || currentCursorY + y < 0){
        return;
    }

    currentCursorX += x;
    currentCursorY += y;

    tg_putCursor(currentCursorX, currentCursorY);
}



void tg_init_video(){
    TUI_ROWS = 25;
    TUI_COLS = 80;

    tg_set25Lines_asm();

    textmemptr = (unsigned short *)0xB8000;
    tg_cls();
}

void tg_set43Lines(){
    TUI_ROWS = 43;
    TUI_COLS = 80;

    tg_set350Lines_asm();
    tg_set43Lines_asm();
}

void tg_set50Lines(){
    TUI_ROWS = 50;
    TUI_COLS = 80;

    tg_set400Lines_asm();
    tg_set43Lines_asm();
}

void tg_set25Lines(){
    TUI_ROWS = 25;
    TUI_COLS = 80;
    tg_set400Lines_asm();
    tg_set25Lines_asm();
}

/* Standalone test =================================================*/

#ifdef TUI_STANDALONE

int main(){
    bool endProgram = false;
    char c;
    int ticks = 0;
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
    
    tg_init_video();
    initKeyboard();
    tg_t_initTests();

    while(endProgram == false){
        // ACTION KEYS HANDLING
        // This is uses ISR approach, not getch()
        if(keyboardTable[KEY_ESC] == true) endProgram = true;

        if(keyboardTable[KEY_F1] == true) tg_set25Lines();
        if(keyboardTable[KEY_F2] == true) tg_set50Lines();
        if(keyboardTable[KEY_F3] == true) tg_set43Lines();
        
        // Getch approach, why? Because getch() reads and uses DOS routines for handling the keyboard
        // so it translates the input scancode to the correct codepage value.
        if(kbhit()){
            c = getch();
            
            if(c == 0 || (unsigned char)c == 0xE0){
                // CURSOR ARROW HANDLING
                c = getch(); /* Consume extended byte and arrows */
                if(c == KEY_UP) tg_moveCursor(0, -1);
                if(c == KEY_DOWN) tg_moveCursor(0, 1);
                if(c == KEY_LEFT) tg_moveCursor(-1, 0);
                if(c == KEY_RIGHT) tg_moveCursor(1, 0);
            } else {
                /* Ignore action keys based on ASCII values */
                // TYPING

                if(c == CHAR_BACKSPACE){
                    tg_moveCursor(-1, 0);
                    tg_putChar(' ');
                    tg_moveCursor(-1, 0);
                }

                if(c == CHAR_ENTER){
                    currentCursorX = 0;
                    if(currentCursorY < TUI_ROWS - 1){
                        currentCursorY++;
                        tg_putCursor(currentCursorX, currentCursorY);
                    }
                }

                if(!(c == CHAR_ESCAPE ||
                    c == CHAR_BACKSPACE ||
                    c == CHAR_TAB ||
                    c == CHAR_ENTER ||
                    c == CHAR_DELETE)){
                        
                        tg_putChar(c);
                        //tg_renderElements();
                        // Tick counting by user activity, not globally
                        tg_writeBuffer("Hello World %d", 5, 6, 20, 10, T_COLOR_WHITE, T_COLOR_BLACK, ticks);
                        
                        ticks++;
                    }
                }
            }
        // Cursor coordinates for testing
        tg_writeBuffer("X: %d Y: %d", 0, 0, 10, 0, T_COLOR_WHITE, T_COLOR_BLACK, currentCursorX, currentCursorY);
        tg_updateCursor();
    }
    
    closeKeyboard();
    tg_set25Lines();
    tg_cls();
    printf("\nUser pressed ESC.");
    return 0;
}

    #endif