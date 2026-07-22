#ifndef PLATFORM_DOS_VIDEO_H
#define PLATFORM_DOS_VIDEO_H

#include "../../../hal/hal_vid.h"
#include "../../../core/config/config.h"

/* ASM Prototypes and pragmas */
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

#pragma aux _set80x60_asm = \
    "mov ax, 0x1202" \
    "mov bl, 0x30" \
    "int 0x10" \
    "mov ax, 0x0083" \
    "int 0x10" \
    "mov ax, 0x1112" \
    "mov bl, 0x00" \
    "int 0x10" \
    modify[ax bx cx dx]

#pragma aux _set132x25_asm = \
    "mov ax, 0x4F02" \
    "mov bx, 0x8109" \
    "int 0x10" \
    "mov ax, 0x1112" \
    "mov bl, 0x00" \
    "int 0x10" \
    modify [ax bx cx dx]

#pragma aux _set132x43_asm = \
    "mov ax, 0x4F02" \
    "mov bx, 0x810A" \
    "int 0x10" \
    "mov ax, 0x1112" \
    "mov bl, 0x00" \
    "int 0x10" \
    modify [ax bx cx dx]

#pragma aux _set132x50_asm = \
    "mov ax, 0x4F02" \
    "mov bx, 0x810B" \
    "int 0x10" \
    "mov ax, 0x1112" \
    "mov bl, 0x00" \
    "int 0x10" \
    modify [ax bx cx dx]

#pragma aux _set132x60_asm = \
    "mov ax, 0x4F02" \
    "mov bx, 0x810C" \
    "int 0x10" \
    "mov ax, 0x1112" \
    "mov bl, 0x00" \
    "int 0x10" \
    modify [ax bx cx dx]

#endif
