#ifndef PLATFORM_DOS_INPUT_H
#define PLATFORM_DOS_INPUT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <time.h>
#include <ctype.h>
#include <dos.h>
#include <stdarg.h>

#include "..\..\..\HAL\hal_inp.h"
#include "..\VIDEO\VGAREGS.H"

#ifndef __cplusplus
typedef unsigned char bool;
#define true 1
#define false 0
#endif

/* Keep backward compatible macros mapping to HAL values */
#define KEY_ESC             HAL_KEY_ESC
#define KEY_1               HAL_KEY_1
#define KEY_2               HAL_KEY_2
#define KEY_3               HAL_KEY_3
#define KEY_4               HAL_KEY_4
#define KEY_5               HAL_KEY_5
#define KEY_6               HAL_KEY_6
#define KEY_7               HAL_KEY_7
#define KEY_8               HAL_KEY_8
#define KEY_9               HAL_KEY_9
#define KEY_0               HAL_KEY_0
#define KEY_MINUS           HAL_KEY_MINUS
#define KEY_EQUALS          HAL_KEY_EQUALS

#define KEY_BACKSPACE       HAL_KEY_BACKSPACE
#define KEY_TAB             HAL_KEY_TAB

#define KEY_Q               HAL_KEY_Q
#define KEY_W               HAL_KEY_W
#define KEY_E               HAL_KEY_E
#define KEY_R               HAL_KEY_R
#define KEY_T               HAL_KEY_T
#define KEY_Y               HAL_KEY_Y
#define KEY_U               HAL_KEY_U
#define KEY_I               HAL_KEY_I
#define KEY_O               HAL_KEY_O
#define KEY_P               HAL_KEY_P
#define KEY_LBRACKET        HAL_KEY_LBRACKET
#define KEY_RBRACKET        HAL_KEY_RBRACKET

#define KEY_ENTER           HAL_KEY_ENTER
#define KEY_LCTRL           HAL_KEY_LCTRL

#define KEY_A               HAL_KEY_A
#define KEY_S               HAL_KEY_S
#define KEY_D               HAL_KEY_D
#define KEY_F               HAL_KEY_F
#define KEY_G               HAL_KEY_G
#define KEY_H               HAL_KEY_H
#define KEY_J               HAL_KEY_J
#define KEY_K               HAL_KEY_K
#define KEY_L               HAL_KEY_L
#define KEY_SEMICOLON       HAL_KEY_SEMICOLON
#define KEY_APOSTROPHE      HAL_KEY_APOSTROPHE
#define KEY_GRAVE           HAL_KEY_GRAVE

#define KEY_LSHIFT          HAL_KEY_LSHIFT
#define KEY_BACKSLASH       HAL_KEY_BACKSLASH

#define KEY_Z               HAL_KEY_Z
#define KEY_X               HAL_KEY_X
#define KEY_C               HAL_KEY_C
#define KEY_V               HAL_KEY_V
#define KEY_B               HAL_KEY_B
#define KEY_N               HAL_KEY_N
#define KEY_M               HAL_KEY_M
#define KEY_COMMA           HAL_KEY_COMMA
#define KEY_PERIOD          HAL_KEY_PERIOD
#define KEY_SLASH           HAL_KEY_SLASH
#define KEY_RSHIFT          HAL_KEY_RSHIFT

#define KEY_KP_MULTIPLY     HAL_KEY_KP_MULTIPLY
#define KEY_LALT            HAL_KEY_LALT
#define KEY_SPACE           HAL_KEY_SPACE
#define KEY_CAPSLOCK        HAL_KEY_CAPSLOCK

#define KEY_F1              HAL_KEY_F1
#define KEY_F2              HAL_KEY_F2
#define KEY_F3              HAL_KEY_F3
#define KEY_F4              HAL_KEY_F4
#define KEY_F5              HAL_KEY_F5
#define KEY_F6              HAL_KEY_F6
#define KEY_F7              HAL_KEY_F7
#define KEY_F8              HAL_KEY_F8
#define KEY_F9              HAL_KEY_F9
#define KEY_F10             HAL_KEY_F10

#define KEY_NUMLOCK         HAL_KEY_NUMLOCK
#define KEY_SCROLLLOCK      HAL_KEY_SCROLLLOCK

#define KEY_KP_7            HAL_KEY_KP_7
#define KEY_KP_8            HAL_KEY_KP_8
#define KEY_KP_9            HAL_KEY_KP_9
#define KEY_KP_MINUS        HAL_KEY_KP_MINUS
#define KEY_KP_4            HAL_KEY_KP_4
#define KEY_KP_5            HAL_KEY_KP_5
#define KEY_KP_6            HAL_KEY_KP_6
#define KEY_KP_PLUS         HAL_KEY_KP_PLUS
#define KEY_KP_1            HAL_KEY_KP_1
#define KEY_KP_2            HAL_KEY_KP_2
#define KEY_KP_3            HAL_KEY_KP_3
#define KEY_KP_0            HAL_KEY_KP_0
#define KEY_KP_PERIOD       HAL_KEY_KP_PERIOD

#define KEY_F11             HAL_KEY_F11
#define KEY_F12             HAL_KEY_F12

#define KEY_KP_ENTER        HAL_KEY_KP_ENTER
#define KEY_RCTRL           HAL_KEY_RCTRL
#define KEY_KP_DIVIDE       HAL_KEY_KP_DIVIDE
#define KEY_RALT            HAL_KEY_RALT

#define KEY_HOME            HAL_KEY_HOME
#define KEY_UP              HAL_KEY_UP
#define KEY_PAGEUP          HAL_KEY_PAGEUP
#define KEY_LEFT            HAL_KEY_LEFT
#define KEY_RIGHT           HAL_KEY_RIGHT
#define KEY_END             HAL_KEY_END
#define KEY_DOWN            HAL_KEY_DOWN
#define KEY_PAGEDOWN        HAL_KEY_PAGEDOWN
#define KEY_INSERT          HAL_KEY_INSERT
#define KEY_DELETE          HAL_KEY_DELETE

#define CHAR_BACKSPACE      HAL_CHAR_BACKSPACE
#define CHAR_TAB            HAL_CHAR_TAB
#define CHAR_ENTER          HAL_CHAR_ENTER
#define CHAR_SPACE          HAL_CHAR_SPACE
#define CHAR_ESCAPE         HAL_CHAR_ESCAPE
#define CHAR_DELETE         HAL_CHAR_DELETE

#define INP_TRIGGER_LEVEL   HAL_INP_TRIGGER_LEVEL
#define INP_TRIGGER_EDGE    HAL_INP_TRIGGER_EDGE

extern unsigned char inp_scanCode;

#endif
