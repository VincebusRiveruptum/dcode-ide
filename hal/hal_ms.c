#include "hal_ms.h"
/* Function overload ==========================*/
/* Not the cleanest approach, i know...        */

unsigned char hal_check_mouse(){
    #if defined(__WATCOMC__) || defined(__MSDOS__)
    check_mouse();
    #else
    check_mouse();
    #endif
}
void hal_enable_mouse(){
    #if defined(__WATCOMC__) || defined(__MSDOS__)
    enable_mouse_cursor();
    #else
    enable_mouse();
    #endif
}
void hal_disable_mouse(){
    #if defined(__WATCOMC__) || defined(__MSDOS__)
    disable_mouse_cursor();
    #else
    disable_mouse();
    #endif
}

void hal_read_mouse(){
    #if defined(__WATCOMC__) || defined(__MSDOS__)
    read_mouse_cursor();
    #else
    read_mouse();
    #endif
}