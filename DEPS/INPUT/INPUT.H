#ifndef DEPS_INPUT_H
#define DEPS_INPUT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

#include <time.h>
#include <ctype.h>
#include <dos.h>
#include <stdarg.h>
#include "..\VGA\VGAREGS.H"

#ifndef __cplusplus
typedef unsigned char bool;
#define true 1
#define false 0
#endif

/* 1–13 : Esc + number row */
#define KEY_ESC             1
#define KEY_1               2
#define KEY_2               3
#define KEY_3               4
#define KEY_4               5
#define KEY_5               6
#define KEY_6               7
#define KEY_7               8
#define KEY_8               9
#define KEY_9              10
#define KEY_0              11
#define KEY_MINUS          12
#define KEY_EQUALS         13

/* 14–15 : Editing */
#define KEY_BACKSPACE      14
#define KEY_TAB            15

/* 16–27 : Q row */
#define KEY_Q              16
#define KEY_W              17
#define KEY_E              18
#define KEY_R              19
#define KEY_T              20
#define KEY_Y              21
#define KEY_U              22
#define KEY_I              23
#define KEY_O              24
#define KEY_P              25
#define KEY_LBRACKET       26
#define KEY_RBRACKET       27

/* 28–29 : Enter & Ctrl */
#define KEY_ENTER          28
#define KEY_LCTRL          29

/* 30–41 : A row */
#define KEY_A              30
#define KEY_S              31
#define KEY_D              32
#define KEY_F              33
#define KEY_G              34
#define KEY_H              35
#define KEY_J              36
#define KEY_K              37
#define KEY_L              38
#define KEY_SEMICOLON      39
#define KEY_APOSTROPHE     40
#define KEY_GRAVE          41

/* 42–43 : Shift */
#define KEY_LSHIFT         42
#define KEY_BACKSLASH      43

/* 44–54 : Z row */
#define KEY_Z              44
#define KEY_X              45
#define KEY_C              46
#define KEY_V              47
#define KEY_B              48
#define KEY_N              49
#define KEY_M              50
#define KEY_COMMA          51
#define KEY_PERIOD         52
#define KEY_SLASH          53
#define KEY_RSHIFT         54

/* 55–58 : Modifiers */
#define KEY_KP_MULTIPLY    55
#define KEY_LALT           56
#define KEY_SPACE          57
#define KEY_CAPSLOCK       58

/* 59–68 : Function keys */
#define KEY_F1             59
#define KEY_F2             60
#define KEY_F3             61
#define KEY_F4             62
#define KEY_F5             63
#define KEY_F6             64
#define KEY_F7             65
#define KEY_F8             66
#define KEY_F9             67
#define KEY_F10            68

/* 69–70 : Locks */
#define KEY_NUMLOCK        69
#define KEY_SCROLLLOCK     70

/* 71–83 : Keypad & navigation */
#define KEY_KP_7           71
#define KEY_KP_8           72
#define KEY_KP_9           73
#define KEY_KP_MINUS       74
#define KEY_KP_4           75
#define KEY_KP_5           76
#define KEY_KP_6           77
#define KEY_KP_PLUS        78
#define KEY_KP_1           79
#define KEY_KP_2           80
#define KEY_KP_3           81
#define KEY_KP_0           82
#define KEY_KP_PERIOD      83

/* 87–88 : Extended function keys */
#define KEY_F11            87
#define KEY_F12            88

/* E0-prefixed keys */
#define KEY_KP_ENTER       28
#define KEY_RCTRL          29
#define KEY_KP_DIVIDE      53
#define KEY_RALT           56

#define KEY_HOME           71
#define KEY_UP             72
#define KEY_PAGEUP         73
#define KEY_LEFT           75
#define KEY_RIGHT          77
#define KEY_END            79
#define KEY_DOWN           80
#define KEY_PAGEDOWN       81
#define KEY_INSERT         82
#define KEY_DELETE         83

/** SOME EKIVALENT ASCII VALUES */
// in 16-bit word
#define CHAR_BACKSPACE      0x08
#define CHAR_TAB            0x09
#define CHAR_ENTER          0x0D
#define CHAR_SPACE          0x20
#define CHAR_ESCAPE         0x1B
#define CHAR_DELETE         0x7F

#define INP_TRIGGER_LEVEL   0
#define INP_TRIGGER_EDGE    1

/* PROTOTYPES ===========================================================*/

void inp_initKeyboard();
void inp_closeKeyboard();
void inp_updateKeyboard();

bool inp_isKeyDown(unsigned char key);
bool inp_isKeyPressed(unsigned char key);
bool inp_isKeyReleased(unsigned char key);
unsigned char inp_keysPressed(int triggerType, int nKeys, ...);
unsigned char inp_keysDown(int nKeys, ...);

/* VARIABLES ============================================================*/

extern volatile unsigned char inp_state[256];
extern volatile unsigned char inp_pressed[256];
extern volatile unsigned char inp_released[256];
extern volatile unsigned char inp_justPressed[256];
extern const unsigned char *inp_keyboardMap[256];
extern unsigned char inp_scanCode;

#endif
