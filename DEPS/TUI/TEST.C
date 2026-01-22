
#include "TEST.H"

void test_tui(){
    return;
}

void tg_t_initTests(){
    printf("This is just a blank test\n");

    tg_fill(T_COLOR_BLUE, T_COLOR_LIGHT_BLUE, '°');
    
    tg_colorRectangles();
    
}

void tg_colorRectangles(){
    tg_drawRectangle(3, 3, 20, 10, T_COLOR_CYAN, T_COLOR_BLACK, 219, T_COLOR_CYAN, T_COLOR_BLACK, true, TUI_DRAW_BORDER_SIMPLE);
    tg_drawRectangle(30, 7, 50, 20, T_COLOR_MAGENTA, T_COLOR_BLACK, 219, T_COLOR_MAGENTA, T_COLOR_BLACK, true, TUI_DRAW_BORDER_SIMPLE);
    tg_drawRectangle(23, 23, 40, 40, T_COLOR_RED, T_COLOR_BLACK, 219, T_COLOR_RED, T_COLOR_BLACK, true, TUI_DRAW_BORDER_SIMPLE);
    tg_drawRectangle(60, 20, 70, 30, T_COLOR_LIGHT_YELLOW, T_COLOR_BLACK, 219, T_COLOR_BROWN, T_COLOR_BLACK, true, TUI_DRAW_BORDER_SIMPLE);

    tg_writeBuffer("Hello World", 5, 6, 20, 10, T_COLOR_WHITE, T_COLOR_BLACK);
    tg_writeBuffer("John D. Carmack II[1] (born August 21,[a] 1970)[1] is an American computer programmer and video game developer. He co-founded the video game company id Software and was the lead programmer of its 1990s games Commander Keen, Wolfenstein 3D, Doom, Quake, and their sequels. Carmack made innovations in 3D computer graphics, such as his Carmack's Reverse algorithm for shadow volumes.", 32, 12, 50, 18, T_COLOR_WHITE, T_COLOR_BLACK);
}
