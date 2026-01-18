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

#define TUI_MODE_COMMAND 1
#define TUI_MODE_INSERT 2

/* Global =================================================*/

extern unsigned char tuiMode;

/* Asm ====================================================*/

#pragma aux clearScreen = \
    "int 10h", \
    "mov ax, 0600h", \
    "mov bh, 07h", \
    "mov cx, 0000h", \
    "mov dx, 184Fh", \
    "int 10h";

#endif