#ifndef HAL_MOUSE_H
#define HAL_MOUSE_H

#if defined(__WATCOMC__) || defined(__MSDOS__)
#include "../platform/dos/mouse/mouse.h"
#else
#include "../platform/linux/mouse/mouse.h"
#endif

extern unsigned int hal_mouse_x;
extern unsigned int hal_mouse_y;
extern unsigned int hal_mouse_left_btn;
extern unsigned int hal_mouse_right_btn;

unsigned char hal_check_mouse();
void hal_enable_mouse();
void hal_disable_mouse();
void hal_read_mouse();

#endif