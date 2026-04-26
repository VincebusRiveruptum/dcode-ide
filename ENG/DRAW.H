#ifndef ENG_DRAW_H
#define ENG_DRAW_H

#include "STD.H"

#include "..\APP\MAIN.H"

#define COLOR_BLACK 0x00
#define COLOR_BLUE 0x01
#define COLOR_GREEN 0x02
#define COLOR_CYAN 0x03
#define COLOR_RED 0x04
#define COLOR_MAGENTA 0x05
#define COLOR_BROWN 0x06
#define COLOR_LIGHT_GRAY 0x07
#define COLOR_DARK_GRAY 0x08
#define COLOR_LIGHT_BLUE 0x09
#define COLOR_LIGHT_GREEN 0x0A
#define COLOR_LIGHT_CYAN 0x0B
#define COLOR_LIGHT_RED 0x0C
#define COLOR_LIGHT_MAGENTA 0x0D
#define COLOR_LIGHT_YELLOW 0x0E
#define COLOR_WHITE 0x0F

#define LINE_COUNTER_WIDTH 6

#define DW_RESWORD_SINGLE_LINE_COMMENT 4
#define DW_RESWORD_STRING 2
#define DW_RESWORD_INCLUDE 1
#define DW_RESWORD_NONE 0

typedef enum {
    DRAW_BORDER_SIMPLE,
    DRAW_BORDER_DOUBLE
} BorderType;

typedef enum {
    DW_SIDE_TOP_LEFT,
    DW_SIDE_TOP_RIGHT,
    DW_SIDE_BOTTOM_LEFT,
    DW_SIDE_BOTTOM_RIGHT,
    DW_SIDE_TOP,
    DW_SIDE_BOTTOM,
    DW_SIDE_LEFT,
    DW_SIDE_RIGHT,
    DW_SIDE_ALL
} RectangleSides;

void dw_cls(unsigned short *buffer);
void dw_fill(unsigned short *buffer, unsigned char backgroundColor, unsigned char foregroundColor, unsigned char character);
void dw_rectangle(unsigned short *buffer, unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2, unsigned char backgroundBorderColor, unsigned char foregroundBorderColor, unsigned char fillCharacter,  unsigned char foregroundFillColor, unsigned char backgroundFillColor, bool blinking, BorderType borderType);
void dw_writeBuffer(unsigned short *buffer, const char *format, int x1, int y1, int x2, int y2, int foregroundColor, int backgroundColor, ...);
void dw_writeBufferEditorFormatted(unsigned short *destBuffer, int x1, int y1, int x2, int y2, int foregroundColor, int backgroundColor, struct File *file);

void dw_charXY_color(unsigned short *buffer, char c, unsigned char x, unsigned char y, unsigned short color);
void dw_charXY(unsigned short *buffer, char c, unsigned char x, unsigned char y);
char dw_read(unsigned short *buffer, int x, int y);


void dw_writeColor(unsigned short *buffer, int x, int y, unsigned short foregroundColor, unsigned short backgroundColor);
char dw_readForegroundColor(unsigned short *buffer, int x, int y);
char dw_readBackgroundColor(unsigned short *buffer, int x, int y);

extern unsigned char currentCursorX;
extern unsigned char currentCursorY;

#endif