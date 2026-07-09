#ifndef ENG_DRAW_H
#define ENG_DRAW_H

#include "../std.h"

#include "../../app/main.h"

struct File;

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

#define DW_RESWORD_NONE 0
#define DW_RESWORD_PREPROCESSOR 1
#define DW_RESWORD_TYPES 2
#define DW_RESWORD_INT 3
#define DW_RESWORD_FLOAT 4
#define DW_RESWORD_CONSTANT 5
#define DW_RESWORD_CHAR 6
#define DW_RESWORD_EXPRESSION 7
#define DW_RESWORD_STRING 8
#define DW_RESWORD_CONTROL_FLOW 9
#define DW_RESWORD_DOS_FUNC 10
#define DW_RESWORD_STD_FUNC 11
#define DW_RESWORD_COMMENT 12
#define DW_RESWORD_FUNCTION 13
#define DW_RESWORD_WIFE 14

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

typedef enum {
	DW_RENDER_NONE,
	DW_RENDER_ALL,
	DW_RENDER_WINDOW,
	DW_RENDER_LINE,
	DW_RENDER_CURSOR,
	DW_RENDER_UI,
} RenderType;

void dw_cls(unsigned short *buffer);
void dw_fill(unsigned short *buffer, unsigned char backgroundColor, unsigned char foregroundColor, unsigned char character);

void dw_rectangle(
    unsigned short *buffer,
    unsigned short x1, 
    unsigned short y1, 
    unsigned short x2,
    unsigned short y2, 
    unsigned char backgroundBorderColor, 
    unsigned char foregroundBorderColor, 
    unsigned char fillCharacter,  
    unsigned char foregroundFillColor, 
    unsigned char backgroundFillColor, 
    bool blinking, 
    BorderType borderType,
    const char *title
);

void dw_writeBuffer(unsigned short *buffer, const char *format, int x1, int y1, int x2, int y2, int foregroundColor, int backgroundColor, ...);
void dw_writeBufferEditorFormatted(unsigned short *destBuffer, int x1, int y1, int x2, int y2, int foregroundColor, int backgroundColor, struct File *file);
bool dw_isCharSelected(struct File *file, int lineIndex, int colIndex);

void dw_charXY_color(unsigned short *buffer, char c, unsigned char x, unsigned char y, unsigned short color);
void dw_charXY(unsigned short *buffer, char c, unsigned char x, unsigned char y);
char dw_read(unsigned short *buffer, int x, int y);


void dw_writeColor(unsigned short *buffer, int x, int y, unsigned short foregroundColor, unsigned short backgroundColor);
char dw_readForegroundColor(unsigned short *buffer, int x, int y);
char dw_readBackgroundColor(unsigned short *buffer, int x, int y);

// SINGLE LINE FORMATTING
void dw_c_lineFormatter(
	unsigned short *destBuffer, 
	int x, 
	int y, 
	size_t len, 
	size_t destWidth,
	char *srcBuffer
);

void dw_txt_lineFormatter(
	unsigned short *destBuffer, 
	int x, 
	int y, 
	size_t len, 
	size_t destWidth,
	char *srcBuffer
);

void dw_copyFormatted(
	unsigned short *destBuffer, 
	int x, 
	int y, 
	size_t len,
	size_t destWidth,
	char *srcBuffer,
	struct File *file
);

extern unsigned char currentCursorX;
extern unsigned char currentCursorY;

//extern bool ed_renderLineEvent;
//extern bool ed_fullRenderEvent;
extern bool dw_renderEvent;
extern RenderType dw_renderEventType;

#endif