#ifndef ENG_VIDEO_H
#define ENG_VIDEO_H

#include "STD.H"

/* Global =================================================*/

#define VIDEO_BUFFER_SIZE 4160   /* WE'LL USE 80 * 50  + 2 lines ( 80 x 2 ) as universal buffer size*/
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
    modify [ax bx cx dx]

#pragma aux _set400Lines_asm = \
    "mov ax, 0x1202" \
    "mov bl, 0x30" \
    "int 0x10" \
    "mov ax, 0x0083" \
    "int 0x10" \
    modify [ax bx cx dx]

#pragma aux _set43Lines_asm = \
    "mov ax, 0x1112" \
    "mov bl, 0x00" \
    "int 0x10" \
    modify [ax bx cx dx]
    
#pragma aux _set25Lines_asm = \
    "mov ax, 0x1114" \
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

