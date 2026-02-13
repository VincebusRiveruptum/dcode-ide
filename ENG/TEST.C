
#include "TEST.H"

void _colorRectangles(){
    dw_rectangle(textmemptr, 3, 3, 20, 10, COLOR_CYAN, COLOR_BLACK, 219, COLOR_CYAN, COLOR_BLACK, true, DRAW_BORDER_SIMPLE);
    dw_rectangle(textmemptr, 30, 7, 50, 20, COLOR_MAGENTA, COLOR_BLACK, 219, COLOR_MAGENTA, COLOR_BLACK, true, DRAW_BORDER_SIMPLE);
    dw_rectangle(textmemptr, 23, 23, 40, 40, COLOR_RED, COLOR_BLACK, 219, COLOR_RED, COLOR_BLACK, true, DRAW_BORDER_SIMPLE);
    dw_rectangle(textmemptr, 60, 20, 70, 30, COLOR_LIGHT_YELLOW, COLOR_BLACK, 219, COLOR_BROWN, COLOR_BLACK, true, DRAW_BORDER_SIMPLE);

    dw_writeBuffer(textmemptr, "Hello World", 5, 6, 20, 10, COLOR_WHITE, COLOR_BLACK);
    dw_writeBuffer(textmemptr, "John D. Carmack II[1] (born August 21,[a] 1970)[1] is an American computer programmer and video game developer. He co-founded the video game company id Software and was the lead programmer of its 1990s games Commander Keen, Wolfenstein 3D, Doom, Quake, and their sequels. Carmack made innovations in 3D computer graphics, such as his Carmack's Reverse algorithm for shadow volumes.", 32, 12, 50, 18, COLOR_WHITE, COLOR_BLACK);
}

void _filesTest(){
    f_openFile("..\\DEPS\\DATA\\DATA.C");  
}

void _scanfTest(){
    char *output;

    output = ed_scanf(1,1,20);

    logger("[t_scanfTest]: SCANF %s", output);
}

/* ===================================================================*/

void t_initTests(){
    printf("This is just a blank test\n");

    //dw_fill(textmemptr, COLOR_BLUE, COLOR_LIGHT_BLUE, '°');
    
    //_colorRectangles();

    //_filesTest();

    _scanfTest();


    
}

