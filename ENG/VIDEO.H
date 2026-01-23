#ifndef ENG_VIDEO_H
#define ENG_VIDEO_H

#include "STD.H"

/* Global =================================================*/

extern unsigned short *textmemptr;
extern unsigned char VIDEO_COLS;
extern unsigned char VIDEO_ROWS;
extern unsigned char currentCursorX;
extern unsigned char currentCursorY;

/* Prototypes =============================================*/

/* ASM Prototypes */

// Rembember, there is a 'clear' bit :
// AX = 0x0003 = clear screen
// AX = 0x0083 = don't clear screen

#pragma aux _set350Lines_asm = \
    "mov ax, 0x1201" \
    "mov bl, 0x30" \
    "int 0x10" \
    "mov ax, 0x0083" \
    "int 0x10" \

#pragma aux _set400Lines_asm = \
    "mov ax, 0x1202" \
    "mov bl, 0x30" \
    "int 0x10" \
    "mov ax, 0x0083" \
    "int 0x10" \

#pragma aux _set43Lines_asm = \
    "mov ax, 0x1112" \
    "mov bl, 0x00" \
    "int 0x10" \
    
#pragma aux _set25Lines_asm = \
    "mov ax, 0x1114" \
    "mov bl, 0x00" \
    "int 0x10" \

void v_init_video();
void v_close_video();

void v_set43Lines();
void v_set25Lines();

#endif

