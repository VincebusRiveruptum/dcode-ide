
#include "test.h"

void t_drawDebugger(){

    dw_writeBuffer(
		textmemptr, 
		"scrollY: %d", 
		VIDEO_COLS - 15, 
		0, 
		VIDEO_COLS - 1,
		0, 
		settings.STATUSBAR_COLOR_TEXT, 
		settings.STATUSBAR_COLOR_BG, 
		currentWindow->currentFile->scrollY 
	);

	// TODO: FOrmat arguments each one on its own line...
	
    dw_writeBuffer(textmemptr, "scrollX: %d", VIDEO_COLS - 15, 1, VIDEO_COLS - 1, 1,settings.STATUSBAR_COLOR_TEXT, settings.STATUSBAR_COLOR_BG, currentWindow->currentFile->scrollX );
    dw_writeBuffer(textmemptr, "cursorLine: %d", VIDEO_COLS - 15, 2, VIDEO_COLS - 1, 2, settings.STATUSBAR_COLOR_TEXT, settings.STATUSBAR_COLOR_BG,currentWindow->currentFile->cursorLine );
    dw_writeBuffer(textmemptr, "cursorCol: %d", VIDEO_COLS - 15, 3, VIDEO_COLS - 1, 3,settings.STATUSBAR_COLOR_TEXT, settings.STATUSBAR_COLOR_BG, currentWindow->currentFile->cursorCol );
    dw_writeBuffer(textmemptr, "Existing lines: %d", VIDEO_COLS - 20, 4, VIDEO_COLS - 1, 4,settings.STATUSBAR_COLOR_TEXT, settings.STATUSBAR_COLOR_BG, currentWindow->currentFile->lines->length );
    dw_writeBuffer(textmemptr, "Deleted lines: %d", VIDEO_COLS - 20, 5, VIDEO_COLS - 1, 5,settings.STATUSBAR_COLOR_TEXT, settings.STATUSBAR_COLOR_BG, currentWindow->currentFile->deletedLines->length );
    dw_writeBuffer(textmemptr, "currentCursorX: %d", VIDEO_COLS - 20, 6, VIDEO_COLS - 1, 6, settings.STATUSBAR_COLOR_TEXT, settings.STATUSBAR_COLOR_BG, currentCursorX );
    dw_writeBuffer(textmemptr, "currentCursorY: %d", VIDEO_COLS - 20, 7, VIDEO_COLS - 1, 7, settings.STATUSBAR_COLOR_TEXT, settings.STATUSBAR_COLOR_BG, currentCursorY );
    dw_writeBuffer(textmemptr, "Line length: %d", VIDEO_COLS - 20, 8, VIDEO_COLS - 1, 8, settings.STATUSBAR_COLOR_TEXT, settings.STATUSBAR_COLOR_BG, currentWindow->currentFile->currentLine->length);
    dw_writeBuffer(textmemptr, "selectedStartX: %d", VIDEO_COLS - 20, 9, VIDEO_COLS - 1, 9, settings.STATUSBAR_COLOR_TEXT, settings.STATUSBAR_COLOR_BG, currentWindow->currentFile->selectedStartX);
    dw_writeBuffer(textmemptr, "selectedEndX: %d", VIDEO_COLS - 20, 10, VIDEO_COLS - 1, 10, settings.STATUSBAR_COLOR_TEXT, settings.STATUSBAR_COLOR_BG, currentWindow->currentFile->selectedEndX);
    dw_writeBuffer(textmemptr, "selectedStartLine: %d", VIDEO_COLS - 20, 11, VIDEO_COLS - 1, 11, settings.STATUSBAR_COLOR_TEXT, settings.STATUSBAR_COLOR_BG, currentWindow->currentFile->selectedStartLine);
    dw_writeBuffer(textmemptr, "selectedEndLine: %d", VIDEO_COLS - 20, 12, VIDEO_COLS - 1, 12, settings.STATUSBAR_COLOR_TEXT, settings.STATUSBAR_COLOR_BG, currentWindow->currentFile->selectedEndLine);
    dw_writeBuffer(textmemptr, "oldCol: %d", VIDEO_COLS - 15, 13, VIDEO_COLS - 1, 13, settings.STATUSBAR_COLOR_TEXT, settings.STATUSBAR_COLOR_BG, currentWindow->currentFile->oldCol);
    dw_writeBuffer(textmemptr, "oldLine: %d", VIDEO_COLS - 15, 14, VIDEO_COLS - 1, 14, settings.STATUSBAR_COLOR_TEXT, settings.STATUSBAR_COLOR_BG, currentWindow->currentFile->oldLine);
    dw_writeBuffer(textmemptr, "isActive: %d", VIDEO_COLS - 15, 15, VIDEO_COLS - 1, 15, settings.STATUSBAR_COLOR_TEXT, settings.STATUSBAR_COLOR_BG, currentWindow->currentFile->isActive);
    dw_writeBuffer(textmemptr, "currentWindow->x: %d", VIDEO_COLS - 25, 16, VIDEO_COLS - 1, 16, settings.STATUSBAR_COLOR_TEXT, settings.STATUSBAR_COLOR_BG, currentWindow->x);
    dw_writeBuffer(textmemptr, "currentWindow->y: %d", VIDEO_COLS - 25, 17, VIDEO_COLS - 1, 17, settings.STATUSBAR_COLOR_TEXT, settings.STATUSBAR_COLOR_BG, currentWindow->y);
    dw_writeBuffer(textmemptr, "currentWindow->height: %d", VIDEO_COLS - 30, 18, VIDEO_COLS - 1, 18, settings.STATUSBAR_COLOR_TEXT, settings.STATUSBAR_COLOR_BG, currentWindow->height);
    dw_writeBuffer(textmemptr, "currentWindow->width: %d", VIDEO_COLS - 30, 19, VIDEO_COLS - 1, 19, settings.STATUSBAR_COLOR_TEXT, settings.STATUSBAR_COLOR_BG, currentWindow->width);
    dw_writeBuffer(textmemptr, "currentWindow files: %d", VIDEO_COLS - 25, 20, VIDEO_COLS - 1, 20, settings.STATUSBAR_COLOR_TEXT, settings.STATUSBAR_COLOR_BG, currentWindow->fileList->length);
}

/* ===================================================================*/

void t_initTests(){
    printf("This is just a blank test\n");

    //dw_fill(textmemptr, COLOR_BLUE, COLOR_LIGHT_BLUE, '°');
    
    //_colorRectangles();

    //_filesTest();

    //_scanfTest();


    
}

