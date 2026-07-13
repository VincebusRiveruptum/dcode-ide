#include "editor.h"

void ed_statusBarMessage(const char *format,  ...){
    va_list args;

    time(&ed_globalAuxTimer);

    memset(statusBarMessage, '\0', ED_STATUSBAR_WIDTH - 1);

    va_start(args, format);
    vsprintf(statusBarMessage, format, args);
    va_end(args);

	dw_requestRenderEvent(DW_RENDER_STATUSBAR);

    return;
}

bool ed_checkStatusBarMessage(){
    static time_t endClock;

    if(statusBarMessage[0] == '\0') return false;
    if(ed_globalAuxTimer == 0) return false;

    time(&endClock);

    // 5 seconds of duration
    if(difftime(endClock, ed_globalAuxTimer) > 5){
        memset(statusBarMessage, '\0', ED_STATUSBAR_WIDTH - 1);
        ed_globalAuxTimer = 0;
        dw_requestRenderEvent(DW_RENDER_STATUSBAR);
        return false;
    }  

    return true;
}

// Statusbar drawing function
void ed_statusBar(){
	static File *currentFile = NULL;

	if(!currentWindow || !currentWindow->currentFile)
		return;

	currentFile = currentWindow->currentFile;

	if(ed_checkStatusBarMessage() == true){
		dw_writeBuffer(
			textmemptr, 
			"%s", 
			0, 
			VIDEO_ROWS - 1, 
			VIDEO_COLS - 1, 
			VIDEO_ROWS - 1, 
			settings.STATUSBAR_COLOR_TEXT,
			settings.STATUSBAR_COLOR_BG, 
			statusBarMessage
		);            
	}else if (currentFile) {
		dw_writeBuffer(
			textmemptr, 
			"Line %d, Col %d %c", 
			0, 
			VIDEO_ROWS - 1, 
			39, 
			VIDEO_ROWS - 1, 
			settings.STATUSBAR_COLOR_TEXT,
			settings.STATUSBAR_COLOR_BG, 
			currentFile->cursorLine + 1, 
			currentFile->cursorCol + 1, 
			179, 
			currentFile->currentLine->length
		);

		dw_writeBuffer(
			textmemptr, 
			" %s", 
			40, 
			VIDEO_ROWS - 1, 
			VIDEO_COLS - 1, 
			VIDEO_ROWS - 1, 
			settings.STATUSBAR_COLOR_TEXT,
			settings.STATUSBAR_COLOR_BG, 
			currentFile->name
		);
	}else{
		dw_writeBuffer(
			textmemptr, 
			"No files opened", 
			0, 
			VIDEO_ROWS - 1, 
			VIDEO_COLS - 1,
			VIDEO_ROWS - 1, 
			settings.STATUSBAR_COLOR_TEXT,
			settings.STATUSBAR_COLOR_BG
		);
	}        
}
