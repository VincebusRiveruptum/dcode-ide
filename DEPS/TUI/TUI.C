/*
    This is a basic TUI library, right now just for testing purposes
    By Vincebus Riveruptum
    2026
*/

#include "TUI.H"

unsigned short *textmemptr;
int attrib = 0x0F;

unsigned char TUI_COLS = 80;
unsigned char TUI_ROWS = 25;

void tg_moveCursor(unsigned char x, unsigned char y){
    unsigned short temp;

    /* The equation for finding the index in a linear
    *  chunk of memory can be represented by:
    *  Index = [(y * width) + x] */
    temp = y * 80 + x;

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

void tg_cls(){
    unsigned short blank;
    int i;

    blank = CHAR_SPACE | (attrib << 8);
    for(i = 0; i < TUI_COLS * TUI_ROWS; i++)
        textmemptr[i] = blank;

    tg_moveCursor(0, 0);
}

void tg_fill(unsigned char background, unsigned char foreground, unsigned char character){
    unsigned short screenCharacter;
    int i;

    screenCharacter = character | ((background << 4 | foreground) << 8);
    
    for(i = 0; i < TUI_COLS * TUI_ROWS; i++)
        textmemptr[i] = screenCharacter;
}

void tg_drawRectangle(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2, unsigned char background, unsigned char foreground, unsigned char character,  bool blinking){
    unsigned short screenCharacter;
    int i, lowerLimit, upperLimit;

    // Boundary check
    if(x1 > x2 || y1 > y2) return;

    i = (y1 * TUI_COLS) + x1;

    screenCharacter = character | ((background << 4 | foreground) << 8);
    
    for(i; i <= (y2 * TUI_COLS) + x2; i++){
        lowerLimit = i % TUI_COLS >= x1;
        upperLimit = i % TUI_COLS <= x2;
    
        if(lowerLimit && upperLimit) textmemptr[i] = screenCharacter;
    }
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

    //tg_drawRectangle(0, 0, TUI_COLS - 1, TUI_ROWS - 1, T_COLOR_BLUE, T_COLOR_LIGHT_BLUE, '°', false);
    tg_fill(T_COLOR_BLUE, T_COLOR_LIGHT_BLUE, '°');
    while(endProgram == false){
        if(keyboardTable[KEY_ESC] == true) endProgram = true;

        if(keyboardTable[KEY_F1] == true) tg_set25Lines();
        if(keyboardTable[KEY_F2] == true) tg_set50Lines();
        if(keyboardTable[KEY_F3] == true) tg_set43Lines();

        if(kbhit()){
            c = getch();
            putch(c);
        }
    }
    
    closeKeyboard();
    tg_set25Lines();
    tg_cls();
    printf("\nUser pressed ESC.");
    return 0;
}

    #endif