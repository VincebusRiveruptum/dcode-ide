#include <main.h>

int main(int argc, char *argv[]) {
    bool endProgram = false;
    char c;
    
    init_video();
    initKeyboard();

    while(endProgram == false){
        if(keyboardTable[KEY_ESC] == true) endProgram = true;

        if(kbhit()){
            c = getch();
            putch(c);
        }
    }
    
    closeKeyboard();

    printf("\n96 tears...\n");
    return 0;
}