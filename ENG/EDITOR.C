/*
    This is a basic TUI library, right now just for testing purposes
    By Vincebus Riveruptum
    2026

    Credits osdever.net

    http://www.osdever.net/bkerndev/Docs/printing.htm
*/

#include "EDITOR.H"
#include "FILES.H"

//unsigned char attrib = 0x07; // Default attribute: White on Black
struct Container *root;

unsigned char VIDEO_COLS = 80;
unsigned char VIDEO_ROWS = 25;

unsigned char currentCursorX = 0;
unsigned char currentCursorY = 0;

bool ed_renderEvent = false;

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


int _get_tab_counts_until(int col){
    int i = 0, tabCount = 0;

    if (!currentFileArena->file->currentLine) return 0;

    while(i < col && i < currentFileArena->file->currentLine->length){
        if(currentFileArena->file->currentLine->buffer[i] == CHAR_TAB) tabCount++;
        i++; 
    }
    
    return tabCount;
}

int _get_tab_counts_someline(Line *someLine, int col){
    int i = 0, tabCount = 0;

    if (!someLine) return 0;

    while(i < col && i < someLine->length){
        if(someLine->buffer[i] == CHAR_TAB) tabCount++;
        i++; 
    }
    
    return tabCount;
}

int _get_auto_close_pos(){
    int tabCount = 0;
    Node *travelingBackwards = currentFileArena->file->currentLineNode;

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
    int i = 0, tabCount = 0;
    char c, cnext;

    do{
        c = currentFileArena->file->currentLine->buffer[i];

        if (c == CHAR_TAB){
            tabCount++;

            cnext = currentFileArena->file->currentLine->buffer[i + 1];

            if(cnext == NULL && cnext != CHAR_TAB){
                return tabCount;
            }
        }
        i++;
    }while (c != '\0');

    return tabCount;
}

void _updateCurrentCursorY(){
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
        : currentFileArena->file->currentLine->length;
}

void _ensureHorizontalScroll(){
    int visualCursor = _calculateVisualOffset(currentFileArena->file->cursorCol);
    int visualScroll = _calculateVisualOffset(currentFileArena->file->scrollX);
    int displayWidth = VIDEO_COLS - LINE_COUNTER_WIDTH;

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
    _updateCurrentCursorY();
    _updateCurrentCursorX();

    ed_putCursor(currentCursorX, currentCursorY);
    ed_renderEvent = true;
}
// We reset the cursor to X:0 Y:0 relative to the active currentFileArena text area
void ed_resetCursor(){

    // In th future, when the text area became a movable element we will have to 
    // calculate the cursor position relative to the text area position.
    _updateCursor();
}

void ed_putCursor(unsigned char x, unsigned char y){
    unsigned short temp;

    currentCursorX = x;
    currentCursorY = y;
    /* The equation for finding the index in a linear
    *  chunk of memory can be represented by:
    *  Index = [(y * width) + x] */
    temp = currentCursorY * VIDEO_COLS + currentCursorX;

    /* This sends a command to indicies 14 and 15 in the
    *  CRT Control Register of the VGA controller. These
    *  are the high and low bytes of the index that show
    *  where the hardware cursor is to be 'blinking'. To
    *  learn more, you should look up some VGA specific
    *  programming documents. A great start to graphics:
    *  http://www.brackeen.com/home/vga */
    outPortb(0x3D4, 14);
    outPortb(0x3D5, temp >> 8);
    outPortb(0x3D4, 15);
    outPortb(0x3D5, temp);
}

/*
    This is the cursor behavior when is inside a TEXT AREA
*/
void ed_moveCursor(short x, short y){
    Node *tempNode;
    Line *tempLine;
    int nextLineStartIndex = 0, prevLineStartIndex = 0;
    char c;

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
            currentFileArena->file->cursorCol + x <= currentFileArena->file->currentLine->length
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
    el_renderFiles_test();
}


void ed_typeChar(char c){
    // We type the char at 
    // X : currentCursorX + LINE_COUNTER_WIDTH + 1
    // Y : currentCursorY + file->scrollY + 1 
    Node *node;
    Line *line;

    int x = 0;
    int y = 0;
    int i;

    File *file;
    file = currentFileArena->file;

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
    
    _ensureHorizontalScroll();
    _updateCursor();
}

void ed_backspace(){
        // We type the char at 
    // X : currentCursorX + LINE_COUNTER_WIDTH + 1
    // Y : currentCursorY + file->scrollY + 1 
    Node *node, *prevNode;
    Line *line, *prevLine;
    char c;

    int x = 0;
    int y = 0;

    File *file;
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
            c = currentFileArena->file->currentLine->buffer[currentFileArena->file->cursorCol - 1];

            memcpy(currentFileArena->file->currentLine->buffer + x - 1, currentFileArena->file->currentLine->buffer + x, currentFileArena->file->currentLine->length - x);
            currentFileArena->file->currentLine->length--;
            
            currentFileArena->file->cursorCol--;
            _ensureHorizontalScroll();
        }
    }

    currentFileArena->file->isModified = true;      
    _updateCursor();
}
void ed_supr(){
        // We type the char at 
    // X : currentCursorX + LINE_COUNTER_WIDTH + 1
    // Y : currentCursorY + file->scrollY + 1 
    Node *node;
    Line *line;

    int x = 0;
    int y = 0;

    File *file;
    file = currentFileArena->file;

    x = currentFileArena->file->cursorCol;
    y = currentFileArena->file->cursorLine;
    
    if (x < 0) x = 0;
    if (y < 0) y = 0;

    node = file->currentLineNode;
    line = (Line *)node->data;
    
    if (x >= line->length) return;
    
    memcpy(line->buffer + x, line->buffer + x + 1, line->length - x);    
    line->length--;

    currentFileArena->file->isModified = true;
    _updateCursor();
}

void ed_newLine(){
    unsigned int x = 0;
    size_t copyLen = 0;
    Node *currentLineNode, *newLineNode;
    Line *newLine;
    MemoryArena *arena;
    bool isIndent = false;
    bool isAutoClose = false;
    int prevLineTabs = 0;
    int autoClosePos = 0;
    int autoIdentMovement = 0;
    

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

    _updateCursor();
}

// PROMPT ELEMENT
char *ed_scanf(unsigned char x, unsigned char y, unsigned char maxChars ){
    int i = 0, j = 0, lenbuff = 0;
    char c = 0;
    bool esc = false;

    static char buffer[MAX_FILE_LINE_LENGTH];

    memset(buffer, '\0', MAX_FILE_LINE_LENGTH);

    ed_putCursor(x,y);    

    while(c != CHAR_ENTER && !(esc = inp_isKeyPressed(KEY_ESC) == true)){
        c = getch();

        if(c == 0 || (unsigned char)c == 0xE0){
            c = getch();

            if(c == KEY_LEFT && i > 0){
                i--;
                ed_putCursor(x + i, y);
            } 
            if(c == KEY_RIGHT && i < strlen(buffer)){
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
    int j = 0, lenbuff = 0;
    char c = 0;
    bool esc = false;
    int charLimit;

    charLimit = bufflen >= maxChars ? maxChars : bufflen + 2;
    
    ed_putCursor(x + (*stepIndex),y);    
    
    // Redraw he entire prompt by copying the buffer content to the screen buffer
    for(j=0;j < maxChars; j++){
        if(j > bufflen){
            dw_charXY(textmemptr,' ', x+j, y);
        }else{
            dw_charXY(textmemptr,buffer[j], x+j, y);
        }
    }       

    c = getch();

    if(c == 0 || (unsigned char)c == 0xE0){
        c = getch();

        if(c == KEY_LEFT && (*stepIndex) > 0){
            (*stepIndex)--;
            ed_putCursor(x + (*stepIndex), y);
        } 
        if(c == KEY_RIGHT && (*stepIndex) < strlen(buffer)){
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
    Line *tmpLine;
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
        currentFileArena->file->currentLine->length - 1 < currentCursorX - LINE_COUNTER_WIDTH
        ?
            LINE_COUNTER_WIDTH + currentFileArena->file->currentLine->length - 1
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

    return;
}

bool ed_checkStatusBarMessage(){
    time_t endClock;

    if(statusBarMessage[0] == '\0') return false;
    if(ed_globalAuxTimer == 0) return false;

    time(&endClock);

    // 5 seconds of duration
    if(difftime(endClock, ed_globalAuxTimer) > 5){
        memset(statusBarMessage, '\0', ED_STATUSBAR_WIDTH - 1);
        ed_globalAuxTimer = 0;
        return false;
    }  
    
    return true;
}

// Statusbar drawing function
void ed_statusBar(){
    if(ed_checkStatusBarMessage() == true){
        dw_writeBuffer(textmemptr, "%s", 0, VIDEO_ROWS - 1, VIDEO_COLS - 1, VIDEO_ROWS - 1, settings.STATUSBAR_COLOR_TEXT,settings.STATUSBAR_COLOR_BG, statusBarMessage);            
    }else{
        dw_writeBuffer(textmemptr, "Line %d, Col %d %c", 0, VIDEO_ROWS - 1, 39, VIDEO_ROWS - 1, settings.STATUSBAR_COLOR_TEXT,settings.STATUSBAR_COLOR_BG, currentFileArena->file->cursorLine + 1, currentFileArena->file->cursorCol + 1, 179, currentFileArena->file->currentLine->length);
        dw_writeBuffer(textmemptr, " %s", 40, VIDEO_ROWS - 1, VIDEO_COLS - 1, VIDEO_ROWS - 1, settings.STATUSBAR_COLOR_TEXT,settings.STATUSBAR_COLOR_BG, currentFileArena->file->name);
    }        
}

void ed_wordJump(short wordJump){
    unsigned short currentCharPos = currentFileArena->file->cursorCol;
    char *currentLineBuffer = currentFileArena->file->currentLine->buffer;

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
   _updateCursor();
}

void ed_showFileSwitcher(){
    int i;
    int selectedIndex = 0;
    bool selected = false;
    FileArena *fileptr;
    
    inp_clearKeyboardBuffer();
    
    /* Encontrar el índice seleccionado inicial antes del bucle */
    for(i = 0; i < MAX_ARENAS; i++) {
        if(fileList[i].file != NULL && 
           strcmp(fileList[i].file->name, currentFileArena->file->name) == 0) {
            selectedIndex = i;
            break;
        }
    }

    while(inp_isKeyDown(KEY_LALT)){
        /* 1. Dibujar el cuadro */
        dw_rectangle(textmemptr, 4, 4, 34, 16, COLOR_RED, COLOR_WHITE, ' ', COLOR_WHITE, COLOR_RED, false, DRAW_BORDER_SIMPLE, NULL);

        /* 2. Dibujar la lista de archivos */
        for(i = 0; i < MAX_ARENAS; i++){
            fileptr = &fileList[i];
            if (fileptr->file != NULL) {
                selected = (i == selectedIndex);
                dw_writeBuffer(textmemptr, "%s %s", 5, 5 + i, 33, 5 + i, 
                               COLOR_WHITE, COLOR_RED, (selected ? "*" : " "), fileptr->file->name);
            }
        }

        /* 3. Detectar pulsación de borde de LSHIFT dentro del bucle */
        if(inp_keysPressed(INP_TRIGGER_EDGE, 2, KEY_LALT, KEY_LSHIFT)){
            /* Avanzar al siguiente archivo abierto válido */
            do {
                selectedIndex = (selectedIndex + 1) % MAX_ARENAS;
            } while(fileList[selectedIndex].file == NULL);

            currentFileArena = &fileList[selectedIndex];
        }

        /* 4. Actualizar el buffer del teclado manualmente en este bucle */
        inp_updateKeyboard();
        
        /* En DOS es bueno dar un brevísimo retardo/yield aquí para no saturar la CPU */
        delay(10); /* O similar */
    }

    ed_renderEvent = true;
}

int _goBackPath(char *path){
    int end, len;
    char *endptr;
    
    end = strlen(path) - 1;
    len = end + 1;

    if(len <= 3) return len;
    
    endptr = path + end;

    if(*endptr == '\\' ){
        *endptr = '\0';
        endptr--;
    }

    while(endptr > path && *(endptr) != '\\'){
        *endptr = '\0';
        endptr--;
    };
    
    return (int)(endptr - path) + 1;
}
/*
    This is basically VSCODE's quick open feature. The behavior is the following:
    Prompt that has pre-filled the absolute path of the current position, filalble with the left and right arrows
    Select file of the current selected path with the up and down arrows.
    The file list will be reactive depending on hte detected path from the input prompt.
*/
void ed_quickOpenFileDialog(){
    int i, stepIndex;
    int vis_offset = 0, dialog_offset = 0, dialogStartY, dialogEndY, dialogHeight;
    int entriesLen = 0;
    bool isSelected = false;
    int selectedEntry = 0;
    int selectedIndex = 0;
    char selectedEntryFullPath[255] = {'\0'};

    char currentPath[255] = {'\0'};
    Directory *currPathDirectory = NULL;
    Node *node;
    int entryIndex;
    FileEntry *fileEntry, *selectedFileEntry;
    int clearMarkPoint = 0;
    fs_getAbsoluteCurrentPath(currentPath, 255);
    
    if(currentPath[0] == '\0'){
        logger("[ed_quickOpenFileDialog]: Failed to retrieve currentPath");
    }

    logger("[ed_quickOpenFileDialog]: currentPath : %s", currentPath);

    strcat(currentPath, "\\" );

    vis_offset = (VIDEO_COLS / 4);
    dialog_offset = vis_offset / 4;

    dialogStartY = 2;
    dialogEndY = 18; 
    dialogHeight = dialogEndY - dialogStartY;

    dw_rectangle(textmemptr, vis_offset, dialogStartY, VIDEO_COLS - vis_offset, dialogEndY, COLOR_BLUE, COLOR_WHITE, ' ', COLOR_WHITE, COLOR_BLUE, false, DRAW_BORDER_SIMPLE, "OPEN FILE");

    stepIndex = strlen(currentPath);
    
    inp_waitForRelease();

    do{
        // Key selection
        if(inp_isKeyPressed(KEY_UP)){
            selectedEntry = 
                selectedEntry > 0
                ? selectedEntry - 1
                : 0; 
        }else if(inp_isKeyPressed(KEY_DOWN)){
            selectedEntry = 
                selectedEntry < entriesLen
                ? selectedEntry + 1
                : entriesLen; 
        }else if(inp_isKeyPressed(KEY_ENTER)){
            // If we press enter, we have to detect if the entry is either a directory or a file
            // 
            if(strcmp(selectedFileEntry->name, "..") == 0){         // .. path
                stepIndex = _goBackPath(currentPath);       
            }else if(selectedFileEntry && selectedFileEntry->isDirectory){
                //// Nothing happens
                strcat(currentPath, selectedFileEntry->name);
                strcat(currentPath, "\\");
                stepIndex = strlen(currentPath);
            }else{
                // We open the file
                // TODO: SANITIZE buffer by removing the chars until the last directory
                sprintf(selectedEntryFullPath, "%s%s", currentPath, selectedFileEntry->name);
                f_openFile(selectedEntryFullPath);
                _updateCursor();
                ed_renderEvent = true;
                return;
            }
        }else{

            // Freeing up list of files each time there is a change in the prompt.

            // Ok, for the file selection we have to clear mark a flag for the selected file
            // By
            if(currPathDirectory) fs_freeDirectory(currPathDirectory);

            currPathDirectory = fs_getDirectoryFileList(currentPath);

            if(!currPathDirectory){
                logger("[ed_quickOpenFileDialog]: Could not get currPathDirectory or FileEntry list for selection!");
                return;
            }

            // Draw list of files
            entriesLen = currPathDirectory->fileEntries->length;
            node = currPathDirectory->fileEntries->firstNode;
            entryIndex = 0;

            while(node != NULL){
                fileEntry = (FileEntry*)node->data;
                entryIndex++;
                // We write the filename under the prompt
                isSelected = (entryIndex == selectedEntry);

                // We mar the selected item or not
                if(isSelected){
                    selectedFileEntry = fileEntry;
                    dw_writeBuffer(textmemptr,"%s", vis_offset + 1, 3 + entryIndex + 1, VIDEO_COLS - vis_offset - 1, 3 + entryIndex + 1, COLOR_BLUE, COLOR_WHITE, fileEntry->name);
                }else{
                    dw_writeBuffer(textmemptr,"%s", vis_offset + 1, 3 + entryIndex + 1, VIDEO_COLS - vis_offset - 1, 3 + entryIndex + 1, COLOR_WHITE, COLOR_BLUE, fileEntry->name);
                }

                node = node->next;
            }

            // Clear list container until touches bottom
            clearMarkPoint = entryIndex;

            if(clearMarkPoint < dialogEndY - 5){
                while(clearMarkPoint < dialogEndY - 5){     
                    clearMarkPoint++;
                    dw_writeBuffer(textmemptr,"%s", vis_offset + 1, clearMarkPoint + 4, VIDEO_COLS - vis_offset - 1, clearMarkPoint + 4, COLOR_WHITE, COLOR_BLUE, "            ");
                }
            }
            
            // Draw rect in the middle, 1/4 will be the start and the end, so i it will always be in the center
            ed_async_scanf(vis_offset + 1, 3, (2 * vis_offset) - 1, currentPath, strlen(currentPath), &stepIndex);
            
        }
        
        inp_updateKeyboard();
    }while(!inp_isKeyPressed(KEY_ESC));

    _updateCursor();
    ed_renderEvent = true;
}




