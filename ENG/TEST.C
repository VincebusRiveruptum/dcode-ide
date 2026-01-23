
#include "TEST.H"

void _colorRectangles(){
    dw_rectangle(3, 3, 20, 10, ED_COLOR_CYAN, ED_COLOR_BLACK, 219, ED_COLOR_CYAN, ED_COLOR_BLACK, true, DRAW_BORDER_SIMPLE);
    dw_rectangle(30, 7, 50, 20, ED_COLOR_MAGENTA, ED_COLOR_BLACK, 219, ED_COLOR_MAGENTA, ED_COLOR_BLACK, true, DRAW_BORDER_SIMPLE);
    dw_rectangle(23, 23, 40, 40, ED_COLOR_RED, ED_COLOR_BLACK, 219, ED_COLOR_RED, ED_COLOR_BLACK, true, DRAW_BORDER_SIMPLE);
    dw_rectangle(60, 20, 70, 30, ED_COLOR_LIGHT_YELLOW, ED_COLOR_BLACK, 219, ED_COLOR_BROWN, ED_COLOR_BLACK, true, DRAW_BORDER_SIMPLE);

    dw_writeBuffer("Hello World", 5, 6, 20, 10, ED_COLOR_WHITE, ED_COLOR_BLACK);
    dw_writeBuffer("John D. Carmack II[1] (born August 21,[a] 1970)[1] is an American computer programmer and video game developer. He co-founded the video game company id Software and was the lead programmer of its 1990s games Commander Keen, Wolfenstein 3D, Doom, Quake, and their sequels. Carmack made innovations in 3D computer graphics, such as his Carmack's Reverse algorithm for shadow volumes.", 32, 12, 50, 18, ED_COLOR_WHITE, ED_COLOR_BLACK);
}

/* ===================================================================*/

void t_initTests(){
    printf("This is just a blank test\n");

    dw_fill(ED_COLOR_BLUE, ED_COLOR_LIGHT_BLUE, '°');
    
    _colorRectangles();
    
}

