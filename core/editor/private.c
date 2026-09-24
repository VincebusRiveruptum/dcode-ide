#include "editor.h"

int _get_tab_counts_until(int col){
    int i = 0;
    int tabCount = 0;
	TextArea *textArea = NULL;
    if (
		!currentWindow ||
		!currentWindow->textArea ||
		!currentWindow->textArea->currentLine
	) return -1;

	textArea = currentWindow->textArea;

    while(i < col && i < (int)textArea->currentLine->length){
        if(textArea->currentLine->buffer[i] == CHAR_TAB) 
			tabCount++;
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
	TextArea *textArea = NULL;

	textArea = currentWindow->textArea;
    travelingBackwards = textArea->currentLineNode;

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
    TextArea *textArea = NULL;
	
	textArea = currentWindow->textArea;

    do{
        c = textArea->currentLine->buffer[i];

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
    TextArea *textArea = NULL;
	textArea = currentWindow->textArea;

    do{
        c = textArea->currentLine->buffer[i];

        if (c == CHAR_TAB){
            tabCount++;

            cnext = textArea->currentLine->buffer[i + 1];

            if(cnext == '\0' && cnext != CHAR_TAB){
                return tabCount;
            }
        }
        i++;
    }while (c != '\0');

    return tabCount;
}

void _updateCurrentCursorY(){
	TextArea *textArea = NULL;

	textArea = currentWindow->textArea;
	
    if (!textArea || !currentWindow) return;

     // If the cursor is closer to the bottom
    if(currentCursorY <= currentWindow->y) currentCursorY = currentWindow->y;
    
    if( textArea->cursorLine - textArea->scrollY >= 0){
        currentCursorY = textArea->cursorLine - textArea->scrollY + currentWindow->y;
    }

    if(currentCursorY >= currentWindow->y + currentWindow->height) {
        currentCursorY = currentWindow->y + currentWindow->height;
    }
}

void _updateCurrentCursorX(){
    int visualCursor = 0;
    int visualScroll = 0;
	TextArea *textArea = NULL;
    
	textArea = currentWindow->textArea;

    if (!textArea || !currentWindow) return;

    visualCursor = _calculateVisualOffset(textArea->cursorCol);
    visualScroll = _calculateVisualOffset(textArea->scrollX);

    currentCursorX = (visualCursor - visualScroll) + LINE_COUNTER_WIDTH + currentWindow->x;

    // Boundary check to keep cursor on active window split
    if(currentCursorX < currentWindow->x + LINE_COUNTER_WIDTH) {
        currentCursorX = currentWindow->x + LINE_COUNTER_WIDTH;
    }

    if(currentCursorX >= currentWindow->x + currentWindow->width) {
        currentCursorX = currentWindow->x + currentWindow->width - 1;
    }

    textArea->prevChar = 
        textArea->cursorCol > 0 
        ? textArea->currentLine->buffer[textArea->cursorCol - 1]
        : 0;

    textArea->currentChar = 
        textArea->cursorCol > 0 
        ? textArea->currentLine->buffer[textArea->cursorCol]
        : 0;

    textArea->nextChar = 
        textArea->cursorCol < textArea->currentLine->length
        ? textArea->currentLine->buffer[textArea->cursorCol + 1]
        : 0;
}
// This gets the pointer of the start offset of the currentWindow's
// textArea's  current Line relative to the screen buffer ptr (or any)
unsigned short * _getCurrentLinePtrInBuffer(
    unsigned short *ptr, 
    EditorWindow *currentWindow
){
    unsigned short x, y;
    unsigned short abs_line = 0;
    unsigned short cursorLine = 0;
    unsigned short scrollY = 0;
    unsigned short winX = 0;
    unsigned short winY = 0;

    if(
        !currentWindow ||
        !currentWindow->textArea
    ) return NULL;

    if(!ptr)
        return NULL;

    cursorLine = currentWindow->textArea->cursorLine;

    scrollY = currentWindow->textArea->scrollY;

    winX = currentWindow->x;
    winY = currentWindow->y;

    abs_line = cursorLine - scrollY;

    x = winX;
    y = abs_line + winY;

    if(
        x >= VIDEO_COLS ||
        y >= VIDEO_ROWS
    ) return NULL;

    return &ptr[(y * VIDEO_COLS) + x + LINE_COUNTER_WIDTH ];

}

void _calculateSelectedLineStartEnd(
    EditorWindow *currentWindow, 
    unsigned short *selectedStartX, 
    unsigned short *selectedEndX,
    int *step
){
    unsigned short winWidth;
    unsigned short lineWidth;
    TextArea *textArea;

    if (
        !currentWindow ||
        !currentWindow->textArea ||
        !currentWindow->textArea->file ||
        !currentWindow->textArea->currentLine
    )
        return;

    textArea = currentWindow->textArea;
    winWidth = currentWindow->width;
    lineWidth = textArea->currentLine->length;

    *step = 
        (textArea->selectedStartX < textArea->selectedEndX)
        ? 1
        : -1;

    if(lineWidth > winWidth)
        lineWidth = winWidth;

    *selectedStartX = 
        textArea->selectedStartX > lineWidth
        ? lineWidth
        : textArea->selectedStartX;

    *selectedEndX =
        textArea->selectedEndX > lineWidth
        ? lineWidth
        : textArea->selectedEndX;
        
    return;
}


Line *_createLine(File *file){
	Line *newLine = NULL;

	if(!file || !file->arena){
		logger("[_createLine]: Invalid file/arena");
        exit(1);
	}

	newLine = (Line*)mem_arena_alloc(file->arena, sizeof(Line));

	if (!newLine){
		logger("[_createLine]: Could not make new line");
		exit(1);
	}

	newLine->length = 0;
	newLine->buffer = 
		(char*)mem_arena_alloc(
			file->arena,
			sizeof(char) * MAX_FILE_LINE_LENGTH + 1
		);

	if (!newLine->buffer){
		logger("[_createLine]: Could not alloc new line buffer");
        exit(1);
	}

	memset(newLine->buffer, '\0', MAX_FILE_LINE_LENGTH + 1);
	return newLine;
}

Node *_createLineNode(File *file){
	Node *newLineNode = NULL;

	if(!file || !file->arena){
		logger("[_createLine]: Invalid file/arena");
        exit(1);
	}

	newLineNode = (Node *)mem_arena_alloc(file->arena, sizeof(Node));
	
	if (!newLineNode){
		logger("[_createLineNode]: Could not alloc new line NODE");
        exit(1);
	}
	
	newLineNode->data = (Line*) _createLine(file);

	return newLineNode;
}

// This is for reusing a deleted line or creating a new one..
Node *_resolveNewLine(File *file){
	Node *newLineNode = NULL;
	Line *newLine = NULL;

	// Get last deleted line
	newLineNode = pop(&file->deletedLines);
    
    if(!newLineNode){
        logger("[_newLine]: Creating new line");
        newLineNode = _createLineNode(file);
    }else{
        logger("[_newLine]: reusing deleted line");
    }

	newLine = (Line*)newLineNode->data;

	if(!newLine){
		logger("[_newLine]: invalid deleted line data, now attemping to create new line.");
		newLineNode->data = (void*)_createLine(file);
		
		if(!newLineNode->data){
			logger("[_newLine]: Attempt failed. Save and restart dcode.");
			exit(1);
		}

        newLine = (Line *)newLineNode->data;
	}
    
	memset(newLine->buffer, '\0', MAX_FILE_LINE_LENGTH + 1);
	newLine->length = 0;
	newLineNode->isDeleted = false; 
    
	if(!newLineNode){
        logger("[_newLine]: Error creating/reusing line.");
	    exit(1);
	}

	return newLineNode;
}
