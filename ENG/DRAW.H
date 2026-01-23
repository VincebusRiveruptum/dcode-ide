#ifndef ENG_DRAW_H
#define ENG_DRAW_H

#include "STD.H"

#include "TUI.H"

typedef enum {
    DRAW_BORDER_SIMPLE,
    DRAW_BORDER_DOUBLE
} BorderType;

typedef enum {
    DRAW_SIDE_TOP_LEFT,
    DRAW_SIDE_TOP_RIGHT,
    DRAW_SIDE_BOTTOM_LEFT,
    DRAW_SIDE_BOTTOM_RIGHT,
    DRAW_SIDE_TOP,
    DRAW_SIDE_BOTTOM,
    DRAW_SIDE_LEFT,
    DRAW_SIDE_RIGHT,
    DRAW_SIDE_ALL
} RectangleSides;

void dw_cls();
void dw_fill(unsigned char backgroundColor, unsigned char foregroundColor, unsigned char character);
void dw_rectangle(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2, unsigned char backgroundBorderColor, unsigned char foregroundBorderColor, unsigned char fillCharacter,  unsigned char foregroundFillColor, unsigned char backgroundFillColor, bool blinking, BorderType borderType);
void dw_writeBuffer(const char *format, unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char foregroundColor, unsigned char backgroundColor, ...);

extern unsigned char currentCursorX;
extern unsigned char currentCursorY;

#endif