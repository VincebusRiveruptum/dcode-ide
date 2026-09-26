#ifndef HAL_VGAREGS_H
#define HAL_VGAREGS_H

#include <stdio.h>
#include <stdlib.h>
#include <dos.h>

/* CONSTANTS ===========================================================================*/

#define ATTR_ADDR 0x3c0
#define MISC_ADDR 0x3c2
#define VGAENABLE_ADDR 0x3c3
#define SEQU_ADDR 0x3c4
#define GRAC_ADDR 0x3ce
#define CRTC_ADDR 0x3d4
#define STATUS_ADDR 0x3da
#define CHIPSTECH_ADDR 0x3d6


#define VIDEO_BUFFER_ADDR 0xB8000
/* PROTOYPES ===========================================================================*/

void unProtectCRTC(void);
unsigned char registerIn(int, char);
void registerOut(int, char, unsigned char);

extern void outPortb(int, unsigned char);
extern void outPortw(int, int);
extern unsigned char inPortb(int);
extern unsigned int inPortw(int);


#pragma aux inPortb =   \
    "in al, dx"         \
    parm [dx]           \
    value [al];


#pragma aux inPortw =   \
    "in ax, dx"         \
    parm [dx]           \
    value [ax];

#pragma aux outPortb =  \
    "out dx, al"        \
    parm [dx] [al];


#pragma aux outPortw =  \
    "out dx, ax"        \
    parm [dx] [ax];


#endif
