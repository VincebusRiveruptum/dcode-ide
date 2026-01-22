
#include "TEST.H"

void test_tui(){
    return;
}

void tg_t_initTests(){
    printf("This is just a blank test\n");

    //tg_drawRectangle(0, 0, TUI_COLS - 1, TUI_ROWS - 1, T_COLOR_BLUE, T_COLOR_LIGHT_BLUE, '°', false);
    tg_fill(T_COLOR_BLUE, T_COLOR_LIGHT_BLUE, '°');
    //tg_drawRectangle(3, 3, 20, 10, T_COLOR_CYAN, T_COLOR_BLACK, 219, T_COLOR_CYAN, false, TUI_DRAW_BORDER_SIMPLE);
    tg_drawRectangle(3, 3, 20, 10, T_COLOR_CYAN, T_COLOR_BLACK, 219, T_COLOR_CYAN, T_COLOR_BLACK, true, TUI_DRAW_BORDER_SIMPLE);
}
