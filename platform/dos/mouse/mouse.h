#ifndef PLATFORM_DOS_MOUSE_H
#define PLATFORM_DOS_MOUSE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <dos.h>

extern unsigned int mouse_x;
extern unsigned int mouse_y;
extern unsigned int mouse_left_btn;
extern unsigned int mouse_right_btn;

#define MOUSE_LEFT_BTN 0x01
#define MOUSE_RIGHT_BTN 0x02

unsigned char check_mouse();
void enable_mouse_cursor();
void disable_mouse_cursor();
void read_mouse_cursor();

#endif