#ifndef TUI_DRAW_H
#define TUI_DRAW_H

#include "STD.H"
#include "TUI.H"

typedef enum {
    TUI_DRAW_BORDER_SIMPLE,
    TUI_DRAW_BORDER_DOUBLE
} BorderType;

typedef enum {
    TUI_DRAW_SIDE_TOP_LEFT,
    TUI_DRAW_SIDE_TOP_RIGHT,
    TUI_DRAW_SIDE_BOTTOM_LEFT,
    TUI_DRAW_SIDE_BOTTOM_RIGHT,
    TUI_DRAW_SIDE_TOP,
    TUI_DRAW_SIDE_BOTTOM,
    TUI_DRAW_SIDE_LEFT,
    TUI_DRAW_SIDE_RIGHT,
    TUI_DRAW_SIDE_ALL
} RectangleSides;


void tg_cls();
void tg_fill(unsigned char backgroundColor, unsigned char foregroundColor, unsigned char character);
void tg_drawRectangle(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2, unsigned char backgroundBorderColor, unsigned char foregroundBorderColor, unsigned char fillCharacter,  unsigned char foregroundFillColor, unsigned char backgroundFillColor, bool blinking, BorderType borderType);
  
#endif