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
                logger("[ed_handleArguments]: File %s not found. Falling back to new file.", argv[i]);
                f_newFile(argv[i]);
            }
        }
    }else{
        f_newFile(NULL);
    }

    ed_renderEvent = true;
}

void ed_resetActity(){
    if(currentFileArena && currentFileArena->file)
        currentFileArena->file->isActive = false;
    //f_flushSearchMetadata();
}

void ed_markActive(unsigned char activity){
    // Push activity to editor clipboard
    (void)activity;
    if(currentFileArena && currentFileArena->file)
        currentFileArena->file->isActive = true;
}

int _get_tab_counts_until(int col){
    int i = 0;
    int tabCount = 0;

    if (!currentFileArena->file->currentLine) return 0;

    while(i < col && i < (int)currentFileArena->file->currentLine->length){
        if(currentFileArena->file->currentLine->buffer[i] == CHAR_TAB) tabCount++;
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

    travelingBackwards = currentFileArena->file->currentLineNode;

    if(!travelingBackwards) return 0;
    
    while(travelingBackwards != NULL){
        if( travelingBackwards->data &&
            ((Line*)travelingBackwards->data)->buffer &&
            ((Line*)travelingBackwards->data)->length){
                
            if(((Line*)travelingBackwards->data)->buffer[((Line *)travelingBackwards->data)->length-1] == '{'){
                return _get_tab_counts_someline((Line*)travelingBackwards->data, ((Line *)travelingBackwards->data)->length);
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

    do{
        c = currentFileArena->file->currentLine->buffer[i];

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

    do{
        c = currentFileArena->file->currentLine->buffer[i];

        if (c == CHAR_TAB){
            tabCount++;

            cnext = currentFileArena->file->currentLine->buffer[i + 1];

            if(cnext == '\0' && cnext != CHAR_TAB){
                return tabCount;
            }
        }
        i++;
    }while (c != '\0');

    return tabCount;
}
void _updateScrollY(){
    if (!currentFileArena || !currentFileArena->file) return;

    if((currentFileArena->file->cursorLine - currentFileArena->file->scrollY) > VIDEO_ROWS - 1) {
        currentFileArena->file->scrollY += currentFileArena->file->cursorLine - (VIDEO_ROWS - 1);
    }else if(currentFileArena->file->cursorLine <= currentFileArena->file->scrollY){
        currentFileArena->file->scrollY = currentFileArena->file->cursorLine;
    }
}
void _updateCurrentCursorY(){
    if (!currentFileArena || !currentFileArena->file) return;

     // If the cursor is closer to the bottom
    if(currentCursorY <=0) currentCursorY = 0;
    
    if( currentFileArena->file->cursorLine - currentFileArena->file->scrollY >= 0){
        currentCursorY = currentFileArena->file->cursorLine - currentFileArena->file->scrollY;
    }

    if(currentCursorY >= VIDEO_ROWS ) currentCursorY = VIDEO_ROWS;
}
void _updateCurrentCursorX(){
    int visualCursor = 0;
    int visualScroll = 0;
    
    if (!currentFileArena || !currentFileArena->file) return;

    visualCursor = _calculateVisualOffset(currentFileArena->file->cursorCol);
    visualScroll = _calculateVisualOffset(currentFileArena->file->scrollX);

    currentCursorX = (visualCursor - visualScroll) + LINE_COUNTER_WIDTH;

    // Boundary check to keep cursor on screen if something goes wrong
    if(currentCursorX < LINE_COUNTER_WIDTH) currentCursorX = LINE_COUNTER_WIDTH;
    if(currentCursorX >= VIDEO_COLS) currentCursorX = VIDEO_COLS - 1;

    currentFileArena->file->prevChar = 
        currentFileArena->file->cursorCol > 0 
        ? currentFileArena->file->currentLine->buffer[currentFileArena->file->cursorCol - 1]
        : 0;

    currentFileArena->file->currentChar = 
        currentFileArena->file->cursorCol > 0 
        ? currentFileArena->file->currentLine->buffer[currentFileArena->file->cursorCol]
        : 0;

    currentFileArena->file->nextChar = 
        currentFileArena->file->cursorCol < currentFileArena->file->currentLine->length
        ? currentFileArena->file->currentLine->buffer[currentFileArena->file->cursorCol + 1]
        : 0;
}

void _ensureHorizontalScroll(){
    int visualCursor = 0;
    int visualScroll = 0;
    int displayWidth = 0;

    if (!currentFileArena || !currentFileArena->file) return;

    visualCursor = _calculateVisualOffset(currentFileArena->file->cursorCol);
    visualScroll = _calculateVisualOffset(currentFileArena->file->scrollX);
    displayWidth = VIDEO_COLS - LINE_COUNTER_WIDTH;

    // If cursor is to the left of the visible area
    if (visualCursor < visualScroll) {
        currentFileArena->file->scrollX = currentFileArena->file->cursorCol;
    } 
    // If cursor is to the right of the visible area
    else if (visualCursor >= visualScroll + displayWidth) {
        // We move scrollX forward until the cursor is visible
        while (_calculateVisualOffset(currentFileArena->file->scrollX) + displayWidth <= visualCursor) {
            currentFileArena->file->scrollX++;
        }
    }
}

void _updateCursor(){
    if (!currentFileArena || !currentFileArena->file) return;

    _updateCurrentCursorY();
    _updateCurrentCursorX();

    ed_putCursor(currentCursorX, currentCursorY);
    ed_renderEvent = true;
}
// We reset the cursor to X:0 Y:0 relative to the active currentFileArena text area
void ed_resetCursor(){
    if (!currentFileArena || !currentFileArena->file) return;

    // In th future, when the text area became a movable element we will have to 
    // calculate the cursor position relative to the text area position.
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

    // If the number of lines is less than the screen heightc
    // and the current Y cursor position is less than the nmber of lines
    // DANGEROUS

    if( y > 0 && !(
        currentFileArena->file->cursorLine < currentFileArena->file->lines->length - 1
    )
)    return;

    // We check if we are in the current line
    if(! currentFileArena->file->currentLineNode){
        logger("[ed_moveCursor]: CurrentLineNode is NULL");
        return;
    }

    // VERTICAL SCROLLING ==============================================================

    // If the cursor is at 0 and we want to scroll up
    if(y && currentFileArena->file->cursorLine + y >= 0){       
        // IF the cursor is moved by 1 step, then we move the data between nodes by one node
        if(
            y == -1 &&
            currentFileArena->file->currentLineNode->prev 
        ){
            tempNode = currentFileArena->file->currentLineNode;
            currentFileArena->file->currentLineNode = tempNode->prev;
            
            currentFileArena->file->cursorCol = 
                (currentFileArena->file->prevLine->length < currentFileArena->file->cursorCol) 
                ? currentFileArena->file->prevLine->length - 1
                : currentFileArena->file->cursorCol;

            currentFileArena->file->prevLine = 
                currentFileArena->file->currentLineNode->prev &&
                currentFileArena->file->currentLineNode->prev->data ?
                currentFileArena->file->currentLineNode->prev->data
                :
                NULL;
            currentFileArena->file->currentLine = currentFileArena->file->currentLineNode->data;
            currentFileArena->file->nextLine = 
                currentFileArena->file->currentLineNode->next &&
                currentFileArena->file->currentLineNode->next->data ?
                currentFileArena->file->currentLineNode->next->data
                :
                NULL;
        }

        if( 
            y == 1 &&
            currentFileArena->file->currentLineNode->next
        ){
            tempNode = currentFileArena->file->currentLineNode;
            currentFileArena->file->currentLineNode = tempNode->next;
            
            currentFileArena->file->cursorCol = 
                (currentFileArena->file->nextLine->length < currentFileArena->file->cursorCol) 
                ? currentFileArena->file->nextLine->length - 1
                : currentFileArena->file->cursorCol;

            currentFileArena->file->prevLine = 
                currentFileArena->file->currentLineNode->prev &&
                currentFileArena->file->currentLineNode->prev->data ?
                currentFileArena->file->currentLineNode->prev->data
                :
                NULL;
            currentFileArena->file->currentLine = currentFileArena->file->currentLineNode->data;
            currentFileArena->file->nextLine = 
                currentFileArena->file->currentLineNode->next &&
                currentFileArena->file->currentLineNode->next->data ?
                currentFileArena->file->currentLineNode->next->data
                :
                NULL;
        }

        
        currentFileArena->file->cursorLine += y;
        // If the cursor is closer to the bottom
        if( (currentCursorY >= VIDEO_ROWS - 1 && y > 0) || (currentCursorY <= currentFileArena->file->scrollY - currentFileArena->file->cursorLine && y < 0) ){
            currentFileArena->file->scrollY += y;
            
            // If the cursor is at the bottom, the first line counter + number of rows in screen are less that the total of lines of the file
            // we proceed to scroll down
        }

        // Cursor col update 
        if (currentFileArena->file->currentLine->length < currentFileArena->file->cursorCol){
            currentFileArena->file->cursorCol = currentFileArena->file->currentLine->length;
        }
    }
    // END VERTICAL SCROLLING =====================================================

    // HORIZ, SCROLLING ===========================================================
    if(x){
        if(currentFileArena->file->cursorCol + x <= 0){
            currentFileArena->file->cursorCol = 0;
        } else if(
            currentFileArena->file->cursorCol + x > 0 && 
            currentFileArena->file->cursorCol + x <= (int)currentFileArena->file->currentLine->length
        ){  
            currentFileArena->file->cursorCol +=x;

        }else if(currentFileArena->file->cursorCol + x >= MAX_FILE_LINE_LENGTH){
            currentFileArena->file->cursorCol = MAX_FILE_LINE_LENGTH - 1;
        }

        _ensureHorizontalScroll();
    }
    
    if (y) {
        _ensureHorizontalScroll();
    }
    // END HORIZ, SCROLLING =====================================================

    _updateCursor();
}

void ed_renderElements(){
    int i = 0;
    int editor_size;
    if (!currentFileArena || !currentFileArena->file) return;

    dw_writeBufferEditorFormatted(editormemptr, 0, 0, VIDEO_COLS - 1, VIDEO_ROWS - 2, COLOR_LIGHT_GRAY, COLOR_BLACK, currentFileArena->file);
     
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

    x = currentFileArena->file->cursorCol;
    y = currentFileArena->file->cursorLine;
    
    if (x < 0) x = 0;
    if (y < 0) y = 0;
        

    node = currentFileArena->file->currentLineNode;
    
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

    currentFileArena->file->cursorCol++;

    currentFileArena->file->isModified = true;
    
    ed_markActive(ED_ACTIVITY_TYPE);
    _ensureHorizontalScroll();
    _updateCursor();
}

// Deletes a selectrion
void ed_backspaceSelection(){

    //if(currentFileArena->selectedStartLine == currentFileArena->selectedEndLine){
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

    file = currentFileArena->file;

    x = file->cursorCol;
    y = file->cursorLine;

    if (x < 0) x = 0;
    if (y < 0) y = 0;

    node = file->currentLineNode;

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
        line = file->currentLine;
        // wE Delete the current line but also we need to copy the current line content to 
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
            memcpy(prevLine->buffer + prevLine->length, line->buffer, line->length);
            prevLine->length += line->length ;
        }   
        
        currentFileArena->file->cursorCol = prevLine->length;

        // We delete the line by moving it to deleted lines and we re join the nodes without the current line
        if(node->next){
            prevNode->next = node->next;
            node->next->prev = prevNode;
        } else {
            // We were at the last node, so prevNode becomes the new last node
            currentFileArena->file->lines->lastNode = prevNode;
            if(prevNode) prevNode->next = NULL;
        }
        
        currentFileArena->file->lines->length--;
        
        // Updating line metadata BEFORE recycling node
        currentFileArena->file->currentLineNode = prevNode;

        // RECYCLE THE NODE
        node->next = NULL;
        node->prev = NULL;
        node->isDeleted = true;
        addToList(&currentFileArena->file->deletedLines, node, NULL, currentFileArena->arena);
        
        if(currentCursorY > 0){
            currentCursorY--;
        } else if(currentFileArena->file->scrollY > 0){
            currentFileArena->file->scrollY--;
        }
         
        // Updating line metadata
        currentFileArena->file->cursorLine = 
            currentFileArena->file->cursorLine > 0 
            ?   currentFileArena->file->cursorLine - 1
            :   0;

        currentFileArena->file->prevLine = 
            currentFileArena->file->currentLineNode->prev
            ? (Line*) currentFileArena->file->currentLineNode->prev->data
            : NULL;

        currentFileArena->file->currentLine = (Line *)currentFileArena->file->currentLineNode->data;

        currentFileArena->file->nextLine = 
            currentFileArena->file->currentLineNode->next
            ? (Line*) currentFileArena->file->currentLineNode->next->data
            : NULL;

    }else{
        if(currentFileArena->file->cursorCol > 0){
            memcpy(
                currentFileArena->file->currentLine->buffer + x - 1,
                currentFileArena->file->currentLine->buffer + x, 
                currentFileArena->file->currentLine->length - x
            );

            currentFileArena->file->currentLine->length--;
            currentFileArena->file->currentLine->buffer[currentFileArena->file->currentLine->length] = '\0';
            currentFileArena->file->cursorCol--;

            _ensureHorizontalScroll();
        }
    }

    currentFileArena->file->isModified = true;      

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

    file = currentFileArena->file;

    x = currentFileArena->file->cursorCol;
    
    node = file->currentLineNode;
    line = (Line *)node->data;
    
    if (x >= line->length) return;
    
    memcpy(line->buffer + x, line->buffer + x + 1, line->length - x);    
    line->length--;
    line->buffer[line->length] = '\0';

    currentFileArena->file->isModified = true;

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
    

    arena = currentFileArena->arena;

    x = currentFileArena->file->cursorCol;

    prevLineTabs = _get_tab_counts_until(currentFileArena->file->currentLine->length);

    // Creating the new line then zeroing
    // RECYCLING/NEW LINE LOGIC ========================================================================
    newLineNode = pop(&currentFileArena->file->deletedLines);
    
    if(newLineNode && currentFileArena->file->deletedLines->length > 0){
        logger("[ed_newLine]: reusing deleted line");
        newLine = (Line*)newLineNode->data;
        memset(newLine->buffer, '\0', MAX_FILE_LINE_LENGTH);
        newLine->length = 0;
        newLineNode->isDeleted = false; 
    } else {
        newLine = (Line*)mem_arena_alloc(arena, NULL, sizeof(Line));

        if (!newLine){
            logger("[ed_newLine]: Could not make new line");
            return;
        }

        newLine->length = 0;
        newLine->buffer = (char*)mem_arena_alloc(arena, NULL, sizeof(char) * MAX_FILE_LINE_LENGTH);

        if (!newLine->buffer){
            logger("[ed_newLine]: Could not alloc new line buffer");
            return;
        }

        memset(newLine->buffer, '\0', MAX_FILE_LINE_LENGTH);
        
        newLineNode = (Node *)mem_arena_alloc(arena, NULL, sizeof(Node));

        
        if (!newLineNode){
            logger("[ed_newLine]: Could not alloc new line NODE");
            return;
        }
        
        newLineNode->data = newLine;
    }

    // ==============================

    // We copy the content from the current line in the current cursor position onwards to the now line
    // We detect opening and closing of function
        
    autoClosePos = _get_auto_close_pos();

    if(currentFileArena->file->prevChar == '{'){
        isIndent = true;
        autoIdentMovement = (int)isIndent + prevLineTabs;
    }else if(currentFileArena->file->currentChar == '}'){
        isAutoClose = true;
        autoIdentMovement = autoClosePos;
    }else{
        autoIdentMovement = prevLineTabs;
    }

    logger("[ed_newLine]: AutoclosePos %d", autoClosePos);

    copyLen = 
        (x <= currentFileArena->file->currentLine->length)
        ?
            currentFileArena->file->currentLine->length - x + autoIdentMovement
        :
            0
        ;
        
    memcpy(newLine->buffer + autoIdentMovement, currentFileArena->file->currentLine->buffer + x, copyLen);
    newLine->length = copyLen;

    // Copy prev line tabs
    if(autoIdentMovement > 0) memset(newLine->buffer , CHAR_TAB, autoIdentMovement);

    if(isAutoClose == true){
        memset(newLine->buffer , CHAR_TAB, autoClosePos);
        newLine->buffer[autoClosePos] = '}';
    }         

    // We clear the current line position onwards
    memset(currentFileArena->file->currentLine->buffer + x, '\0', MAX_FILE_LINE_LENGTH - x);
    
    currentFileArena->file->currentLine->length = x;

    // Pointer logic
    // newLineNode is already allocated or recycled above
    newLineNode->prev = currentFileArena->file->currentLineNode;
    newLineNode->next =
        currentFileArena->file->currentLineNode->next 
        ?
            currentFileArena->file->currentLineNode->next 
        :
            NULL
        ;

    if(newLineNode->next) newLineNode->next->prev = newLineNode;
 

    currentFileArena->file->currentLineNode->next = newLineNode;
    currentFileArena->file->currentLineNode = newLineNode;

    // Update List structure
    if(!newLineNode->next){
        currentFileArena->file->lines->lastNode = newLineNode;
    }
    currentFileArena->file->lines->length++;

    currentFileArena->file->cursorLine = currentFileArena->file->scrollY + currentCursorY;

    currentFileArena->file->prevLine = 
        currentFileArena->file->currentLineNode->prev &&
        currentFileArena->file->currentLineNode->prev->data
        ?
            currentFileArena->file->currentLineNode->prev->data
        :
            NULL    
        ;

    currentFileArena->file->currentLine = currentFileArena->file->currentLineNode->data;
    
    // Next line logic
    currentFileArena->file->nextLine = 
        currentFileArena->file->currentLineNode->next &&
        currentFileArena->file->currentLineNode->next->data
        ?
            currentFileArena->file->currentLineNode->next->data
        :
            NULL
        ;

    
    /* Move cursor down and scroll if necessary */
    if (currentCursorY + 1 >= VIDEO_ROWS) {
        currentFileArena->file->scrollY++;
    } else {
        currentCursorY++;
    }

    currentFileArena->file->cursorCol = 0 + autoIdentMovement;
    currentFileArena->file->scrollX = 0;
    currentFileArena->file->cursorLine++;

    // activity flags
    currentFileArena->file->isModified = true;
    
    ed_markActive(ED_ACTIVITY_NEWLINE);
    _updateCursor();
}

// PROMPT ELEMENT
char *ed_scanf(unsigned char x, unsigned char y, unsigned char maxChars ){
    int i = 0;
    int j = 0;
    int lenbuff = 0;
    char c = 0;
    bool esc = false;

    static char buffer[MAX_FILE_LINE_LENGTH];

    memset(buffer, '\0', MAX_FILE_LINE_LENGTH);

    ed_putCursor(x,y);    

    while(c != CHAR_ENTER && !(esc = hal_inp_isKeyPressed(HAL_KEY_ESC) == true)){
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
                // Shift to the left the buffer from the current position 
                lenbuff = strlen(buffer);
                for(j=i; j <= lenbuff; j++){
                    buffer[j] = buffer[j+1];
                }

                // Redraw he entire prompt by copying the buffer content to the screen buffer
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

                // Redraw he entire prompt by copying the buffer content to the screen buffer
                for(j=0;j < maxChars; j++){
                    dw_charXY(textmemptr,buffer[j], x+j, y);
                }   

                ed_putCursor(x + i, y);
            }else if(c == CHAR_SPACE){                
                for(j=strlen(buffer); j >= i; j--){
                    if(j + 1 < MAX_FILE_LINE_LENGTH && j + 1 < maxChars ){
                        buffer[j + 1] = buffer[j];
                    }
                }

                buffer[i] = ' ';

                // Redraw he entire prompt by copying the buffer content to the screen buffer
                for(j=0;j <  maxChars; j++){
                    dw_charXY(textmemptr,buffer[j], x+j, y);
                }   
                
                i++;
                ed_putCursor(x + i, y);
            } else if (c >= 32 && i < maxChars && i < MAX_FILE_LINE_LENGTH - 1){
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
char *ed_async_scanf(unsigned char x, unsigned char y, unsigned char maxChars, char *buffer, size_t bufflen, int *stepIndex){
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
        } else if (c >= 32 && (*stepIndex) < charLimit && (*stepIndex) < MAX_FILE_LINE_LENGTH - 1){
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
    currentFileArena->file->cursorCol = currentFileArena->file->currentLine->length;

    _ensureHorizontalScroll();
    _updateCursor();
}

void ed_putCursorStart(){   
    currentFileArena->file->cursorCol = 0;

    _ensureHorizontalScroll();
    _updateCursor();
}

void ed_putCursorFistLine(){
    int lineposX;

    currentFileArena->file->currentLineNode = currentFileArena->file->lines->firstNode;

    currentFileArena->file->prevLine =
        currentFileArena->file->currentLineNode &&
        currentFileArena->file->currentLineNode->prev 
        ?
            currentFileArena->file->currentLineNode->prev->data
        :
            NULL
        ;
        
    currentFileArena->file->currentLine = currentFileArena->file->currentLineNode->data;
    
    currentFileArena->file->nextLine =
        currentFileArena->file->currentLineNode &&
        currentFileArena->file->currentLineNode->next
        ?
            currentFileArena->file->currentLineNode->next->data
        :
            NULL
        ;
    
    lineposX = 
        (int)currentFileArena->file->currentLine->length - 1 < currentCursorX - LINE_COUNTER_WIDTH
        ?
            LINE_COUNTER_WIDTH + (int)currentFileArena->file->currentLine->length - 1
        :
            currentCursorX
        ;
    
    currentCursorX = lineposX;
    currentCursorY = 0;
    
    // then we need to reset some flags so we can redraw the screen properly
    /* Sync file cursor */
    currentFileArena->file->scrollY = 0;
    currentFileArena->file->cursorLine = 0;
    currentFileArena->file->cursorCol = currentCursorX - LINE_COUNTER_WIDTH;
    

    _updateCursor();
}

void ed_putCursorLastLine(){
	int lineJump = 0;
	Node *newLineNode = NULL;
	Line *newLine = NULL;

    if(
		currentFileArena->file->lines->length > 0 &&
		currentFileArena->file->cursorLine < currentFileArena->file->lines->length
	){
		lineJump = 
			(currentFileArena->file->lines->length < VIDEO_ROWS)	
			? currentFileArena->file->lines->length - 1
			: VIDEO_ROWS - 1;
			 
		currentFileArena->file->cursorLine += lineJump;
		currentFileArena->file->scrollY += 
			currentFileArena->file->lines->length < VIDEO_ROWS
			? 0 
			: lineJump;	
	}
	
	newLineNode = currentFileArena->file->lines->lastNode;

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

	currentFileArena->file->currentLineNode = newLineNode;
	currentFileArena->file->currentLine = newLine;

	currentFileArena->file->prevLine = 
		currentFileArena->file->currentLineNode->prev 
		? (Line*) currentFileArena->file->currentLineNode->prev->data
		: NULL;

	currentFileArena->file->nextLine = 
		currentFileArena->file->currentLineNode->next
		? (Line*) currentFileArena->file->currentLineNode->next->data
		: NULL;
	
	// If on the new line the previous position is larger than the new line length, then we do the following
	if(currentFileArena->file->cursorCol >= currentFileArena->file->currentLine->length - 1){
		currentFileArena->file->cursorCol = currentFileArena->file->currentLine->length - 1;
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
	if(ed_checkStatusBarMessage() == true){
		dw_writeBuffer(textmemptr, "%s", 0, VIDEO_ROWS - 1, VIDEO_COLS - 1, VIDEO_ROWS - 1, settings.STATUSBAR_COLOR_TEXT,settings.STATUSBAR_COLOR_BG, statusBarMessage);            
	}else if (currentFileArena && currentFileArena->file) {
		dw_writeBuffer(textmemptr, "Line %d, Col %d %c", 0, VIDEO_ROWS - 1, 39, VIDEO_ROWS - 1, settings.STATUSBAR_COLOR_TEXT,settings.STATUSBAR_COLOR_BG, currentFileArena->file->cursorLine + 1, currentFileArena->file->cursorCol + 1, 179, currentFileArena->file->currentLine->length);
		dw_writeBuffer(textmemptr, " %s", 40, VIDEO_ROWS - 1, VIDEO_COLS - 1, VIDEO_ROWS - 1, settings.STATUSBAR_COLOR_TEXT,settings.STATUSBAR_COLOR_BG, currentFileArena->file->name);
	}else{
		dw_writeBuffer(textmemptr, "No open files", 0, VIDEO_ROWS - 1, VIDEO_COLS - 1, VIDEO_ROWS - 1, settings.STATUSBAR_COLOR_TEXT,settings.STATUSBAR_COLOR_BG);
	}        
}

void ed_wordJump(short wordJump){
    size_t currentCharPos = 0;
    char *currentLineBuffer = NULL;

    currentCharPos = currentFileArena->file->cursorCol;
    currentLineBuffer = currentFileArena->file->currentLine->buffer;

    switch(wordJump){
        case ED_WORD_JUMP_PREV:
            if((currentCharPos - 1) > 0 && currentLineBuffer[currentCharPos] == ' ') currentCharPos--;
            
            while(
                (currentCharPos - 1 > 0) &&
                (currentLineBuffer[currentCharPos] != ' ' ||
                currentLineBuffer[currentCharPos - 1] == ' ')){
                    currentCharPos--;
            }
            break;
        case ED_WORD_JUMP_NEXT:
            if(currentCharPos < currentFileArena->file->currentLine->length && currentLineBuffer[currentCharPos] == ' ') currentCharPos++;

            while(
                (currentCharPos + 1 < currentFileArena->file->currentLine->length) &&
                (currentLineBuffer[currentCharPos] != ' ' ||
                currentLineBuffer[currentCharPos + 1] == ' ')){
                    currentCharPos++;
            }
            break;
    }
    
    currentFileArena->file->cursorCol = currentCharPos;
    _updateCursor();
}

// This is for swapping the lines with the next or previous one with the
// ALT + UP or ALT + DOWN key stroke.
// Similar to visual studio code editor feature

void ed_swapLine(short lineJump){
    Node *tmpNext = NULL;
    Node *tmpPrev = NULL;
    Node *curr = currentFileArena->file->currentLineNode;
    switch(lineJump){
        case ED_LINE_JUMP_DOWN:
            if(curr->next == NULL) break;
            curr = currentFileArena->file->currentLineNode->next;
            currentFileArena->file->cursorLine += 2;
            if(currentCursorY + 1 < VIDEO_ROWS ) currentCursorY+=2; 
            /* FALLTHROUGH ...*/
        case ED_LINE_JUMP_UP:
            tmpNext = curr->next;
		    tmpPrev = curr->prev;

            // Is head
            if(curr->prev == NULL) break;

			if(tmpPrev->prev != NULL){
				tmpPrev->prev->next = curr;
			}

			curr->prev = tmpPrev->prev;

			curr->next = tmpPrev;
			tmpPrev->prev = curr;
			tmpPrev->next = tmpNext;

            if(tmpNext != NULL){
                tmpNext->prev = tmpPrev;            
            }

            currentFileArena->file->cursorLine--;

            if(currentCursorY - 1 > 0) currentCursorY--; 

            break;
    }

    ed_markActive(lineJump);

    _updateCursor();
}

void ed_prepareSelectionTool(){
    
    if(currentFileArena && currentFileArena->file){
        currentFileArena->file->oldLineNode = (struct Node *)currentFileArena->file->currentLineNode;
        currentFileArena->file->oldLine = currentFileArena->file->cursorLine;
        currentFileArena->file->oldCol = currentFileArena->file->cursorCol;
    }
}

void ed_clearSelection(){
    if(!currentFileArena || !currentFileArena->file) return;
    currentFileArena->file->selectedStartNode = NULL;
    currentFileArena->file->selectedEndNode = NULL;
    currentFileArena->file->selectedStartX = 0;
    currentFileArena->file->selectedEndX = 0;
    currentFileArena->file->selectedStartLine = 0;
    currentFileArena->file->selectedEndLine = 0;
    on_selection_tool = false;
}

void ed_handleSelection() {
    File *file;
    bool isNav;

    if(!currentFileArena || !currentFileArena->file) return;
    
    file = currentFileArena->file;

    // Check if cursor actually moved
    if (file->currentLineNode == file->oldLineNode && file->cursorCol == file->oldCol) {
        return;
    }

    isNav = hal_inp_isKeyDown(HAL_KEY_UP) || hal_inp_isKeyDown(HAL_KEY_DOWN) ||
            hal_inp_isKeyDown(HAL_KEY_LEFT) || hal_inp_isKeyDown(HAL_KEY_RIGHT) ||
            hal_inp_isKeyDown(HAL_KEY_HOME) || hal_inp_isKeyDown(HAL_KEY_END) ||
            hal_inp_isKeyDown(HAL_KEY_PAGEUP) || hal_inp_isKeyDown(HAL_KEY_PAGEDOWN);

    if (isNav) {
        if (hal_inp_isKeyDown(HAL_KEY_LSHIFT) || hal_inp_isKeyDown(HAL_KEY_RSHIFT)) {
            // If selection is not active, anchor it at the old position
            if (file->selectedStartNode == NULL) {
                file->selectedStartNode =   file->oldLineNode;
                file->selectedStartX = file->oldCol;
                file->selectedStartLine = file->oldLine;
            }
            // Always update selection end to the new position
            file->selectedEndNode = file->currentLineNode;
            file->selectedEndX = file->cursorCol;
            file->selectedEndLine = file->cursorLine;
            on_selection_tool = true;
        } else {
            // Clear selection since we moved cursor without Shift
            ed_clearSelection();
        }
    } else {
        // Any other cursor movement (e.g. typing, backspace, new line) clears selection
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
    char *detectedWordOffset = NULL;
    char *wordIndexPtr = NULL;
    char searchArenaName[32];
    int wordLen = 0;
    unsigned int lineIndex = 0;
    WordMetadata *matchBuffer = NULL;
    Node *lineNode = NULL;

    if(
        !currentFileArena ||
        !currentFileArena->file ||
        !currentFileArena->file->lines ||
        !currentFileArena->file->lines->firstNode
    ){

        logger("[ed_findWord]: currentFileArena first line node is NULL");
        return;
    }

    lineNode = currentFileArena->file->lines->firstNode;
	
    if(!lineNode){
        logger("[ed_findWord]: lineNode is NULL");
        return;
    }
    
    if(!currentFileSearch){
        logger("[ed_findWord]: currentFileSearch is NULL");
        return;
    }

    if(!currentFileSearch->arena || !currentFileSearch->arena->base){
        sprintf(searchArenaName, "SRCH%d", currentFileArena->file->fileIndex);
        currentFileSearch->arena = (MemoryArena *)mem_create_arena(searchArenaName, MEM_ARENA_METADATA, MEM_ARENA_2K);
    } else {
        mem_arena_reset(currentFileSearch->arena->name);
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
    logger("[ed_findWord]: Current word! : %s", currentFileSearch->dialogInputBuffer);

    while(lineNode != NULL){
        
        if(
            !lineNode->data ||
            !((Line*)lineNode->data)->buffer
        ){
            logger("[ed_findWord]: lineNode->data or lineNode->data->buffer is NULL");
            return;
        }
        
        wordIndexPtr = ((Line*)(lineNode->data))->buffer;
        // No words!Wing by spac
        if(*wordIndexPtr == '\0'){
            lineNode = lineNode->next;
            lineIndex++;
            continue;
        } 

        detectedWordOffset = strstr(wordIndexPtr, currentFileSearch->dialogInputBuffer);
        
        while(detectedWordOffset){        
            matchBuffer = (WordMetadata *) mem_arena_alloc(currentFileSearch->arena, NULL, sizeof(WordMetadata));
     
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
            
            addGenericNode(&currentFileSearch->words, matchBuffer, NULL, currentFileSearch->arena);
            
            currentFileSearch->wordCount++;
  
            detectedWordOffset = strstr(detectedWordOffset + wordLen, currentFileSearch->dialogInputBuffer);
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
    int vis_offset = 0;
    int dialogStartY = 0;
    
    vis_offset = (VIDEO_COLS / 4);
    dialogStartY = 2;

    dw_rectangle(textmemptr, vis_offset, dialogStartY, VIDEO_COLS - vis_offset, 6, COLOR_BLUE, COLOR_WHITE, ' ', COLOR_WHITE, COLOR_BLUE, false, DRAW_BORDER_SIMPLE, "Search...");
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

    ed_async_scanf(vis_offset + 1, 3, (2 * vis_offset) - 1, currentFileSearch->dialogInputBuffer, strlen(currentFileSearch->dialogInputBuffer), &(currentFileSearch->dialogInputIndex));
            
}

void ed_searchMoveCursor(){
    if(
        !currentFileSearch->words ||
        !currentFileSearch->currentWordNode ||
        !currentFileSearch->currentWordNode->data
    ) return;
    
    if(hal_inp_isKeyDown(HAL_KEY_ENTER) && !hal_inp_isKeyDown(HAL_KEY_LSHIFT)){
        // We go forward
        currentFileSearch->currentWordNode = 
            currentFileSearch->currentWordNode &&
            currentFileSearch->currentWordNode->next
            ? currentFileSearch->currentWordNode->next
            : currentFileSearch->currentWordNode ;        

    }else if (hal_inp_isKeyDown(HAL_KEY_ENTER) && hal_inp_isKeyDown(HAL_KEY_LSHIFT)){
        // We go back         
        currentFileSearch->currentWordNode = 
            currentFileSearch->currentWordNode &&
            currentFileSearch->currentWordNode->prev
            ? currentFileSearch->currentWordNode->prev
            : currentFileSearch->currentWordNode ;        
    }

    // We update the cursor
    currentFileArena->file->currentLineNode = 
        currentFileSearch->currentWordNode &&
        currentFileSearch->currentWordNode->data &&
        ((WordMetadata*) currentFileSearch->currentWordNode->data)->lineNode
        ? ((WordMetadata *)currentFileSearch->currentWordNode->data)->lineNode
        : NULL;

    currentFileArena->file->cursorCol = 
        currentFileSearch->currentWordNode &&
        currentFileSearch->currentWordNode->data &&
        ((WordMetadata*) currentFileSearch->currentWordNode->data)->cursorCol
        ? ((WordMetadata *)currentFileSearch->currentWordNode->data)->cursorCol
        : 0;

    currentFileArena->file->cursorLine =
        currentFileSearch->currentWordNode &&
        currentFileSearch->currentWordNode->data &&
        ((WordMetadata*) currentFileSearch->currentWordNode->data)->cursorLine
        ? ((WordMetadata *)currentFileSearch->currentWordNode->data)->cursorLine
        : 0;

    _updateScrollY();
    _updateCursor();
}
void ed_prepareSearchTool(){
    if(hal_inp_keysPressed(HAL_INP_TRIGGER_EDGE, 2, HAL_KEY_LCTRL, HAL_KEY_F)) ed_onSearchTool = true;
		
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

    memset(cmd, '\0', 255);
    memset(currPath, '\0', 255);

#if defined(__MSDOS__) || defined(__WATCOMC__)
    comspec = getenv("COMSPEC");
#else
    comspec = getenv("SHELL");
#endif

    if(!currentFileArena || !currentFileArena->file) return;
    
    hal_inp_closeKeyboard();

    hal_vid_set25Lines();
    dw_cls(textmemptr);

    // TODO: SAVE FILE

#if defined(__MSDOS__) || defined(__WATCOMC__)
    if (!comspec) comspec = "COMMAND.COM";
#else
    if (!comspec) comspec = "/bin/bash";
#endif

    strncpy(currPath, currentFileArena->file->name, hal_fs_getFilePath(currentFileArena->file->name));

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