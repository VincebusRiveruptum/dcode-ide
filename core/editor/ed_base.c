/*
    This is a basic TUI library, right now just for testing purposes
    By Vincebus Riveruptum
    2026

    Credits osdever.net

    http://www.osdever.net/bkerndev/Docs/printing.htm
*/

#include "editor.h"

//unsigned char attrib = 0x07; // Default attribute: White on Black
struct Container *root;

unsigned char VIDEO_COLS = 80;
unsigned char VIDEO_ROWS = 25;

unsigned char currentCursorX = 0;
unsigned char currentCursorY = 0;

bool on_selection_tool = false;
bool ed_onSearchTool = false;

time_t ed_globalAuxTimer = 0;
char statusBarMessage[ED_STATUSBAR_WIDTH] = {'\0'};

void ed_updateScrollY(){
    int displayHeight = 0;
	File *currentFile = NULL;
	
	currentFile = currentWindow->currentFile;

    if (!currentFile || !currentWindow) return;
	
    displayHeight = currentWindow->height;
	
    if((currentFile->cursorLine - currentFile->scrollY) > displayHeight) {
        currentFile->scrollY = currentFile->cursorLine - (displayHeight);
		
		dw_requestRenderEvent(DW_RENDER_WINDOW);
    }else if(currentFile->cursorLine <= currentFile->scrollY){
        currentFile->scrollY = currentFile->cursorLine;
		
		dw_requestRenderEvent(DW_RENDER_WINDOW);
    }
}

void ed_updateScrollX(){
    int visualCursor = 0;
    int visualScroll = 0;
    int displayWidth = 0;
	File *currentFile = NULL;
    
	currentFile = currentWindow->currentFile;

    if (!currentFile || !currentWindow) return;

    visualCursor = _calculateVisualOffset(currentFile->cursorCol);
    visualScroll = _calculateVisualOffset(currentFile->scrollX);
    displayWidth = currentWindow->width - LINE_COUNTER_WIDTH;

    // If cursor is to the left of the visible area
    if (visualCursor < visualScroll) {
        currentFile->scrollX = currentFile->cursorCol;
		dw_requestRenderEvent(DW_RENDER_WINDOW);
    } 
    // If cursor is to the right of the visible area
    else if (visualCursor >= visualScroll + displayWidth) {
        // We move scrollX forward until the cursor is visible
        while (
			_calculateVisualOffset(currentFile->scrollX)
			+ displayWidth 
			<= visualCursor
		) {
            currentFile->scrollX++;
			dw_requestRenderEvent(DW_RENDER_WINDOW);
        }
    }
}

void ed_updateCursor(){
	if (!currentWindow || !currentWindow->currentFile) return;

    _updateCurrentCursorY();
    _updateCurrentCursorX();

    ed_putCursor(currentCursorX, currentCursorY);
}



void ed_resetActity(){
    if(currentWindow->currentFile && currentWindow->currentFile)
        currentWindow->currentFile->isActive = false;
}

void ed_markActive(unsigned char activity){
    // Push activity to editor clipboard
    (void)activity;
    if(currentWindow->currentFile && currentWindow->currentFile)
        currentWindow->currentFile->isActive = true;
}

// =======================================================================
// Window refreshing

// This just renders the current line
void ed_renderCurrentLine(){
	// TODO : CAREFUL OF THIS ARRAY SIZE...
	int xpos = 0, ypos = 0;

	static char tempLineBuffer[1024] = {'\0'};
	
	if(
		!currentWindow ||
		!currentWindow->currentFile ||
		!currentWindow->currentFile->currentLine
	){
		logger("[ed_renderCurrentLine]: invalid data...");
		return;
	} 
	
	strcpy(
		tempLineBuffer, 
		(currentWindow->currentFile->currentLine->buffer
		+
		currentWindow->currentFile->scrollX)
	);

	xpos = 
		currentWindow->x + 
		LINE_COUNTER_WIDTH;

	ypos = 
		(
			currentWindow->currentFile->cursorLine - 
			currentWindow->currentFile->scrollY
		) +
		currentWindow->y; 


	dw_copyFormatted(
		textmemptr,
		xpos, 
		ypos,
		VIDEO_COLS,
		tempLineBuffer,
		currentWindow->currentFile
	);

    ed_updateCursor();
	return;
}

// This just renders the current window.
void ed_updateWindow(Workspace *workspace){
    Window *wnd = NULL;
	
    if (
		!workspace || 
		!workspace->windowList ||
		!workspace->currentWindow
	) return;

    //hal_vid_clearBuffer(editormemptr);

    wnd = workspace->currentWindow;
    if (!wnd->currentFile) return;

	dw_writeBufferEditorFormatted(
		//editormemptr, 
		textmemptr, 
		wnd->x, 
		wnd->y, 
		wnd->x + wnd->width, 
		wnd->y + wnd->height, 
		COLOR_LIGHT_GRAY, 
		COLOR_BLACK, 
		wnd->currentFile
	);

	return;
}

// This clears the screen and renders all windows.
void ed_renderWindows(Workspace *workspace){
	//int winLen = 0;
	Node *rec = NULL;
	Window *wnd = NULL;
	if(!workspace){
		logger("[r_refreshWindows]: Invalid workspace.");
		return;
	}
	
	if(!workspace->windowList){
		logger("[r_refreshWindows]: No windows.");
		return;
	}
	
	hal_vid_clearBuffer(textmemptr);

	rec = workspace->windowList->firstNode;
	//winLen = workspace->windowList->length;
	
	while(rec){
		wnd = (Window*)rec->data;

		if(wnd){
			wnd->height = VIDEO_ROWS - 2;

			// TODO :
			// Also, re calculate width and x start position relative to 
			// the screen width.
			
			// ALSO...
			// Recalculate cursor position if resoultion is lower than 
			// the previous one
		
			// We re-render the editor.
			if (wnd && wnd->currentFile) {
				dw_writeBufferEditorFormatted(
					//editormemptr, 
					textmemptr, 
					wnd->x, 
					wnd->y, 
					wnd->x + wnd->width, 
					wnd->y + wnd->height, 
					COLOR_LIGHT_GRAY, 
					COLOR_BLACK, 
					wnd->currentFile
				);
			}
		}

		rec = rec->next;
	}

	return;
}

// Makes a line duplicate
Line *ed_dupLine(Line *src, MemoryArena *arena){
    Line *newLine = NULL;

    if( !src || !arena){
        logger("[_dupLine]: Invalid src or arena.");
        return;
    }
     
    newLine = (Line*)mem_arena_allow(arena, sizeof(Line));
     
    if(!newLine){
        logger("[_dupLine]: Could not instantiate new line.");
        return;
    }

    newLine->length = src->length;
    newLine->buffer = (char*)mem_arena_alloc(arena, MAX_LINE_WIDTH);
    memcpy(newLine->buffer, src->buffer, MAX_LINE_WIDTH);

    return newLine;
}

/*
    This is the cursor behavior when is inside a TEXT AREA
*/
void ed_moveCursor(short x, short y){
    Node *tempNode = NULL;
	static File *currentFile = NULL;
    int tabCount = 0;
	currentFile = currentWindow->currentFile;

    if (!currentFile) return;

    // If the number of lines is less than the screen heightc
    // and the current Y cursor position is less than the nmber of lines
    // DANGEROUS

    if( 
        y > 0 && 
        (
            currentFile->cursorLine >= 
            currentFile->lines->length - 1
    	)
	)    return;

    // We check if we are in the current line
    if(! currentFile->currentLineNode){
        logger("[ed_moveCursor]: CurrentLineNode is NULL");
        return;
    }

    // VERTICAL SCROLLING ============================================
	//

    // If the cursor is at 0 and we want to scroll up
    if(y && currentFile->cursorLine + y >= 0){       
        // IF the cursor is moved by 1 step, then we move the data 
		// between nodes by one node
        if(
            y == -1 &&
            currentFile->currentLineNode->prev 
        ){
            tempNode = currentFile->currentLineNode;
            currentFile->currentLineNode = tempNode->prev;
            
            currentFile->cursorCol = 
                (currentFile->prevLine->length < currentFile->cursorCol) 
                ? currentFile->prevLine->length - 1
                : currentFile->cursorCol;

            currentFile->prevLine = 
                currentFile->currentLineNode->prev &&
                currentFile->currentLineNode->prev->data ?
                currentFile->currentLineNode->prev->data
                :
                NULL;
            currentFile->currentLine = currentFile->currentLineNode->data;
            currentFile->nextLine = 
                currentFile->currentLineNode->next &&
                currentFile->currentLineNode->next->data ?
                currentFile->currentLineNode->next->data
                :
                NULL;
        }

        if( 
            y == 1 &&
            currentFile->currentLineNode->next
        ){
            tempNode = currentFile->currentLineNode;
            currentFile->currentLineNode = tempNode->next;
            
            currentFile->cursorCol = 
                (currentFile->nextLine->length < currentFile->cursorCol) 
                ? currentFile->nextLine->length - 1
                : currentFile->cursorCol;

            currentFile->prevLine = 
                currentFile->currentLineNode->prev &&
                currentFile->currentLineNode->prev->data ?
                currentFile->currentLineNode->prev->data
                :
                NULL;
            currentFile->currentLine = currentFile->currentLineNode->data;
            currentFile->nextLine = 
                currentFile->currentLineNode->next &&
                currentFile->currentLineNode->next->data ?
                currentFile->currentLineNode->next->data
                :
                NULL;
        }
        
        currentFile->cursorLine += y;

        // Cursor col update 
        if (currentFile->currentLine->length < currentFile->cursorCol){
            currentFile->cursorCol = currentFile->currentLine->length;
        }

        tabCount = _get_tab_counts_until(currentFile->currentLine->length);

        // Only if the last character in the the line to
        // move is a tab, we get into the tab position 
        // after.
        if(
            tabCount > 0 &&
            tabCount == currentFile->currentLine->length
        )
            currentFile->cursorCol = tabCount;
    }
    // END VERTICAL SCROLLING =====================================================

    // HORIZ, SCROLLING =
	// ==========================================================
    if(x){
        if(currentFile->cursorCol + x <= 0){
            currentFile->cursorCol = 0;
        } else if(
            currentFile->cursorCol + x > 0 && 
            currentFile->cursorCol + x <= (int)currentFile->currentLine->length
        ){  
            currentFile->cursorCol +=x;

        }else if(currentFile->cursorCol + x >= MAX_FILE_LINE_LENGTH){
            currentFile->cursorCol = MAX_FILE_LINE_LENGTH - 1;
        }    
    }
    
    
    // END HORIZ, SCROLLING =====================================================

	ed_updateScrollX();
	ed_updateScrollY();
	
    dw_requestRenderEvent(DW_RENDER_CURSOR);
}

void ed_typeChar(char c){
    // We type the char a
    // X : currentCursorX + LINE_COUNTER_WIDTH + 1
    // Y : currentCursorY + file->scrollY + 1 
    int x = 0;
    int y = 0;
    int i = 0;
    Node *node = NULL;
    Line *line = NULL;
	static File *currentFile = NULL;

	currentFile = currentWindow->currentFile;

    if (!currentFile) return;

    x = currentFile->cursorCol;
    y = currentFile->cursorLine;
    
    if (x < 0) x = 0;
    if (y < 0) y = 0;
        
    node = currentFile->currentLineNode;
    
    if(!node) {
        logger("[ed_typeChar]: Node at y=%d is NULL", y);
        return;
    }

    line = (Line *)node->data;
    line->length++;
    
    // Had to use a loop instead of memcpy due to overlapping memory
    
    for(i = line->length; i > x; i--){
        line->buffer[i] = line->buffer[i-1];
    }
    
    line->buffer[x] = c;

    currentFile->cursorCol++;
    currentFile->isModified = true;
    
    ed_markActive(ED_ACTIVITY_TYPE);
    ed_updateScrollX();

    
    if(isspace(c)){
		dw_requestRenderEvent(DW_RENDER_WINDOW);
	}else{
		dw_requestRenderEvent(DW_RENDER_LINE);
	}
}

void ed_backspace(){
        // We type the char at 
    // X : currentCursorX + LINE_COUNTER_WIDTH + 1
    // Y : currentCursorY + file->scrollY + 1 
    int x = 0;
    int y = 0;
    Node *node = NULL;
    Node *prevNode = NULL;
    Line *line = NULL;
    Line *prevLine = NULL;
	static File *currentFile = NULL;

	currentFile = currentWindow->currentFile;

    if (!currentFile) return;
	
    x = currentFile->cursorCol;
    y = currentFile->cursorLine;

    if (x < 0) x = 0;
    if (y < 0) y = 0;

    node = currentFile->currentLineNode;

    if(!node){
        logger("[ed_backspace] node is null", 0);
        return;
    }

    if(on_selection_tool == true){
        ed_deleteSelection();
        ed_clearSelection();
        dw_requestRenderEvent(DW_RENDER_SELECTION);
        return;
    }
    // Current character we are on
    // If we are at the first character of the line
    if(x == 0){
        line = currentFile->currentLine;
        // wE Delete the current line but also we need to copy 
		// the current line content to 
        // the last character of the previous line
        // We are at the first line of the file
        if(!node->prev) return;

        prevNode = node->prev;
        prevLine = (Line *)prevNode->data;

        if(!prevLine){
            logger("[ed_backspace] prevLine is null", 0);
            return;
        }

        // Check for buffer overflow before merging
        if((prevLine->length + line->length) >= MAX_FILE_LINE_LENGTH){
             // Cancel merge if it would overflow
             return;
        }

        // We only insert the content to the previous line if any
        if(line->length > 0){
            memcpy(
				prevLine->buffer + prevLine->length,
				line->buffer,
				line->length
			);

            prevLine->length += line->length ;
        }   
        
        currentFile->cursorCol = prevLine->length;

        // We delete the line by moving it to deleted lines and we re join 
		// the nodes without the current line
        if(node->next){
            prevNode->next = node->next;
            node->next->prev = prevNode;
        } else {
            // We were at the last node, so prevNode becomes the new last node
            currentFile->lines->lastNode = prevNode;
            if(prevNode) prevNode->next = NULL;
        }
        
        currentFile->lines->length--;
        
        // Updating line metadata BEFORE recycling node
        currentFile->currentLineNode = prevNode;

        // RECYCLE THE NODE
        node->next = NULL;
        node->prev = NULL;
        node->isDeleted = true;
        addGenericNode(&currentFile->deletedLines, node, currentFile->arena);
        
        if(currentCursorY > 0){
            currentCursorY--;
        } else if(currentFile->scrollY > 0){
            currentFile->scrollY--;
        }
         
        // Updating line metadata
        currentFile->cursorLine = 
            currentFile->cursorLine > 0 
            ?   currentFile->cursorLine - 1
            :   0;

        currentFile->prevLine = 
            currentFile->currentLineNode->prev
            ? (Line*) currentFile->currentLineNode->prev->data
            : NULL;

        currentFile->currentLine = 
			(Line *)currentFile->currentLineNode->data;

        currentFile->nextLine = 
            currentFile->currentLineNode->next
            ? (Line*) currentFile->currentLineNode->next->data
            : NULL;

		dw_requestRenderEvent(DW_RENDER_WINDOW);

    }else{
        if(currentFile->cursorCol > 0){
            memcpy(
                currentFile->currentLine->buffer + x - 1,
                currentFile->currentLine->buffer + x, 
                currentFile->currentLine->length - x
            );

            currentFile->currentLine->length--;
            currentFile->currentLine->buffer[
				currentFile->currentLine->length
			] = '\0';

            currentFile->cursorCol--;

			if(currentFile->currentLine->length > currentWindow->width){
            	ed_updateScrollX();
			}else{
				dw_requestRenderEvent(DW_RENDER_WINDOW);
			}
        }
    }

    currentFile->isModified = true;      

    ed_markActive(ED_ACTIVITY_DEL);
    ed_updateCursor();

	dw_requestRenderEvent(DW_RENDER_LINE);
}
void ed_supr(){
        // We type the char at 
    // X : currentCursorX + LINE_COUNTER_WIDTH + 1
    // Y : currentCursorY + file->scrollY + 1 
    unsigned short x = 0;
    Node *node = NULL;
    Line *line = NULL;
	static File *currentFile = NULL;

	currentFile = currentWindow->currentFile;

    if (!currentFile) return;

    x = currentFile->cursorCol;
    
    node = currentFile->currentLineNode;
    line = (Line *)node->data;
    
    if (x >= line->length) return;
    
    memcpy(
		line->buffer + x, 
		line->buffer + x + 1, 
		line->length - x
	);    
    
	line->length--;
    line->buffer[line->length] = '\0';

    currentFile->isModified = true;

    ed_markActive(ED_ACTIVITY_SUPR);
    ed_updateCursor();

	dw_requestRenderEvent(DW_RENDER_WINDOW);
}

void ed_newLine(){
    unsigned int cursorCol = 0;
    size_t copyLen = 0;
    int prevLineTabs = 0;
    int autoClosePos = 0;
    int indentTabs = 0;
    bool isAutoClose = false;
    Node *newLineNode = NULL;
    Line *newLine = NULL;
    MemoryArena *arena = NULL;
	static File *currentFile = NULL;

	currentFile = currentWindow->currentFile;

    if (!currentFile) return;

    arena = currentFile->arena;

    cursorCol = currentFile->cursorCol;

    prevLineTabs =
		 _get_tab_counts_until(currentFile->currentLine->length);

	// Creating new line/reusing deleted line
    newLineNode = _resolveNewLine(currentFile);

	newLine = (Line*)newLineNode->data;
	
	if(!newLineNode || !newLine)
		return;

    autoClosePos = _get_auto_close_pos();

    if(currentFile->prevChar == '{'){
        indentTabs = prevLineTabs + 1;
    }else if(currentFile->currentChar == '}'){
        isAutoClose = true;
        indentTabs = autoClosePos;
    }else{
        indentTabs = prevLineTabs;
    }
	
    copyLen = 
		(cursorCol <= currentFile->currentLine->length)
		? currentFile->currentLine->length - cursorCol
		: 0;
	
    logger(
		"[ed_newLine]: indentTabs %d, %d, copyLen : %d", 
		indentTabs, 
		currentFile->currentLine->buffer + cursorCol,
		copyLen
	);
    
	
    if(indentTabs > 0){
        memset(newLine->buffer , CHAR_TAB, indentTabs);
        newLine->length = indentTabs;
    }	

	if(copyLen > 0){
        memcpy(
			newLine->buffer + indentTabs, 
			currentFile->currentLine->buffer + cursorCol, 
			copyLen
		);
		newLine->length += copyLen;
	}

	// =============

    // Copy prev line tabs
    if(isAutoClose == true){
        newLine->buffer[indentTabs] = '}';
		memset(
			currentFile->currentLine->buffer + cursorCol, 
			'\0', 
			MAX_FILE_LINE_LENGTH - cursorCol
		);
    }         
    // We clear the current line position onwards
    
    currentFile->currentLine->length = cursorCol;

    // Pointer logic
    // newLineNode is already allocated or recycled above
    newLineNode->prev = currentFile->currentLineNode;
    newLineNode->next =
        currentFile->currentLineNode->next 
        ?
            currentFile->currentLineNode->next 
        :
            NULL
        ;

    if(newLineNode->next) 
		newLineNode->next->prev = newLineNode;
 

    currentFile->currentLineNode->next = newLineNode;
    currentFile->currentLineNode = newLineNode;

    // Update List structure
    if(!newLineNode->next){
        currentFile->lines->lastNode = newLineNode;
    }
    currentFile->lines->length++;

    currentFile->prevLine = 
        currentFile->currentLineNode->prev &&
        currentFile->currentLineNode->prev->data
        ?
            currentFile->currentLineNode->prev->data
        :
            NULL    
        ;

    currentFile->currentLine = 
		currentFile->currentLineNode->data;
    
    // Next line logic
    currentFile->nextLine = 
        currentFile->currentLineNode->next &&
        currentFile->currentLineNode->next->data
        ?
            currentFile->currentLineNode->next->data
        :
            NULL
        ;

    currentFile->cursorCol = 0 + indentTabs;
    currentFile->cursorLine++;

    // activity flags
    currentFile->isModified = true;
    
    ed_markActive(ED_ACTIVITY_NEWLINE);
    ed_updateCursor();
    ed_updateScrollY();

	dw_requestRenderEvent(DW_RENDER_WINDOW);
}

// PROMPT ELEMENT
char *ed_scanf(
	unsigned char x, 
	unsigned char y, 
	unsigned char maxChars 
){
    int i = 0;
    int j = 0;
    int lenbuff = 0;
    char c = 0;
    bool esc = false;

    static char buffer[MAX_FILE_LINE_LENGTH];

    memset(buffer, '\0', MAX_FILE_LINE_LENGTH);

    ed_putCursor(x,y);    

    while(
		c != CHAR_ENTER && 
		!(esc = hal_inp_isKeyPressed(HAL_KEY_ESC) == true)
	){
        c = hal_inp_getch();

        if(c == 0 || (unsigned char)c == 0xE0){
            c = hal_inp_getch();

            if(c == KEY_LEFT && i > 0){
                i--;
                ed_putCursor(x + i, y);
            } 
            if(c == KEY_RIGHT && i < (int)strlen(buffer)){
                i++;
                ed_putCursor(x + i, y);
            } 
            if(c == KEY_DELETE){
                // Shift to the left the buffer from the current 
				// position 
                lenbuff = strlen(buffer);
                for(j=i; j <= lenbuff; j++){
                    buffer[j] = buffer[j+1];
                }

                // Redraw he entire prompt by copying the
				//  buffer content to the screen buffer
                for(j=0;j <  maxChars; j++){
                    dw_charXY(textmemptr,buffer[j], x+j, y);
                }
            }
        }else{   
            // OK
            if(c == CHAR_BACKSPACE && i > 0 ){
                lenbuff = strlen(buffer);
                for(j=i; i > 0 && j <= lenbuff; j++){
                    buffer[j - 1] = buffer[j];
                }   
                
                // Redraw
                i--;

                // Redraw he entire prompt by copying the 
				// buffer content to the screen buffer
                for(j=0;j < maxChars; j++){
                    dw_charXY(textmemptr,buffer[j], x+j, y);
                }   

                ed_putCursor(x + i, y);
            }else if(c == CHAR_SPACE){                
                for(j=strlen(buffer); j >= i; j--){
                    if(
						j + 1 < MAX_FILE_LINE_LENGTH &&
						j + 1 < maxChars 
					)
                        buffer[j + 1] = buffer[j];                    
                }

                buffer[i] = ' ';

                // Redraw he entire prompt by copying the buffer 
				// content to the screen buffer
                for(j=0;j <  maxChars; j++){
                    dw_charXY(textmemptr,buffer[j], x+j, y);
                }   
                
                i++;
                ed_putCursor(x + i, y);
            } else if (
				c >= 32 && 
				i < maxChars && 
				i < MAX_FILE_LINE_LENGTH - 1
			){
                buffer[i] = c;
                dw_charXY(textmemptr,c,x + i,y);
                i++;
                ed_putCursor(x + i, y);
            }
        }
    }
    
    ed_putCursor(currentCursorX,currentCursorY);    

    if (esc == true) return NULL;

    return buffer;
}


// PROMPT ELEMENT
// This is async, i mean, each loop step like in the original function
// in ed_scanf, is done outside the function call.
char *ed_async_scanf(
	unsigned char x, 
	unsigned char y, 
	unsigned char maxChars,
	char *buffer, 
	size_t bufflen, 
	int *stepIndex
){
    int j = 0;
    int lenbuff = 0;
    int charLimit = 0;
    char c = 0;
    bool esc = false;

    charLimit = bufflen >= maxChars ? maxChars : bufflen + 2;
    
    ed_putCursor(x + (*stepIndex),y);    
    
    // Redraw he entire prompt by copying the buffer content to the screen buffer
    for(j=0;j < maxChars; j++){
        if(j > (int)bufflen){
            dw_charXY(textmemptr,' ', x+j, y);
        }else{
            dw_charXY(textmemptr,buffer[j], x+j, y);
        }
    }       

    hal_vid_refresh();
    c = hal_inp_getch();

    if(c == 0 || (unsigned char)c == 0xE0){
        c = hal_inp_getch();

        if(c == KEY_LEFT && (*stepIndex) > 0){
            (*stepIndex)--;
            ed_putCursor(x + (*stepIndex), y);
        } 
        if(c == KEY_RIGHT && (*stepIndex) < (int)strlen(buffer)){
            (*stepIndex)++;
            ed_putCursor(x + (*stepIndex), y);
        } 
        if(c == KEY_DELETE){
            // Shift to the left the buffer from the current position 
            lenbuff = strlen(buffer);
            for(j=(*stepIndex); j <= lenbuff; j++){
                buffer[j] = buffer[j+1];
            }

        }
    }else{   
        // OK
        if(c == CHAR_BACKSPACE && (*stepIndex) > 0 ){
            lenbuff = strlen(buffer);
            for(j=(*stepIndex); (*stepIndex) > 0 && j <= lenbuff; j++){
                buffer[j - 1] = buffer[j];
            }   
            
            // Redraw
            (*stepIndex)--;

            ed_putCursor(x + (*stepIndex), y);
        }else if(c == CHAR_SPACE){                
            for(j=strlen(buffer); j >= (*stepIndex); j--){
                if(j + 1 < MAX_FILE_LINE_LENGTH && j + 1 < charLimit ){
                    buffer[j + 1] = buffer[j];
                }
            }

            buffer[(*stepIndex)] = ' ';
            
            (*stepIndex)++;
            ed_putCursor(x + (*stepIndex), y);
        // Only accept printable characters (ASCII >= 32)
        } else if (
			c >= 32 &&
			(*stepIndex) < charLimit && 
			(*stepIndex) < MAX_FILE_LINE_LENGTH - 1
		){
            buffer[(*stepIndex)] = c;
            dw_charXY(textmemptr,c,x + (*stepIndex),y);
            (*stepIndex)++;
            ed_putCursor(x + (*stepIndex), y);
        }
    }

    ed_putCursor(currentCursorX,currentCursorY);    

    if (esc == true) return NULL;

    return buffer;
}

// Update a window currentFile currentLine from cursorLine
void ed_updateCurrentLine(Window *window){
    Node *currNode = NULL;
    Line *currLine = NULL;

    if(!window || !window->currentFile){
        logger("[ed_updateCurrentLine]: Invalid window/currentLine data.");
        return;
    }

    currNode = getNodeByIndex(
        (&window->currentFile->lines), 
        window->currentFile->cursorLine
    );

    if(!currNode || !currNode->data){
        logger("[ed_updateCurrentLine]: Invalid currNode.");
        return;
    }

    currLine = (Line *)currNode->data;

    window->currentFile->currentLineNode = currNode;
    window->currentFile->currentLine = currLine;
    
    window->currentFile->prevLine = 
        (currNode->prev &&
        currNode->prev->data)
        ? (Line*)currNode->prev
        : NULL;

    window->currentFile->nextLine = 
        (currNode->next &&
        currNode->next->data)
        ? (Line*)currNode->next->data
        : NULL;
        
    return;
}

// HOME
void ed_putCursorEnd(){
	static File *currentFile = NULL;

	if(!currentWindow || !currentWindow->currentFile)
		return;

	currentFile = currentWindow->currentFile;

    currentFile->cursorCol = 
		currentFile->currentLine->length;

    ed_updateScrollX();

	dw_requestRenderEvent(DW_RENDER_WINDOW);
    ed_updateCursor();
}

// END
void ed_putCursorStart(){   
	static File *currentFile = NULL;

	if(!currentWindow || !currentWindow->currentFile)
		return;

	currentFile = currentWindow->currentFile;
    currentFile->cursorCol = 0;

    ed_updateScrollX();
	
	dw_requestRenderEvent(DW_RENDER_WINDOW);
    ed_updateCursor();
}

// PG UP
void ed_putCursorFistLine(){
    int screenHeightJump = 0;
	static File *currentFile = NULL;

	if(!currentWindow || !currentWindow->currentFile)
		return;

    screenHeightJump = currentWindow->height;
    currentFile = currentWindow->currentFile;

    // if we are on any line but the line position is
    // over the screeen height
    if(currentFile->cursorLine > currentFile->scrollY){
        currentFile->cursorLine = currentFile->scrollY;
    }else if(
        // If we are on any page after the third one
        currentFile->cursorLine == currentFile->scrollY &&
        (currentFile->cursorLine - screenHeightJump > 0)
    ){
        currentFile->cursorLine -= screenHeightJump;
    }else if( 
        // if we are on top but already on the second page
        currentFile->cursorLine == currentFile->scrollY
    ){
        currentFile->cursorLine = 0;
    }
    
    // We update the currentLineNode, currentLine, prevLine, nextLine
    // File attrs. 
    ed_updateCurrentLine(currentWindow);

    // Truncate cursorCol if next position line length is
    // shorter than the previous one
    if(currentFile->cursorCol > currentFile->currentLine->length)
        currentFile->cursorCol = currentFile->currentLine->length;
    
    // Sync file cursor
    ed_updateScrollY();
	dw_requestRenderEvent(DW_RENDER_WINDOW);
    ed_updateCursor();
}

// PG DOWN
void ed_putCursorLastLine(){
    int screenHeightJump = 0;
	static File *currentFile = NULL;

	if(!currentWindow || !currentWindow->currentFile)
		return;

    screenHeightJump = currentWindow->height;
    currentFile = currentWindow->currentFile;
    
    // If we are sitting on the last line of the page
    if(
        (currentFile->cursorLine - currentFile->scrollY) == 
        (screenHeightJump)
    ){
        if(
            (currentFile->cursorLine + screenHeightJump) >
            (currentFile->lines->length)
        ){
            currentFile->cursorLine = currentFile->lines->length - 1;
        }else{
            currentFile->cursorLine += screenHeightJump;
        }
    }else{
        // If we are sitting in the middle of the page, we just go
        // to the bottom of the page
        if(currentFile->lines->length > screenHeightJump){
            currentFile->cursorLine = 
                currentFile->scrollY + screenHeightJump;
        }else{
            currentFile->cursorLine = 
                currentFile->lines->length - 1;
        }
    }
    
    // We update the currentLineNode, currentLine, prevLine, nextLine
    // File attrs. 
    ed_updateCurrentLine(currentWindow);

    // Truncate cursorCol if next position line length is
    // shorter than the previous one
    if(currentFile->cursorCol > currentFile->currentLine->length)
        currentFile->cursorCol = currentFile->currentLine->length;
    // then we need to reset some flags so we can redraw the screen properly
    
    // Sync file cursor
    ed_updateScrollY();
	dw_requestRenderEvent(DW_RENDER_WINDOW);
    ed_updateCursor();
}

void ed_wordJump(short wordJump){
    size_t currentCharPos = 0;
    char *currentLineBuffer = NULL;
	static File *currentFile = NULL;

	if(
		!currentWindow ||
		!currentWindow->currentFile
	)
		return;

	currentFile = currentWindow->currentFile;
    currentCharPos = currentFile->cursorCol;
    currentLineBuffer = currentFile->currentLine->buffer;

    switch(wordJump){
        case ED_WORD_JUMP_PREV:
            if(
				(currentCharPos - 1) > 0 &&
			 	currentLineBuffer[currentCharPos] == ' '
			)
				currentCharPos--;
            
            while(
                (currentCharPos - 1 > 0) &&
                (currentLineBuffer[currentCharPos] != ' ' ||
                currentLineBuffer[currentCharPos - 1] == ' ')){
                    currentCharPos--;
            }
            break;
        case ED_WORD_JUMP_NEXT:
            if(
				currentCharPos < currentFile->currentLine->length &&
				currentLineBuffer[currentCharPos] == ' '
			) 
				currentCharPos++;

            while(
                (currentCharPos + 1 < currentFile->currentLine->length) &&
                (currentLineBuffer[currentCharPos] != ' ' ||
                currentLineBuffer[currentCharPos + 1] == ' ')){
                    currentCharPos++;
            }
            break;
    }
    
    currentFile->cursorCol = currentCharPos;
    ed_updateCursor();
}

// This is for swapping the lines with the next or previous one with the
// ALT + UP or ALT + DOWN key stroke.
// Similar to visual studio code editor feature

void ed_swapLine(short lineJump){
	static File *currentFile = NULL;
    Node *tmpNext = NULL;
    Node *tmpPrev = NULL;
    Node *currentLineNode = NULL; 
	
	if(!currentWindow || !currentWindow->currentFile)
		return;

	currentFile = currentWindow->currentFile;
	currentLineNode = currentFile->currentLineNode;

    switch(lineJump){
        case ED_LINE_JUMP_DOWN:
            if(currentLineNode->next == NULL) 
				break;

            currentLineNode = currentFile->currentLineNode->next;
            currentFile->cursorLine += 2;

            if(currentCursorY + 1 < VIDEO_ROWS ) 
				currentCursorY+=2; 
            /* FALLTHROUGH ...*/
        case ED_LINE_JUMP_UP:
            tmpNext = currentLineNode->next;
		    tmpPrev = currentLineNode->prev;

            // Is head
            if(currentLineNode->prev == NULL) 
				break;

			if(tmpPrev->prev != NULL)
				tmpPrev->prev->next = currentLineNode;
			

			currentLineNode->prev = tmpPrev->prev;

			currentLineNode->next = tmpPrev;
			tmpPrev->prev = currentLineNode;
			tmpPrev->next = tmpNext;

            if(tmpNext != NULL)
                tmpNext->prev = tmpPrev;            
            
            currentFile->cursorLine--;

            if(currentCursorY - 1 > 0) 
				currentCursorY--; 

            break;
    }

    ed_markActive(lineJump);

    ed_updateCursor();
	
	dw_requestRenderEvent(DW_RENDER_WINDOW);
}