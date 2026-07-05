
#include "test.h"

void _colorRectangles(){
    dw_rectangle(
		textmemptr, 
		3, 
		3, 
		20, 
		10, 
		COLOR_CYAN, 
		COLOR_BLACK, 
		219, 
		COLOR_CYAN, 
		COLOR_BLACK, 
		true, 
		DRAW_BORDER_SIMPLE, 
		NULL
	);
	
    dw_rectangle(
		textmemptr, 
		30, 
		7, 
		50, 
		20, 
		COLOR_MAGENTA, 
		COLOR_BLACK, 
		219, 
		COLOR_MAGENTA, 
		COLOR_BLACK, 
		true, 
		DRAW_BORDER_SIMPLE, 
		NULL
	);

    dw_rectangle(
		textmemptr, 
		23, 
		23, 
		40,
		40, 
		COLOR_RED, 
		COLOR_BLACK, 
		219, 
		COLOR_RED, 
		COLOR_BLACK, 
		true, 
		DRAW_BORDER_SIMPLE, 
		NULL
	);
    
	dw_rectangle(
		textmemptr, 
		60, 
		20, 
		70, 
		30, 
		COLOR_LIGHT_YELLOW, 
		COLOR_BLACK, 
		219, 
		COLOR_BROWN, 
		COLOR_BLACK, 
		true, 
		DRAW_BORDER_SIMPLE, 
		NULL
	);

    dw_writeBuffer(
		textmemptr, 
		"Hello World", 
		5, 
		6,
		20, 
		10, 
		COLOR_WHITE, 
		COLOR_BLACK
	);

    dw_writeBuffer(
		textmemptr, 
		"John D. Carmack II[1] (born August 21,[a] 1970)[1] is an American computer programmer and video game developer. He co-founded the video game company id Software and was the lead programmer of its 1990s games Commander Keen, Wolfenstein 3D, Doom, Quake, and their sequels. Carmack made innovations in 3D computer graphics, such as his Carmack's Reverse algorithm for shadow volumes.", 32, 12, 50, 18, COLOR_WHITE, COLOR_BLACK);
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

    dw_writeBuffer(
		textmemptr, 
		"scrollY: %d", 
		65, 
		0, 
		VIDEO_COLS - 1,
		0, 
		settings.STATUSBAR_COLOR_TEXT, 
		settings.STATUSBAR_COLOR_BG, 
		currentWindow->currentFile->scrollY 
	);

	// TODO: FOrmat arguments each one on its own line...
	
    dw_writeBuffer(textmemptr, "scrollX: %d", 65, 1, VIDEO_COLS - 1, 1,settings.STATUSBAR_COLOR_TEXT, settings.STATUSBAR_COLOR_BG, currentWindow->currentFile->scrollX );
    dw_writeBuffer(textmemptr, "cursorLine: %d", 65, 2, VIDEO_COLS - 1, 2, settings.STATUSBAR_COLOR_TEXT, settings.STATUSBAR_COLOR_BG,currentWindow->currentFile->cursorLine );
    dw_writeBuffer(textmemptr, "cursorCol: %d", 65, 3, VIDEO_COLS - 1, 3,settings.STATUSBAR_COLOR_TEXT, settings.STATUSBAR_COLOR_BG, currentWindow->currentFile->cursorCol );
    dw_writeBuffer(textmemptr, "existing lines: %d", 60, 4, VIDEO_COLS - 1, 4,settings.STATUSBAR_COLOR_TEXT, settings.STATUSBAR_COLOR_BG, currentWindow->currentFile->lines->length );
    dw_writeBuffer(textmemptr, "deleted lines: %d", 60, 5, VIDEO_COLS - 1, 5,settings.STATUSBAR_COLOR_TEXT, settings.STATUSBAR_COLOR_BG, currentWindow->currentFile->deletedLines->length );
    dw_writeBuffer(textmemptr, "currentCursorX: %d", 60, 6, VIDEO_COLS - 1, 6, settings.STATUSBAR_COLOR_TEXT, settings.STATUSBAR_COLOR_BG, currentCursorX );
    dw_writeBuffer(textmemptr, "currentCursorY: %d", 60, 7, VIDEO_COLS - 1, 7, settings.STATUSBAR_COLOR_TEXT, settings.STATUSBAR_COLOR_BG, currentCursorY );
    dw_writeBuffer(textmemptr, "line length: %d", 60, 8, VIDEO_COLS - 1, 8, settings.STATUSBAR_COLOR_TEXT, settings.STATUSBAR_COLOR_BG, currentWindow->currentFile->currentLine->length);
    dw_writeBuffer(textmemptr, "selectedStartX: %d", 60, 9, VIDEO_COLS - 1, 9, settings.STATUSBAR_COLOR_TEXT, settings.STATUSBAR_COLOR_BG, currentWindow->currentFile->selectedStartX);
    dw_writeBuffer(textmemptr, "selectedEndX: %d", 60, 10, VIDEO_COLS - 1, 10, settings.STATUSBAR_COLOR_TEXT, settings.STATUSBAR_COLOR_BG, currentWindow->currentFile->selectedEndX);
    dw_writeBuffer(textmemptr, "selectedStartLine: %d", 60, 11, VIDEO_COLS - 1, 11, settings.STATUSBAR_COLOR_TEXT, settings.STATUSBAR_COLOR_BG, currentWindow->currentFile->selectedStartLine);
    dw_writeBuffer(textmemptr, "selectedEndLine: %d", 60, 12, VIDEO_COLS - 1, 12, settings.STATUSBAR_COLOR_TEXT, settings.STATUSBAR_COLOR_BG, currentWindow->currentFile->selectedEndLine);
    dw_writeBuffer(textmemptr, "oldCol: %d", 60, 13, VIDEO_COLS - 1, 13, settings.STATUSBAR_COLOR_TEXT, settings.STATUSBAR_COLOR_BG, currentWindow->currentFile->oldCol);
    dw_writeBuffer(textmemptr, "oldLine: %d", 60, 14, VIDEO_COLS - 1, 14, settings.STATUSBAR_COLOR_TEXT, settings.STATUSBAR_COLOR_BG, currentWindow->currentFile->oldLine);
    dw_writeBuffer(textmemptr, "isActive: %d", 60, 15, VIDEO_COLS - 1, 15, settings.STATUSBAR_COLOR_TEXT, settings.STATUSBAR_COLOR_BG, currentWindow->currentFile->isActive);
    dw_writeBuffer(textmemptr, "currentWindow->x: %d", 55, 16, VIDEO_COLS - 1, 16, settings.STATUSBAR_COLOR_TEXT, settings.STATUSBAR_COLOR_BG, currentWindow->x);
    dw_writeBuffer(textmemptr, "currentWindow->y: %d", 55, 17, VIDEO_COLS - 1, 17, settings.STATUSBAR_COLOR_TEXT, settings.STATUSBAR_COLOR_BG, currentWindow->y);
    dw_writeBuffer(textmemptr, "currentWindow->height: %d", 55, 18, VIDEO_COLS - 1, 18, settings.STATUSBAR_COLOR_TEXT, settings.STATUSBAR_COLOR_BG, currentWindow->height);
    dw_writeBuffer(textmemptr, "currentWindow->width: %d", 55, 19, VIDEO_COLS - 1, 19, settings.STATUSBAR_COLOR_TEXT, settings.STATUSBAR_COLOR_BG, currentWindow->width);
    dw_writeBuffer(textmemptr, "currentWindow files: %d", 55, 20, VIDEO_COLS - 1, 20, settings.STATUSBAR_COLOR_TEXT, settings.STATUSBAR_COLOR_BG, currentWindow->fileList->length);
}

/* ===================================================================*/

void t_initTests(){
    printf("This is just a blank test\n");

    //dw_fill(textmemptr, COLOR_BLUE, COLOR_LIGHT_BLUE, '°');
    
    //_colorRectangles();

    //_filesTest();

    //_scanfTest();


    
}

