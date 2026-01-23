#include "VIDEO.H"

void v_init_video(){
    VIDEO_ROWS = 25;
    VIDEO_COLS = 80;

    _set25Lines_asm();

    textmemptr = (unsigned short *)0xB8000;
    dw_cls();
}

void v_set43Lines(){
    VIDEO_ROWS = 43;
    VIDEO_COLS = 80;

    _set350Lines_asm();
    _set43Lines_asm();
}

void v_set50Lines(){
    VIDEO_ROWS = 50;
    VIDEO_COLS = 80;

    _set400Lines_asm();
    _set43Lines_asm();
}

void v_set25Lines(){
    VIDEO_ROWS = 25;
    VIDEO_COLS = 80;
    _set400Lines_asm();
    _set25Lines_asm();
}
