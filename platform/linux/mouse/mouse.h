#ifndef PLATFORM_LINUX_MOUSE_H
#define PLATFORM_LINUX_MOUSE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>

extern unsigned int mouse_x;
extern unsigned int mouse_y;
extern unsigned int mouse_left_btn;
extern unsigned int mouse_right_btn;

unsigned char check_mouse();
void enable_mouse();
void disable_mouse();
void read_mouse();

#endif