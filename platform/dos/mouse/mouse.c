#include "mouse.h"

union REGS regs; 

unsigned int mouse_x = 0;
unsigned int mouse_y = 0;
unsigned int mouse_left_btn = 0;
unsigned int mouse_right_btn = 0;

unsigned char check_mouse(){
    /* Check if installed*/
    regs.x.eax = 0x00;
    int386(0x33, &regs, &regs);

    /* Mice not installed*/
    if(regs.x.eax == 0x0){
        fprintf(stderr, "Mouse is not installed.");
        return 0;
    }

    return 1;
}

void enable_mouse_cursor(){
    regs.x.eax = 0x01;
    int386(0x33, &regs, &regs);

    return;
}

void disable_mouse_cursor(){
    regs.x.eax = 0x02;
    int386(0x33, &regs, &regs);

    return;
}
/* Position ====================================*/

void read_mouse_cursor(){

    regs.x.eax = 0x03;
    int386(0x33, &regs, &regs);

    mouse_x = regs.x.ecx;
    mouse_y = regs.x.edx; 
    mouse_left_btn = (regs.x.ebx & MOUSE_LEFT_BTN);
    mouse_right_btn = (regs.x.ebx & MOUSE_RIGHT_BTN) >> 1; 

    return;
}

#ifdef STANDALONE

int main(){
    char c;
    printf("\nTesting mouse, move cursor or press any key to return.\n");

    enable_mouse_cursor();

    while(c != 1){
        read_mouse_cursor();

        printf("\nX: %d", mouse_x);
        printf("\nY: %d", mouse_y);
        printf("\nLEFT: %d", mouse_left_btn);
        printf("\nRIGHT: %d", mouse_right_btn);
        getch();
        c = getch();
    }

    disable_mouse_cursor();

    return 0;
}

#endif