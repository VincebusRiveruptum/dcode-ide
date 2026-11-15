#include "mouse.h"

unsigned int mouse_x = 0;
unsigned int mouse_y = 0;
unsigned int mouse_left_btn = 0;
unsigned int mouse_right_btn = 0;

unsigned char check_mouse(){
    return 1;
}

void enable_mouse(){
    return;
}

void disable_mouse(){
    return;
}

void read_mouse(){
    return;
}

/* TESTING =================================*/
#ifdef STANDALONE
int main(){
    char c;
    printf("\nTesting mouse, move cursor or press any key to return.\n");

    enable_mouse();

    while(c != 1){
        read_mouse();

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