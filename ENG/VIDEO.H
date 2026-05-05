#ifndef ENG_VIDEO_H
#define ENG_VIDEO_H

#include "STD.H"
#include "..\ENG\CONFIG.H"

/* Global =================================================*/

#define VIDEO_BUFFER_SIZE 8192   /* WE'LL USE 132 * 60 (7920) as universal buffer size*/
/* Prototypes =============================================*/

/* ASM Prototypes */

// Rembember, there is a 'clear' bit :
// AX = 0x0003 = clear screen
// AX = 0x0083 = don't clear screen

#pragma aux _set80x25_asm = \
    "mov ax, 0x1202" \
    "mov bl, 0x30" \
    "int 0x10" \
    "mov ax, 0x0083" \
    "int 0x10" \
    "mov ax, 0x1114" \
    "mov bl, 0x00" \
    "int 0x10" \
    modify[ax bx cx dx]

#pragma aux _set80x43_asm = \
    "mov ax, 0x1201" \
    "mov bl, 0x30" \
    "int 0x10" \
    "mov ax, 0x0083" \
    "int 0x10" \
    "mov ax, 0x1112" \
    "mov bl, 0x00" \
    "int 0x10" \
    modify[ax bx cx dx]

#pragma aux _set80x50_asm = \
    "mov ax, 0x1202" \
    "mov bl, 0x30" \
    "int 0x10" \
    "mov ax, 0x0083" \
    "int 0x10" \
    "mov ax, 0x1112" \
    "mov bl, 0x00" \
    "int 0x10" \
    modify[ax bx cx dx]

// VESA
// 1. We set VESA 10C mode with bit 15 high
// 2. We set 8x8 char mode
// so it wiont clear the screen
#pragma aux _set132x60_asm = \
    "mov ax, 0x4F02" \
    "mov bx, 0x810C" \
    "int 0x10" \
    "mov ax, 0x1112" \
    "mov bl, 0x00" \
    "int 0x10" \
    modify [ax bx cx dx]
   
extern unsigned short *textmemptr;      // VIDEO BUFFER
extern unsigned short editormemptr[VIDEO_BUFFER_SIZE];    // EDITOR BUFFER, i think this could be the half of VIDEO_BUFFER_SIZE extern unsigned char tempBuffer[VIDEO_BUFFER_SIZE];   
extern char tempBuffer[VIDEO_BUFFER_SIZE];

extern unsigned char VIDEO_COLS;
extern unsigned char VIDEO_ROWS;
extern unsigned char currentCursorX;
extern unsigned char currentCursorY;
    
void v_init_video();
void v_close_video();

void v_set43Lines();
void v_set25Lines();
unsigned short v_getVideoBufferSize();
void v_clearBuffer(unsigned short *buffer);

#endif

