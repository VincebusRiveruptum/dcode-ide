#include "VIDEO.H"


unsigned short *textmemptr;
unsigned short editormemptr[VIDEO_BUFFER_SIZE];
int v_currentMode = VID_80X25;

char tempBuffer[VIDEO_BUFFER_SIZE];

void v_init_video(){
    switch(settings.VIDEO_MODE){
        case 3:
            VIDEO_ROWS = 50;
            VIDEO_COLS = 80;
             v_set50Lines();
            break;
        case 2:
            VIDEO_ROWS = 43;
            VIDEO_COLS = 80;
            v_set43Lines();
        case 1:
        case 0:
        default:
            VIDEO_ROWS = 25;
            VIDEO_COLS = 80;
            v_set25Lines();
            break;    
    }

    textmemptr = (unsigned short *)0xB8000;
    dw_cls(textmemptr);
}

void v_set25Lines(){
    VIDEO_ROWS = 25;
    VIDEO_COLS = 80;
    _set80x25_asm();
}

void v_set43Lines(){
    VIDEO_ROWS = 43;
    VIDEO_COLS = 80;

    _set80x43_asm();
}

void v_set50Lines(){
    VIDEO_ROWS = 50;
    VIDEO_COLS = 80;

    _set80x50_asm();
}
void v_set132x25(){
    VIDEO_ROWS = 25;
    VIDEO_COLS = 132;

    _set132x25_asm();
}
void v_set132x43(){
    VIDEO_ROWS = 43;
    VIDEO_COLS = 132;

    _set132x43_asm();
}
void v_set132x50(){
    VIDEO_ROWS = 50;
    VIDEO_COLS = 132;

    _set132x60_asm();
}
void v_set132x60(){
    VIDEO_ROWS = 60;
    VIDEO_COLS = 132;

    _set132x60_asm();
}

unsigned short v_getVideoBufferSize(){
    return VIDEO_COLS * VIDEO_ROWS;
}

void v_clearBuffer(unsigned short *buffer){
    int i=0;
    while(i < v_getVideoBufferSize()){
        buffer[i] = ' ';
        i++;
    }
}

void v_cycleVideoModes(){
    v_currentMode++;

    if(v_currentMode > 8) v_currentMode = 0;

    switch(v_currentMode){
        case VID_80X25:
            ed_statusBarMessage("Set 80x25 video mode (%d)", v_currentMode);       v_set25Lines();
            break;
        case VID_80X43:
            ed_statusBarMessage("Set 80x43 video mode (%d)", v_currentMode);       v_set43Lines();
            break;
        case VID_80X50:
            ed_statusBarMessage("Set 80x50 video mode (%d)", v_currentMode);       v_set50Lines();
            break;
        case VID_80X60:
            ed_statusBarMessage("Set 80x60 video mode (%d)", v_currentMode);       v_set50Lines();
            break;
        case VID_132X25:
            ed_statusBarMessage("Set 132x25 video mode (%d)", v_currentMode);       v_set132x25();
            break;
        case VID_132X43:
            ed_statusBarMessage("Set 132x43 video mode (%d)", v_currentMode);       v_set132x43();
            break;
        case VID_132X50:
            ed_statusBarMessage("Set 132x50 video mode (%d)", v_currentMode);       v_set132x50();
            break;
        case VID_132X60:
            ed_statusBarMessage("Set 132x60 video mode (%d)", v_currentMode);       v_set132x60();
            break;
        default:
            ed_statusBarMessage("Set default video mode (%d)", v_currentMode);
            v_set25Lines();
    }
}