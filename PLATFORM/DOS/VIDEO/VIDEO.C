#include "VIDEO.H"
#include "VGAREGS.H"
#include "..\..\..\CORE\DRAW\DRAW.H"
#include "..\..\..\CORE\EDITOR\EDITOR.H"

unsigned short *textmemptr;
unsigned short editormemptr[HAL_VIDEO_BUFFER_SIZE];
int v_currentMode = HAL_VID_80X25;

char tempBuffer[HAL_VIDEO_BUFFER_SIZE];

void hal_vid_init(void){
    hal_vid_setVideoMode(0, HAL_NO_MSG);

    textmemptr = (unsigned short *)0xB8000;
    dw_cls(textmemptr);
}

void hal_vid_set25Lines(void){
    VIDEO_ROWS = 25;
    VIDEO_COLS = 80;
    _set80x25_asm();
}

void hal_vid_set43Lines(void){
    VIDEO_ROWS = 43;
    VIDEO_COLS = 80;
    _set80x43_asm();
}

void hal_vid_set50Lines(void){
    VIDEO_ROWS = 50;
    VIDEO_COLS = 80;
    _set80x50_asm();
}

void hal_vid_set80x60(void){
    VIDEO_ROWS = 60;
    VIDEO_COLS = 80;
    _set80x60_asm();
}

void hal_vid_set132x25(void){
    VIDEO_ROWS = 25;
    VIDEO_COLS = 132;
    _set132x25_asm();
}

void hal_vid_set132x43(void){
    VIDEO_ROWS = 43;
    VIDEO_COLS = 132;
    _set132x43_asm();
}

void hal_vid_set132x50(void){
    VIDEO_ROWS = 50;
    VIDEO_COLS = 132;
    _set132x50_asm();
}

void hal_vid_set132x60(void){
    VIDEO_ROWS = 60;
    VIDEO_COLS = 132;
    _set132x60_asm();
}

unsigned short hal_vid_getVideoBufferSize(void){
    return VIDEO_COLS * VIDEO_ROWS;
}

void hal_vid_clearBuffer(unsigned short *buffer){
    int i=0;
    while(i < hal_vid_getVideoBufferSize()){
        buffer[i] = ' ';
        i++;
    }
}

unsigned char hal_vid_setVideoMode(unsigned char mode, unsigned char show_msg){
    unsigned char recmode;

    switch(mode){
        case HAL_VID_80X25:
            if(show_msg == HAL_SHOW_MSG) ed_statusBarMessage("Set 80x25 video mode (%d)", mode);
            hal_vid_set25Lines();
            break;
        case HAL_VID_80X43:
            if(show_msg == HAL_SHOW_MSG) ed_statusBarMessage("Set 80x43 video mode (%d)", mode);       
            hal_vid_set43Lines();
            break;
        case HAL_VID_80X50:
            if(show_msg == HAL_SHOW_MSG) ed_statusBarMessage("Set 80x50 video mode (%d)", mode);       
            hal_vid_set50Lines();
            break;
        case HAL_VID_80X60:
            if(show_msg == HAL_SHOW_MSG) ed_statusBarMessage("80x60 video mode NOT SUPPORTED YET (%d)", mode);       
            /* hal_vid_set80x60(); */
            break;
        case HAL_VID_132X25:
            if(show_msg == HAL_SHOW_MSG) ed_statusBarMessage("132x25 video mode NOT SUPPORTED YET (%d)", mode);       
            /* hal_vid_set132x25(); */
            break;
        case HAL_VID_132X43:
            if(show_msg == HAL_SHOW_MSG) ed_statusBarMessage("132x43 video mode NOT SUPPORTED YET (%d)", mode);       
            /* hal_vid_set132x43(); */
            break;
        case HAL_VID_132X50:
            if(show_msg == HAL_SHOW_MSG) ed_statusBarMessage("Set 132x50 video mode (%d)", mode);       
            hal_vid_set132x50();
            break;
        case HAL_VID_132X60:
            if(show_msg == HAL_SHOW_MSG) ed_statusBarMessage("Set 132x60 video mode (%d)", mode);       
            hal_vid_set132x60();
            break;
        default:        /* if mode is 0 or whatever */
            recmode = hal_vid_setVideoMode(settings.DEFAULT_VIDEO_MODE, show_msg);

            if(show_msg == HAL_SHOW_MSG) ed_statusBarMessage("Set default video mode (%d)", settings.DEFAULT_VIDEO_MODE);
            return recmode;
    }

    return mode;
}

void hal_vid_cycleVideoModes(void){
    v_currentMode++;

    if(v_currentMode > 8) v_currentMode = 0;

    hal_vid_setVideoMode(v_currentMode, HAL_SHOW_MSG);
}

void hal_vid_refresh(void){
    /* DOS writing to textmemptr is direct, no-op needed */
}

void hal_vid_putCursor(unsigned char x, unsigned char y){
    unsigned short temp;

    currentCursorX = x;
    currentCursorY = y;
    temp = currentCursorY * VIDEO_COLS + currentCursorX;

    outPortb(0x3D4, 14);
    outPortb(0x3D5, temp >> 8);
    outPortb(0x3D4, 15);
    outPortb(0x3D5, temp);
}

void hal_vid_close(void){
    hal_vid_set25Lines();
}
