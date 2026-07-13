#include "editor.h"

int _get_tab_counts_until(int col){
    int i = 0;
    int tabCount = 0;
	File *currentFile = NULL;
    if (
		!currentWindow->currentFile ||
		!currentWindow->currentFile->currentLine
	) return 0;

	currentFile = currentWindow->currentFile;

    while(i < col && i < (int)currentFile->currentLine->length){
        if(currentFile->currentLine->buffer[i] == CHAR_TAB) tabCount++;
        i++; 
    }
    
    return tabCount;
}

int _get_tab_counts_someline(Line *someLine, int col){
    int i = 0;
    int tabCount = 0;

    if (!someLine) return 0;

    while(i < col && i < (int)someLine->length){
        if(someLine->buffer[i] == CHAR_TAB) tabCount++;
        i++; 
    }
    
    return tabCount;
}

int _get_auto_close_pos(){
    Node *travelingBackwards = NULL;
	File *currentFile = NULL;

	currentFile = currentWindow->currentFile;
    travelingBackwards = currentFile->currentLineNode;

    if(!travelingBackwards) return 0;
    
    while(travelingBackwards != NULL){
        if( travelingBackwards->data &&
            ((Line*)travelingBackwards->data)->buffer &&
            ((Line*)travelingBackwards->data)->length){
                
            if(
				((Line*)travelingBackwards->data)->buffer[
					((Line *)travelingBackwards->data)->length-1
				] == '{'
			){
                return _get_tab_counts_someline((
					Line*)travelingBackwards->data, 
					((Line *)travelingBackwards->data)->length
				);
            }
        }

        travelingBackwards = travelingBackwards->prev;
    }

    return 0;
}

int _calculateVisualOffset(int col){
    return col + (_get_tab_counts_until(col) * 3);
}
// Calculate current line number of tabs
int _calculateTabCount(){
    unsigned int i = 0, tabCount = 0;
	char c;
    File *currentFile = NULL;
	
	currentFile = currentWindow->currentFile;

    do{
        c = currentFile->currentLine->buffer[i];

        if(c == CHAR_TAB) tabCount++;

        i++;
    }while ( c != '\0');

    return tabCount;
}

int _calculateTabStart(){
    int i = 0;
    int tabCount = 0;
    char c = '\0';
    char cnext = '\0';
    File *currentFile = NULL;
	currentFile = currentWindow->currentFile;

    do{
        c = currentFile->currentLine->buffer[i];

        if (c == CHAR_TAB){
            tabCount++;

            cnext = currentFile->currentLine->buffer[i + 1];

            if(cnext == '\0' && cnext != CHAR_TAB){
                return tabCount;
            }
        }
        i++;
    }while (c != '\0');

    return tabCount;
}

void _updateCurrentCursorY(){
	File *currentFile = NULL;

	currentFile = currentWindow->currentFile;
	
    if (!currentFile || !currentWindow) return;

     // If the cursor is closer to the bottom
    if(currentCursorY <= currentWindow->y) currentCursorY = currentWindow->y;
    
    if( currentFile->cursorLine - currentFile->scrollY >= 0){
        currentCursorY = currentFile->cursorLine - currentFile->scrollY + currentWindow->y;
    }

    if(currentCursorY >= currentWindow->y + currentWindow->height) {
        currentCursorY = currentWindow->y + currentWindow->height;
    }
}

void _updateCurrentCursorX(){
    int visualCursor = 0;
    int visualScroll = 0;
	File *currentFile = NULL;
    
	currentFile = currentWindow->currentFile;

    if (!currentFile || !currentWindow) return;

    visualCursor = _calculateVisualOffset(currentFile->cursorCol);
    visualScroll = _calculateVisualOffset(currentFile->scrollX);

    currentCursorX = (visualCursor - visualScroll) + LINE_COUNTER_WIDTH + currentWindow->x;

    // Boundary check to keep cursor on active window split
    if(currentCursorX < currentWindow->x + LINE_COUNTER_WIDTH) {
        currentCursorX = currentWindow->x + LINE_COUNTER_WIDTH;
    }

    if(currentCursorX >= currentWindow->x + currentWindow->width) {
        currentCursorX = currentWindow->x + currentWindow->width - 1;
    }

    currentFile->prevChar = 
        currentFile->cursorCol > 0 
        ? currentFile->currentLine->buffer[currentFile->cursorCol - 1]
        : 0;

    currentFile->currentChar = 
        currentFile->cursorCol > 0 
        ? currentFile->currentLine->buffer[currentFile->cursorCol]
        : 0;

    currentFile->nextChar = 
        currentFile->cursorCol < currentFile->currentLine->length
        ? currentFile->currentLine->buffer[currentFile->cursorCol + 1]
        : 0;
}