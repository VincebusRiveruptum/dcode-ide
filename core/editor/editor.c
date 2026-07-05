/*
    This is a basic TUI library, right now just for testing purposes
    By Vincebus Riveruptum
    2026

    Credits osdever.net

    http://www.osdever.net/bkerndev/Docs/printing.htm
*/

#include "editor.h"
#include "../files/files.h"

//unsigned char attrib = 0x07; // Default attribute: White on Black
struct Container *root;

unsigned char VIDEO_COLS = 80;
unsigned char VIDEO_ROWS = 25;

unsigned char currentCursorX = 0;
unsigned char currentCursorY = 0;

bool ed_renderEvent = false;
bool on_selection_tool = false;
bool ed_onSearchTool = false;

time_t ed_globalAuxTimer = 0;
char statusBarMessage[ED_STATUSBAR_WIDTH] = {'\0'};

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

void _updateScrollY(){
	File *currentFile = NULL;
    int displayHeight = 0;
	
	logger("_updateScrollY!!");

	currentFile = currentWindow->currentFile;

    if (!currentFile || !currentWindow) return;
	
    displayHeight = currentWindow->height;
	

    if((currentFile->cursorLine - currentFile->scrollY) > displayHeight) {
        //currentFile->scrollY += currentFile->cursorLine - (displayHeight - 1);
        currentFile->scrollY++;
    }else if(currentFile->cursorLine <= currentFile->scrollY){
        currentFile->scrollY = currentFile->cursorLine;
    }
}

void _updateScrollX(){
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
        }
    }
}

void _updateCursor(){
	if (!currentWindow || !currentWindow->currentFile) return;

    _updateCurrentCursorY();
    _updateCurrentCursorX();

    ed_putCursor(currentCursorX, currentCursorY);
    ed_renderEvent = true;
}

void ed_initConfig(int argc, char *argv[]){
    //f_defaultExtension

    // We will hardcode the default extension until i implement .ENV/CFG LOADING
    if(!cfg_loadConfig()){
        //logger("[ed_initConfig]: Could not load config file!");
        // App exits
        printf("\nCould not load config file!.");
        return ;
    }

    log_init();   
    logger("[ed_initConfig]: %d %s", argc, argv[1]);
    ed_handleArguments(argc, argv);

    v_currentMode = settings.DEFAULT_VIDEO_MODE;

    ed_resetCursor();
}

void ed_handleArguments(int argc, char *argv[]){
    int i;
    // File opening
    logger("[ed_handleArguments]: %d %s", argc, argv[1]);
    
    if(argc > 1 || (argv != NULL && argv[1] != NULL)){
        // Multiple file opening
        for(i=1;i<argc;i++){
            if(!f_openFile(argv[i])){
                logger(
					"[ed_handleArguments]: File %s not found. Falling back to new file.", 
					argv[i]
				);

                f_newFile(argv[i]);
            }
        }
    }else{
        f_newFile(NULL);
    }

    ed_renderEvent = true;
}

void ed_resetActity(){
    if(currentWindow->currentFile && currentWindow->currentFile)
        currentWindow->currentFile->isActive = false;
    //f_flushSearchMetadata();
}

void ed_markActive(unsigned char activity){
    // Push activity to editor clipboard
    (void)activity;
    if(currentWindow->currentFile && currentWindow->currentFile)
        currentWindow->currentFile->isActive = true;
}

// We reset the cursor to X:0 Y:0 relative to the active currentFileext area
void ed_resetCursor(){
    if (!currentWindow || !currentWindow->currentFile) return;

    // In th future, when the text area became a movable element we will 
	// have to  calculate the cursor position relative to the text area 
	// position.
    _updateCursor();
}

void ed_putCursor(unsigned char x, unsigned char y){
    hal_vid_putCursor(x, y);
}

/*
    This is the cursor behavior when is inside a TEXT AREA
*/
void ed_moveCursor(short x, short y){
    Node *tempNode = NULL;
	static File *currentFile = NULL;

	currentFile = currentWindow->currentFile;

    if (!currentFile) return;

    // If the number of lines is less than the screen heightc
    // and the current Y cursor position is less than the nmber of lines
    // DANGEROUS

    if( y > 0 && !(
        currentFile->cursorLine < currentFile->lines->length - 1
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
    }
    // END VERTICAL SCROLLING =====================================================

    // HORIZ, SCROLLING ===========================================================
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

	_updateScrollX();
	_updateScrollY();

    _updateCursor();
}

void ed_renderElements(){
    Node *rec;
    Window *wnd;
    int i = 0;
    int editor_size;
	
    if (!currentWorkspace || !currentWorkspace->windowList) return;

    hal_vid_clearBuffer(editormemptr);

    rec = currentWorkspace->windowList->firstNode;
    while (rec != NULL) {
        wnd = (Window *)rec->data;
        if (wnd && wnd->currentFile) {
            dw_writeBufferEditorFormatted(
                editormemptr, 
                wnd->x, 
                wnd->y, 
                wnd->x + wnd->width, 
                wnd->y + wnd->height, 
                COLOR_LIGHT_GRAY, 
                COLOR_BLACK, 
                wnd->currentFile
            );
        }
        rec = rec->next;
    }
     
    editor_size = (VIDEO_COLS * (VIDEO_ROWS - 1));
    for(i=0; i < editor_size; i++){
        textmemptr[i] = editormemptr[i];
    }
}


void ed_typeChar(char c){
    // We type the char at 
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
    _updateScrollX();
    _updateCursor();
}

// Deletes a selectrion
void ed_backspaceSelection(){

    //if(currentFilectedStartLine == currentFilectedEndLine){
        // We delete in the same line
        // No line reuse   
 
    //}    

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
    File *file = NULL;
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
        ed_backspaceSelection();
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

            _updateScrollX();
        }
    }

    currentFile->isModified = true;      

    ed_markActive(ED_ACTIVITY_DEL);
    _updateCursor();
}
void ed_supr(){
        // We type the char at 
    // X : currentCursorX + LINE_COUNTER_WIDTH + 1
    // Y : currentCursorY + file->scrollY + 1 
    unsigned short x = 0;
    Node *node = NULL;
    Line *line = NULL;
    File *file = NULL;
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
    _updateCursor();
}

void ed_newLine(){
    unsigned int x = 0;
    size_t copyLen = 0;
    int prevLineTabs = 0;
    int autoClosePos = 0;
    int autoIdentMovement = 0;
    bool isIndent = false;
    bool isAutoClose = false;
    Node *newLineNode = NULL;
    Line *newLine = NULL;
    MemoryArena *arena = NULL;
	static File *currentFile = NULL;

	currentFile = currentWindow->currentFile;

    if (!currentFile) return;

    arena = currentFile->arena;

    x = currentFile->cursorCol;

    prevLineTabs = _get_tab_counts_until(currentFile->currentLine->length);

    // Creating the new line then zeroing
    // RECYCLING/NEW LINE LOGIC ==========================================
    newLineNode = pop(&currentFile->deletedLines);
    
    if(newLineNode && currentFile->deletedLines->length > 0){
        logger("[ed_newLine]: reusing deleted line");
        newLine = (Line*)newLineNode->data;
        memset(newLine->buffer, '\0', MAX_FILE_LINE_LENGTH);
        newLine->length = 0;
        newLineNode->isDeleted = false; 
    } else {
        newLine = (Line*)mem_arena_alloc(arena, sizeof(Line));

        if (!newLine){
            logger("[ed_newLine]: Could not make new line");
            return;
        }

        newLine->length = 0;
        newLine->buffer = 
			(char*)mem_arena_alloc(
				arena,
				sizeof(char) * MAX_FILE_LINE_LENGTH
			);

        if (!newLine->buffer){
            logger("[ed_newLine]: Could not alloc new line buffer");
            return;
        }

        memset(newLine->buffer, '\0', MAX_FILE_LINE_LENGTH);
        
        newLineNode = (Node *)mem_arena_alloc(arena, sizeof(Node));

        
        if (!newLineNode){
            logger("[ed_newLine]: Could not alloc new line NODE");
            return;
        }
        
        newLineNode->data = newLine;
    }

    // ==============================

    // We copy the content from the current line in the current 
	// cursor position onwards to the now line
    // We detect opening and closing of function
        
    autoClosePos = _get_auto_close_pos();

    if(currentFile->prevChar == '{'){
        isIndent = true;
        autoIdentMovement = (int)isIndent + prevLineTabs;
    }else if(currentFile->currentChar == '}'){
        isAutoClose = true;
        autoIdentMovement = autoClosePos;
    }else{
        autoIdentMovement = prevLineTabs;
    }

    logger("[ed_newLine]: AutoclosePos %d", autoClosePos);

    copyLen = 
        (x <= currentFile->currentLine->length)
        ?
            currentFile->currentLine->length - x + autoIdentMovement
        :
            0
        ;
        
    memcpy(
		newLine->buffer + autoIdentMovement, 
		currentFile->currentLine->buffer + x, 
		copyLen
	);

    newLine->length = copyLen;

    // Copy prev line tabs
    if(autoIdentMovement > 0) 
		memset(newLine->buffer , CHAR_TAB, autoIdentMovement);

    if(isAutoClose == true){
        memset(newLine->buffer , CHAR_TAB, autoClosePos);
        newLine->buffer[autoClosePos] = '}';
    }         

    // We clear the current line position onwards
    memset(
		currentFile->currentLine->buffer + x, 
		'\0', 
		MAX_FILE_LINE_LENGTH - x
	);
    
    currentFile->currentLine->length = x;

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

    if(newLineNode->next) newLineNode->next->prev = newLineNode;
 

    currentFile->currentLineNode->next = newLineNode;
    currentFile->currentLineNode = newLineNode;

    // Update List structure
    if(!newLineNode->next){
        currentFile->lines->lastNode = newLineNode;
    }
    currentFile->lines->length++;

    currentFile->cursorLine = currentFile->scrollY + currentCursorY;

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

    
    /* Move cursor down and scroll if necessary */
    if (currentCursorY + 1 >= VIDEO_ROWS) {
        currentFile->scrollY++;
    } else {
        currentCursorY++;
    }

    currentFile->cursorCol = 0 + autoIdentMovement;
    currentFile->scrollX = 0;
    currentFile->cursorLine++;

    // activity flags
    currentFile->isModified = true;
    
    ed_markActive(ED_ACTIVITY_NEWLINE);
    _updateCursor();
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


void ed_putCursorEnd(){
	static File *currentFile = NULL;

	if(!currentWindow || !currentWindow->currentFile)
		return;

	currentFile = currentWindow->currentFile;

    currentFile->cursorCol = 
		currentFile->currentLine->length;

    _updateScrollX();
    _updateCursor();
}

void ed_putCursorStart(){   
	static File *currentFile = NULL;

	if(!currentWindow || !currentWindow->currentFile)
		return;

	currentFile = currentWindow->currentFile;
    currentFile->cursorCol = 0;

    _updateScrollX();
    _updateCursor();
}

void ed_putCursorFistLine(){
    int lineposX;
	static File *currentFile = NULL;

	if(!currentWindow || !currentWindow->currentFile)
		return;

	currentFile = currentWindow->currentFile;

    currentFile->currentLineNode = 
		currentFile->lines->firstNode;

    currentFile->prevLine =
        currentFile->currentLineNode &&
        currentFile->currentLineNode->prev 
        ?
            currentFile->currentLineNode->prev->data
        :
            NULL
        ;
        
    currentFile->currentLine = 
		currentFile->currentLineNode->data;
    
    currentFile->nextLine =
        currentFile->currentLineNode &&
        currentFile->currentLineNode->next
        ?
            currentFile->currentLineNode->next->data
        :
            NULL
        ;
    
    lineposX = 
        (int)currentFile->currentLine->length - 1 
		< currentCursorX - LINE_COUNTER_WIDTH
        ?
            LINE_COUNTER_WIDTH + (int)currentFile->currentLine->length - 1
        :
            currentCursorX
        ;
    
    currentCursorX = lineposX;
    currentCursorY = 0;
    
    // then we need to reset some flags so we can redraw the screen properly
    /* Sync file cursor */
    currentFile->scrollY = 0;
    currentFile->cursorLine = 0;
    currentFile->cursorCol = currentCursorX - LINE_COUNTER_WIDTH;
    
    _updateCursor();
}

void ed_putCursorLastLine(){
	int lineJump = 0;
	Node *newLineNode = NULL;
	Line *newLine = NULL;
	static File *currentFile = NULL;

	if(!currentWindow || !currentWindow->currentFile)
		return;

	currentFile = currentWindow->currentFile;

    if(
		currentFile->lines->length > 0 &&
		currentFile->cursorLine < currentFile->lines->length
	){
		lineJump = 
			(currentFile->lines->length < VIDEO_ROWS)	
			? currentFile->lines->length - 1
			: VIDEO_ROWS - 1;
			 
		currentFile->cursorLine += lineJump;
		currentFile->scrollY += 
			currentFile->lines->length < VIDEO_ROWS
			? 0 
			: lineJump;	
	}
	
	newLineNode = currentFile->lines->lastNode;

	if(!newLineNode){
		logger("[ed_putCursorLastLine]: Error trying to obtain last line node.");
		return;
	}
	
	newLine = (Line*)newLineNode->data;

	if(!newLine){
		logger("[ed_putCursorLastLine]: Error trying to obtain last line object.");
		return;
	}	        
	// Line metadata updating

	currentFile->currentLineNode = newLineNode;
	currentFile->currentLine = newLine;

	currentFile->prevLine = 
		currentFile->currentLineNode->prev 
		? (Line*) currentFile->currentLineNode->prev->data
		: NULL;

	currentFile->nextLine = 
		currentFile->currentLineNode->next
		? (Line*) currentFile->currentLineNode->next->data
		: NULL;
	
	// If on the new line the previous position is larger than the new 
	// line length, then we do the following
	if(currentFile->cursorCol >= currentFile->currentLine->length - 1){
		currentFile->cursorCol = currentFile->currentLine->length - 1;
	}

	_updateCursor();
}

void ed_statusBarMessage(const char *format,  ...){
    va_list args;

    time(&ed_globalAuxTimer);

    memset(statusBarMessage, '\0', ED_STATUSBAR_WIDTH - 1);

    va_start(args, format);
    vsprintf(statusBarMessage, format, args);
    va_end(args);

	ed_renderEvent = true;

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
        ed_renderEvent = true;
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
    _updateCursor();
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

    _updateCursor();
}

void ed_prepareSelectionTool(){
    static File *currentFile = NULL;

    if(!currentWindow || !currentWindow->currentFile)
		return;
	
	currentFile = currentWindow->currentFile;

	currentFile->oldLineNode = 
		(struct Node *)currentFile->currentLineNode;

	currentFile->oldLine = currentFile->cursorLine;
	currentFile->oldCol = currentFile->cursorCol;
}

void ed_clearSelection(){
	static File *currentFile = NULL;

    if(!currentWindow || !currentWindow->currentFile) 
		return;

	currentFile = currentWindow->currentFile;

    currentFile->selectedStartNode = NULL;
    currentFile->selectedEndNode = NULL;
    currentFile->selectedStartX = 0;
    currentFile->selectedEndX = 0;
    currentFile->selectedStartLine = 0;
    currentFile->selectedEndLine = 0;
    on_selection_tool = false;
}

void ed_handleSelection() {
    File *currentFile;
    bool isNav;

    if(!currentWindow || !currentWindow->currentFile) return;
    
    currentFile = currentWindow->currentFile;

    // Check if cursor actually moved
    if (
		currentFile->currentLineNode == currentFile->oldLineNode && 
		currentFile->cursorCol == currentFile->oldCol
	) return;
    

    isNav = 
		hal_inp_isKeyDown(HAL_KEY_UP) || 
		hal_inp_isKeyDown(HAL_KEY_DOWN) ||
        hal_inp_isKeyDown(HAL_KEY_LEFT) || 
		hal_inp_isKeyDown(HAL_KEY_RIGHT) ||
        hal_inp_isKeyDown(HAL_KEY_HOME) || 
		hal_inp_isKeyDown(HAL_KEY_END) ||
        hal_inp_isKeyDown(HAL_KEY_PAGEUP) || 
		hal_inp_isKeyDown(HAL_KEY_PAGEDOWN);

    if (isNav) {
        if (
			hal_inp_isKeyDown(HAL_KEY_LSHIFT) || 
			hal_inp_isKeyDown(HAL_KEY_RSHIFT)) 
		{
            // If selection is not active, anchor it at the old position
            if (currentFile->selectedStartNode == NULL) {
                currentFile->selectedStartNode = currentFile->oldLineNode;
                currentFile->selectedStartX = currentFile->oldCol;
                currentFile->selectedStartLine = currentFile->oldLine;
            }
            // Always update selection end to the new position
            currentFile->selectedEndNode = currentFile->currentLineNode;
            currentFile->selectedEndX = currentFile->cursorCol;
            currentFile->selectedEndLine = currentFile->cursorLine;
            on_selection_tool = true;
        } else {
            // Clear selection since we moved cursor without Shift
            ed_clearSelection();
        }
    } else {
        // Any other cursor movement (e.g. typing, backspace, new line) 
		// clears selection
        ed_clearSelection();
    }
}

// This is a small program for testing. 
// The purpose is take a phrase and count the words.e

int ed_wordCountInStr(char *str){
	int wordCount = 0;
	char *wordIndexPtr;
	char *detectedStartOffset = NULL;
	char *detectedEndOffset = NULL;
	size_t detectedWordLen = 0;
	char detectedWord[255] = {'\0'};
	wordIndexPtr = str;
	
	
	// No words!Wing by spac
	if(*wordIndexPtr == '\0') return 0;

	while(wordIndexPtr && *wordIndexPtr != '\0'){
		// Buffer reset
		
		// Word start is when the previous char is empty space
		// Word end is when next char is space
		// We have to ignore escape chars
		// We need to calculate the len between both detectedWord offsets.
		if(!detectedStartOffset &&
			isalpha(*wordIndexPtr)
		){
			detectedStartOffset = wordIndexPtr;
		
		// If is an space, we know that a word ended
		}else if (*wordIndexPtr == ' ' || *(wordIndexPtr + 1) == '\0'){
			detectedEndOffset = wordIndexPtr;
			
			wordCount++;
			if(detectedEndOffset && detectedStartOffset){
				detectedWordLen = detectedEndOffset - detectedStartOffset;
				memcpy(detectedWord, detectedStartOffset, detectedWordLen);
				detectedWord[detectedWordLen] = '\0';
				//printf("\nDetected word: %s", detectedWord);
			}
            
			detectedStartOffset = NULL;
			detectedEndOffset = NULL;
			memset(detectedWord, '\0', 255);
			detectedWordLen = 0;
		}
		
		wordIndexPtr++; 
	}

	return wordCount;
}

// This will find word matches according to the currentSearchMetadata
// found word match
void ed_findWord(){
    int wordLen = 0;
    unsigned int lineIndex = 0;
    char *detectedWordOffset = NULL;
    char *wordIndexPtr = NULL;
    char searchArenaName[32];
    WordMetadata *matchBuffer = NULL;
    Node *lineNode = NULL;
	File *currentFile = NULL;
	SearchMetadata *currentFileSearch = NULL;

    if(
        !currentWindow ||
        !currentWindow->currentFile ||
        !currentWindow->currentFile->lines ||
        !currentWindow->currentFile->lines->firstNode
    ){

        logger("[ed_findWord]: currentWindow first line node is NULL");
        return;
    }

	currentFile = currentWindow->currentFile;
	currentFileSearch = currentFile->currentFileSearch;

    lineNode = currentFile->lines->firstNode;
	
    if(!lineNode){
        logger("[ed_findWord]: lineNode is NULL");
        return;
    }
    
    if(!currentFileSearch){
        logger("[ed_findWord]: currentFileSearch is NULL");
        return;
    }

    if(!currentFileSearch->arena || !currentFileSearch->arena->base){
        sprintf(searchArenaName, "SRCH");
        currentFileSearch->arena = (MemoryArena *)mem_create_arena(searchArenaName, MEM_ARENA_2K);
    } else {
        mem_arena_reset(currentFileSearch->arena);
    }

    wordLen = strlen(currentFileSearch->dialogInputBuffer);

    if (wordLen == 0) return;


    currentFileSearch->wordCount = 0;
    currentFileSearch->words = NULL;
    currentFileSearch->currentWordNode = NULL;

    // If searchMetadata atributes are NULl this means that there is no previous search done
    // So we will begin the process.

    // If there is already a metadata

    // Depending on the orientation ( previous, next ) we will look forward or previous from the wordOffset and 
    // the line index.
    logger(
		"[ed_findWord]: Current word! : %s",
		currentFileSearch->dialogInputBuffer
	);

    while(lineNode != NULL){
        
        if(
            !lineNode->data ||
            !((Line*)lineNode->data)->buffer
        ){
            logger(
				"[ed_findWord]: lineNode->data or lineNode->data->buffer is NULL"
			);
            return;
        }
        
        wordIndexPtr = ((Line*)(lineNode->data))->buffer;
        // No words!Wing by spac
        if(*wordIndexPtr == '\0'){
            lineNode = lineNode->next;
            lineIndex++;
            continue;
        } 

        detectedWordOffset = 
			strstr(
				wordIndexPtr, 
				currentFileSearch->dialogInputBuffer
			);
        
        while(detectedWordOffset){        
            matchBuffer = 
				(WordMetadata *) 
				mem_arena_alloc(
					currentFileSearch->arena, 
					sizeof(WordMetadata)
				);
     
            if(!matchBuffer){
                logger("[ed_findWord]: Line 1558, matchBuffer is NULL");
                return;
            }

            matchBuffer->lineNode = lineNode;
            matchBuffer->wordIndex = currentFileSearch->wordCount;
    
            matchBuffer->cursorLine = lineIndex;
            logger("[ed_findWord]: matchBuffer->cursorLine = %d", lineIndex);

            matchBuffer->cursorCol = detectedWordOffset - wordIndexPtr;
            // Word position in line
            matchBuffer->wordPtr = detectedWordOffset;
            
            addGenericNode(
				&currentFileSearch->words, 
				matchBuffer, 
				currentFileSearch->arena
			);
            
            currentFileSearch->wordCount++;
  
            detectedWordOffset = 
				strstr(
					detectedWordOffset + wordLen,
					currentFileSearch->dialogInputBuffer
				);
        }
                
        lineNode = lineNode->next;
        lineIndex++;
    }

    // We set the first found word as current word
    currentFileSearch->currentWordNode =
        currentFileSearch->words &&
        currentFileSearch->words->firstNode
        ? currentFileSearch->words->firstNode 
        : NULL ;


}

void ed_drawSearchTool(){
    File *currentFile = currentWindow ? currentWindow->currentFile : NULL;
    SearchMetadata *currentFileSearch = currentFile ? currentFile->currentFileSearch : NULL;
    int vis_offset = 0;
    int dialogStartY = 0;
    
    if (!currentFileSearch) return;
    
    vis_offset = (VIDEO_COLS / 4);
    dialogStartY = 2;

    dw_rectangle(
		textmemptr, 
		vis_offset, 
		dialogStartY, 
		VIDEO_COLS - vis_offset, 
		6, 
		COLOR_BLUE, 
		COLOR_WHITE, 
		' ', 
		COLOR_WHITE, 
		COLOR_BLUE, 
		false, 
		DRAW_BORDER_SIMPLE, 
		"Search..."
	);

    dw_writeBuffer(
        textmemptr, 
        "Found matches: %d, Currently on result: %d", 
        vis_offset + 1, 
        dialogStartY + 2, 
        vis_offset + 48, 
        dialogStartY + 2, 
        COLOR_WHITE, 
        COLOR_BLUE, 
        currentFileSearch->wordCount,
        (
            currentFileSearch->currentWordNode && 
            ((WordMetadata *)currentFileSearch->currentWordNode->data) 
                ? ((WordMetadata *)currentFileSearch->currentWordNode->data)->wordIndex
                : 0
        )
    );

    ed_async_scanf(
		vis_offset + 1, 
		3, 
		(2 * vis_offset) - 1, 
		currentFileSearch->dialogInputBuffer, 
		strlen(currentFileSearch->dialogInputBuffer), 
		&(currentFileSearch->dialogInputIndex)
	);
            
}

void ed_searchMoveCursor(){
    File *currentFile = currentWindow ? currentWindow->currentFile : NULL;
    SearchMetadata *currentFileSearch = currentFile ? currentFile->currentFileSearch : NULL;

    if(
        !currentFile ||
        !currentFileSearch ||
        !currentFileSearch->words ||
        !currentFileSearch->currentWordNode ||
        !currentFileSearch->currentWordNode->data
    ) return;
    
    if(
		hal_inp_isKeyDown(HAL_KEY_ENTER) && 
		!hal_inp_isKeyDown(HAL_KEY_LSHIFT)
	){
        // We go forward
        currentFileSearch->currentWordNode = 
            currentFileSearch->currentWordNode &&
            currentFileSearch->currentWordNode->next
            ? currentFileSearch->currentWordNode->next
            : currentFileSearch->currentWordNode ;        

    }else if (
		hal_inp_isKeyDown(HAL_KEY_ENTER) && 
		hal_inp_isKeyDown(HAL_KEY_LSHIFT)
	){
        // We go back         
        currentFileSearch->currentWordNode = 
            currentFileSearch->currentWordNode &&
            currentFileSearch->currentWordNode->prev
            ? currentFileSearch->currentWordNode->prev
            : currentFileSearch->currentWordNode ;        
    }

    // We update the cursor
    currentFile->currentLineNode = 
        currentFileSearch->currentWordNode &&
        currentFileSearch->currentWordNode->data &&
        ((WordMetadata*) currentFileSearch->currentWordNode->data)->lineNode
        ? ((WordMetadata *)currentFileSearch->currentWordNode->data)->lineNode
        : NULL;

    currentFile->cursorCol = 
        currentFileSearch->currentWordNode &&
        currentFileSearch->currentWordNode->data &&
        ((WordMetadata*) currentFileSearch->currentWordNode->data)->cursorCol
        ? ((WordMetadata *)currentFileSearch->currentWordNode->data)->cursorCol
        : 0;

    currentFile->cursorLine =
        currentFileSearch->currentWordNode &&
        currentFileSearch->currentWordNode->data &&
        ((WordMetadata*) currentFileSearch->currentWordNode->data)->cursorLine
        ? ((WordMetadata *)currentFileSearch->currentWordNode->data)->cursorLine
        : 0;
	logger("ed_searchMoveCursor!!");

    _updateScrollY();
    _updateCursor();
}
void ed_prepareSearchTool(){
    if(
		hal_inp_keysPressed(
			HAL_INP_TRIGGER_EDGE, 
			2, 
			HAL_KEY_LCTRL, 
			HAL_KEY_F)
		) ed_onSearchTool = true;
		
    if(ed_onSearchTool == true){
        if(hal_inp_isKeyPressed(HAL_KEY_ESC)){
            ed_onSearchTool = false;
            ed_renderEvent = true;
        }else{
            ed_drawSearchTool();
            if(hal_inp_isKeyPressed(HAL_KEY_ESC)){
                ed_onSearchTool = false;
                ed_renderEvent = true;
            }else if (!hal_inp_isKeyDown(HAL_KEY_ENTER)){
                ed_findWord();
                ed_renderEvent = true;
            }else{                
                ed_searchMoveCursor();
                ed_renderEvent = true;
            }
        }
    } 
}


// Shell spawn.. 
void ed_shellSpawn(){
    char cmd[255];
    char currPath[255];
    char *comspec = NULL;
    File *currentFile = NULL;

    memset(cmd, '\0', 255);
    memset(currPath, '\0', 255);

#if defined(__MSDOS__) || defined(__WATCOMC__)
    comspec = getenv("COMSPEC");
#else
    comspec = getenv("SHELL");
#endif

    currentFile = currentWindow ? currentWindow->currentFile : NULL;

    if(!currentFile) return;
    
    hal_inp_closeKeyboard();

    hal_vid_set25Lines();
    dw_cls(textmemptr);

    // TODO: SAVE FILE

#if defined(__MSDOS__) || defined(__WATCOMC__)
    if (!comspec) comspec = "COMMAND.COM";
#else
    if (!comspec) comspec = "/bin/bash";
#endif

    strncpy(
		currPath, 
		currentFile->name, 
		hal_fs_getFilePath(currentFile->name)
	);

#if defined(__MSDOS__) || defined(__WATCOMC__)
    sprintf(cmd, "cd %s", currPath);

    logger("[ed_shellSpawn]: %s", currPath);

    spawnl(P_WAIT, comspec, comspec, "/K", cmd, NULL);
#else
    sprintf(cmd, "cd %s && %s", currPath, comspec);

    logger("[ed_shellSpawn]: %s", currPath);

    system(cmd);
#endif
    
    hal_inp_initKeyboard();
    hal_inp_clearKeyboardBuffer();
    
    hal_vid_setVideoMode(v_currentMode, false);

    ed_renderEvent = true;
}