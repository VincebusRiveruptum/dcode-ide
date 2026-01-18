#ifndef TUI_H
#define TUI_H

#include <STDIO.H>
#include <STDLIB.H>
#include <CONIO.H>
#include <STRING.H>
#include <DOS.H>    
#include <MEM.H>
#include <i86.h>

// THIS REQUIRE THE INPUT DEPS
#include "..\INPUT\INPUT.H"


#define T_COLOR_BLACK 0x00
#define T_COLOR_BLUE 0x01
#define T_COLOR_GREEN 0x02
#define T_COLOR_CYAN 0x03
#define T_COLOR_RED 0x04
#define T_COLOR_MAGENTA 0x05
#define T_COLOR_BROWN 0x06
#define T_COLOR_LIGHT_GRAY 0x07
#define T_COLOR_DARK_GRAY 0x08
#define T_COLOR_LIGHT_BLUE 0x09
#define T_COLOR_LIGHT_GREEN 0x0A
#define T_COLOR_LIGHT_CYAN 0x0B
#define T_COLOR_LIGHT_RED 0x0C
#define T_COLOR_LIGHT_MAGENTA 0x0D
#define T_COLOR_LIGHT_YELLOW 0x0E
#define T_COLOR_WHITE 0x0F

/* Prototypes ==============================================*/

void init_video();
void close_video();
void cls();
void moveCursor(unsigned char x, unsigned char y);

/* Global =================================================*/

extern unsigned short *textmemptr;
extern int attrib;
extern unsigned char TUI_COLS;
extern unsigned char TUI_ROWS;


#endif