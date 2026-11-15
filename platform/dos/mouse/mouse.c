#include "mouse.h"

union REGS regs; 

unsigned int mouse_x = 0;
unsigned int mouse_y = 0;
unsigned int mouse_left_btn = 0;
unsigned int mouse_right_btn = 0;

void enable_mouse(){
    /* Check if installed*/
    regs.x.eax = 0x00;
    int386(0x33, &regs, &regs);

    /* Mice not installed*/
    if(regs.x.eax == 0x0){
        fprintf(stderr, "Mouse is not installed.");
        return;
    }
    
    regs.x.eax = 0x01;
    int386(0x33, &regs, &regs);

    return;
}

void disable_mouse(){
    regs.x.eax = 0x02;
    int386(0x33, &regs, &regs);

    return;
}
/* Position ====================================*/

void get_mouse_position_button_status(){

    regs.x.eax = 0x03;
    int386(0x33, &regs, &regs);

    mouse_x = regs.x.ecx;
    mouse_y = regs.x.edx; 
    mouse_left_btn = (regs.x.ebx & 0x01);
    mouse_right_btn = (regs.x.ebx & 0x02) >> 1; 

    return;
}


#ifdef STANDALONE

int main(){
    char c;
    printf("\nTesting mouse, move cursor or press any key to return.\n");

    enable_mouse();

    while(c != 1){
        get_mouse_position_button_status();

        printf("\nX: %d", mouse_x);
        printf("\nY: %d", mouse_y);
        printf("\nLEFT: %d", mouse_left_btn);
        printf("\nRIGHT: %d", mouse_right_btn);
        getch();
        c = getch();
    }

    disable_mouse();

    return 0;
}

#endif