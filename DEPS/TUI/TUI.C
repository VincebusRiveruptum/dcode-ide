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

void moveCursor(unsigned char x, unsigned char y)
{
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

void cls(){
    unsigned short blank;
    int i;

    /* Create the blank character: space (0x20) with current attribute.
     * Format: low byte = character, high byte = attribute */
    blank = CHAR_SPACE | (attrib << 8);

    /* Fill the entire screen (80 columns x 25 rows = 2000 words)
     * We can't use memset() here because it fills BYTES, not WORDS.
     * Text mode video memory needs 16-bit values (char + attribute). */
    for(i = 0; i < 80 * 25; i++)
        textmemptr[i] = blank;

    /* Move the hardware cursor to top-left */
    moveCursor(0, 0);
}

void drawRectangle(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2, unsigned char background, unsigned char foreground, unsigned char character,  bool blinking){
    unsigned short screenCharacter;
    int i, lowerLimit, upperLimit;

    i = (y1 * TUI_COLS) + x1;

    screenCharacter = character | ((background << 4 | foreground) << 8);
    
    for(i; i <= (y2 * TUI_COLS) + x2; i++){
        lowerLimit = i % TUI_COLS >= x1;
        upperLimit = i % TUI_COLS <= x2;
    
        if(lowerLimit && upperLimit) textmemptr[i] = screenCharacter;
    }
}

void init_video(){
    textmemptr = (unsigned short *)0xB8000;
    cls();
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
    
    init_video();
    initKeyboard();

    drawRectangle(5, 5, 10, 10, T_COLOR_BLUE, T_COLOR_LIGHT_BLUE, 'L', false);
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