
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

    logger("[t_scanfTest]: Scanned value : %s", output);
}

void t_drawDebugger(){
    dw_writeBuffer(textmemptr, "scrollY: %d", 65, 0, VIDEO_COLS - 1, 0, ed_statusbarFgColor, ed_statusbarBgColor, currentFileArena->file->scrollY );
    dw_writeBuffer(textmemptr, "scrollX: %d", 65, 1, VIDEO_COLS - 1, 1,ed_statusbarFgColor, ed_statusbarBgColor, currentFileArena->file->scrollX );
    dw_writeBuffer(textmemptr, "cursorLine: %d", 65, 2, VIDEO_COLS - 1, 2, ed_statusbarFgColor, ed_statusbarBgColor,currentFileArena->file->cursorLine );
    dw_writeBuffer(textmemptr, "cursorCol: %d", 65, 3, VIDEO_COLS - 1, 3,ed_statusbarFgColor, ed_statusbarBgColor, currentFileArena->file->cursorCol );
    dw_writeBuffer(textmemptr, "existing lines: %d", 60, 4, VIDEO_COLS - 1, 4,ed_statusbarFgColor, ed_statusbarBgColor, currentFileArena->file->lines->length );
    dw_writeBuffer(textmemptr, "deleted lines: %d", 60, 5, VIDEO_COLS - 1, 5,ed_statusbarFgColor, ed_statusbarBgColor, currentFileArena->file->deletedLines->length );
    dw_writeBuffer(textmemptr, "currentCursorX: %d", 60, 6, VIDEO_COLS - 1, 6, ed_statusbarFgColor, ed_statusbarBgColor, currentCursorX );
    dw_writeBuffer(textmemptr, "currentCursorY: %d", 60, 7, VIDEO_COLS - 1, 7, ed_statusbarFgColor, ed_statusbarBgColor, currentCursorY );
}

/* ===================================================================*/

void t_initTests(){
    printf("This is just a blank test\n");

    //dw_fill(textmemptr, COLOR_BLUE, COLOR_LIGHT_BLUE, '°');
    
    //_colorRectangles();

    //_filesTest();

    //_scanfTest();


    
}

