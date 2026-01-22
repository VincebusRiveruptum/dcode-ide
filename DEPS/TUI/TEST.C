
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
}
