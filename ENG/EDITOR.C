/*
    This is a basic TUI library, right now just for testing purposes
    By Vincebus Riveruptum
    2026

    Credits osdever.net

    http://www.osdever.net/bkerndev/Docs/printing.htm
*/

#include "EDITOR.H"


//unsigned char attrib = 0x07; // Default attribute: White on Black
struct Container *root;

unsigned char VIDEO_COLS = 80;
unsigned char VIDEO_ROWS = 25;

unsigned char currentCursorX = 0;
unsigned char currentCursorY = 0;

bool ed_renderEvent = false;

void ed_updateCursor(){
    ed_putCursor(currentCursorX, currentCursorY);
}

void ed_putCursor(unsigned char x, unsigned char y){
    unsigned short temp;

    currentCursorX = x;
    currentCursorY = y;
    /* The equation for finding the index in a linear
    *  chunk of memory can be represented by:
    *  Index = [(y * width) + x] */
    temp = currentCursorY * VIDEO_COLS + currentCursorX;

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

void ed_moveCursor(short x, short y){
    if(currentCursorX + x >= VIDEO_COLS || currentCursorY + y >= VIDEO_ROWS){
        return;
    }

    if(currentCursorX + x < 0 || currentCursorY + y < 0){
        return;
    }

    currentCursorX += x;
    currentCursorY += y;

    ed_putCursor(currentCursorX, currentCursorY);
}

void ed_triggerSave(){
    dw_writeBuffer(textmemptr, "Saved!", 0, 0, 10, 0, COLOR_WHITE, COLOR_BLACK);   

    f_dumpToFile("test.txt");
}

void ed_renderElements(){
    el_renderFiles_test();
}
