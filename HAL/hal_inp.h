#ifndef HAL_INP_H
#define HAL_INP_H

#include "STD.H"

/* Neutral Key Definitions mapped to hardware keys */
#define HAL_KEY_ESC         1
#define HAL_KEY_1           2
#define HAL_KEY_2           3
#define HAL_KEY_3           4
#define HAL_KEY_4           5
#define HAL_KEY_5           6
#define HAL_KEY_6           7
#define HAL_KEY_7           8
#define HAL_KEY_8           9
#define HAL_KEY_9           10
#define HAL_KEY_0           11
#define HAL_KEY_MINUS       12
#define HAL_KEY_EQUALS      13

#define HAL_KEY_BACKSPACE   14
#define HAL_KEY_TAB         15

#define HAL_KEY_Q           16
#define HAL_KEY_W           17
#define HAL_KEY_E           18
#define HAL_KEY_R           19
#define HAL_KEY_T           20
#define HAL_KEY_Y           21
#define HAL_KEY_U           22
#define HAL_KEY_I           23
#define HAL_KEY_O           24
#define HAL_KEY_P           25
#define HAL_KEY_LBRACKET    26
#define HAL_KEY_RBRACKET    27

#define HAL_KEY_ENTER       28
#define HAL_KEY_LCTRL       29

#define HAL_KEY_A           30
#define HAL_KEY_S           31
#define HAL_KEY_D           32
#define HAL_KEY_F           33
#define HAL_KEY_G           34
#define HAL_KEY_H           35
#define HAL_KEY_J           36
#define HAL_KEY_K           37
#define HAL_KEY_L           38
#define HAL_KEY_SEMICOLON   39
#define HAL_KEY_APOSTROPHE  40
#define HAL_KEY_GRAVE       41

#define HAL_KEY_LSHIFT      42
#define HAL_KEY_BACKSLASH   43

#define HAL_KEY_Z           44
#define HAL_KEY_X           45
#define HAL_KEY_C           46
#define HAL_KEY_V           47
#define HAL_KEY_B           48
#define HAL_KEY_N           49
#define HAL_KEY_M           50
#define HAL_KEY_COMMA       51
#define HAL_KEY_PERIOD      52
#define HAL_KEY_SLASH       53
#define HAL_KEY_RSHIFT      54

#define HAL_KEY_KP_MULTIPLY 55
#define HAL_KEY_LALT        56
#define HAL_KEY_SPACE       57
#define HAL_KEY_CAPSLOCK    58

#define HAL_KEY_F1          59
#define HAL_KEY_F2          60
#define HAL_KEY_F3          61
#define HAL_KEY_F4          62
#define HAL_KEY_F5          63
#define HAL_KEY_F6          64
#define HAL_KEY_F7          65
#define HAL_KEY_F8          66
#define HAL_KEY_F9          67
#define HAL_KEY_F10         68

#define HAL_KEY_NUMLOCK     69
#define HAL_KEY_SCROLLLOCK  70

#define HAL_KEY_KP_7        71
#define HAL_KEY_KP_8        72
#define HAL_KEY_KP_9        73
#define HAL_KEY_KP_MINUS    74
#define HAL_KEY_KP_4        75
#define HAL_KEY_KP_5        76
#define HAL_KEY_KP_6        77
#define HAL_KEY_KP_PLUS     78
#define HAL_KEY_KP_1        79
#define HAL_KEY_KP_2        80
#define HAL_KEY_KP_3        81
#define HAL_KEY_KP_0        82
#define HAL_KEY_KP_PERIOD   83

#define HAL_KEY_F11         87
#define HAL_KEY_F12         88

#define HAL_KEY_KP_ENTER    28
#define HAL_KEY_RCTRL       29
#define HAL_KEY_KP_DIVIDE   53
#define HAL_KEY_RALT        56

#define HAL_KEY_HOME        71
#define HAL_KEY_UP          72
#define HAL_KEY_PAGEUP      73
#define HAL_KEY_LEFT        75
#define HAL_KEY_RIGHT       77
#define HAL_KEY_END         79
#define HAL_KEY_DOWN        80
#define HAL_KEY_PAGEDOWN    81
#define HAL_KEY_INSERT      82
#define HAL_KEY_DELETE      83

#define HAL_CHAR_BACKSPACE  0x08
#define HAL_CHAR_TAB        0x09
#define HAL_CHAR_ENTER      0x0D
#define HAL_CHAR_SPACE      0x20
#define HAL_CHAR_ESCAPE     0x1B
#define HAL_CHAR_DELETE     0x7F

/* Legacy Aliases for backward compatibility in CORE engine */
#define CHAR_BACKSPACE      HAL_CHAR_BACKSPACE
#define CHAR_TAB            HAL_CHAR_TAB
#define CHAR_ENTER          HAL_CHAR_ENTER
#define CHAR_SPACE          HAL_CHAR_SPACE
#define CHAR_ESCAPE         HAL_CHAR_ESCAPE
#define CHAR_DELETE         HAL_CHAR_DELETE

#define KEY_UP              HAL_KEY_UP
#define KEY_DOWN            HAL_KEY_DOWN
#define KEY_LEFT            HAL_KEY_LEFT
#define KEY_RIGHT           HAL_KEY_RIGHT
#define KEY_HOME            HAL_KEY_HOME
#define KEY_END             HAL_KEY_END
#define KEY_PAGEUP          HAL_KEY_PAGEUP
#define KEY_PAGEDOWN        HAL_KEY_PAGEDOWN
#define KEY_INSERT          HAL_KEY_INSERT
#define KEY_DELETE          HAL_KEY_DELETE

#define HAL_INP_TRIGGER_LEVEL 0
#define HAL_INP_TRIGGER_EDGE  1

extern volatile unsigned char inp_state[256];
extern volatile unsigned char inp_pressed[256];
extern volatile unsigned char inp_released[256];
extern volatile unsigned char inp_justPressed[256];

void hal_inp_initKeyboard(void);
void hal_inp_closeKeyboard(void);
void hal_inp_updateKeyboard(void);
void hal_inp_clearKeyboardBuffer(void);
bool hal_inp_isKeyDown(unsigned char key);
bool hal_inp_isKeyPressed(unsigned char key);
bool hal_inp_isKeyReleased(unsigned char key);
unsigned char hal_inp_keysPressed(int triggerType, int nKeys, ...);
unsigned char hal_inp_keysDown(int nKeys, ...);
void hal_inp_waitForRelease(void);

bool hal_inp_kbhit(void);
char hal_inp_getch(void);
void hal_sleep_ms(int ms);

#endif
