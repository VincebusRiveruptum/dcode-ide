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

void ed_initConfig(int argc, char *argv[]){
    //f_defaultExtension

    // We will hardcode the default extension until i implement .ENV/CFG LOADING
    strcpy(f_defaultExtension, ".c");

    ed_statusbarBgColor = COLOR_LIGHT_GRAY;
    ed_statusbarFgColor = COLOR_RED;

    logger("[ed_initConfig]: %d %s", argc, argv[1]);
    ed_handleArguments(argc, argv);

    ed_resetCursor();
}

void ed_handleArguments(int argc, char *argv[]){
    // File opening
    logger("[ed_handleArguments]: %d %s", argc, argv[1]);
    
    if(argc > 1 || (argv != NULL && argv[1] != NULL)){
        if(!f_openFile(argv[1])){
            logger("[ed_handleArguments]: File %s not found. Falling back to new file.", argv[1]);
            f_newFile();
        }
    }else{
        f_newFile();
    }

    ed_renderEvent = true;
}

int _get_tab_counts_until_cursorCol(){
    int i = 0, tabCount = 0;

    while(i < currentFileArena->file->cursorCol){
        if(currentFileArena->file->currentLine->buffer[i] == CHAR_TAB) tabCount++;
        i++; 
    }
    
    return tabCount;
}
// Calculate current line number of tabs
int _calculateTabCount(){
    unsigned int i = 0, tabCount = 0;
    char c;

    while(c != '\0'){
        c = currentFileArena->file->currentLine->buffer[i];

        if(c == CHAR_TAB) tabCount++;

        i++;
    }
    return tabCount;
}

int _calculateTabStart(){
    int i = 0, tabCount = 0;
    char c, cnext;

    while(c != '\0'){
        c = currentFileArena->file->currentLine->buffer[i];

        if (c == CHAR_TAB){
            tabCount++;

            cnext = currentFileArena->file->currentLine->buffer[i + 1];

            if(cnext == NULL && cnext != CHAR_TAB){
                return tabCount;
            }
        }
        i++;

    }

    return tabCount;
}

void _updateCurrentCursorY(){
     // If the cursor is closer to the bottom
    if(currentCursorY <=0 || currentFileArena->file->cursorLine < 0) currentCursorY = 0;
    
    if( currentFileArena->file->cursorLine - currentFileArena->file->scrollY >= 0){
        currentCursorY = currentFileArena->file->cursorLine - currentFileArena->file->scrollY;
    }

    if(currentCursorY >= VIDEO_ROWS ) currentCursorY = VIDEO_ROWS;
}
void _updateCurrentCursorX(){
    int tabcounts = 0;
    // For debugging
    currentFileArena->file->prevChar = 
    currentFileArena->file->currentLine->buffer[currentFileArena->file->cursorCol - 1]
    ? currentFileArena->file->currentLine->buffer[currentFileArena->file->cursorCol - 1]
    : ' ';
    
    currentFileArena->file->currentChar =
    currentFileArena->file->currentLine->buffer[currentFileArena->file->cursorCol]
    ? currentFileArena->file->currentLine->buffer[currentFileArena->file->cursorCol]
    : ' ';
    
    currentFileArena->file->nextChar = 
    currentFileArena->file->currentLine->buffer[currentFileArena->file->cursorCol + 1]
    ? currentFileArena->file->currentLine->buffer[currentFileArena->file->cursorCol + 1]
    : ' ';
    
    
     if(currentFileArena->file->cursorCol <= 0 ){
         currentCursorX = 0;
        
     }else if(currentFileArena->file->cursorCol > 0){
         currentCursorX = currentFileArena->file->cursorCol;        
    
     }else if(currentFileArena->file->cursorCol >= VIDEO_COLS) {
         currentCursorX = VIDEO_COLS - 1;
     }    
    
    tabcounts = _get_tab_counts_until_cursorCol();
    currentCursorX = currentCursorX + tabcounts * 3 + LINE_COUNTER_WIDTH;
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
    currentCursorX = LINE_COUNTER_WIDTH;
    currentCursorY = 0;
}

void ed_updateCursor(){
    char c;
    
    // Force a cursor in 43/50 line moded
    if(VIDEO_ROWS == 43 || VIDEO_ROWS == 50){
        dw_writeColor(textmemptr, currentCursorX, currentCursorY, COLOR_BLACK, COLOR_LIGHT_GRAY);
    }
    ed_putCursor(currentCursorX, currentCursorY);
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
    
    }
    // END VERTICAL SCROLLING =====================================================

    // HORIZ, SCROLLING ===========================================================
    // We can't move the cursor past the end of the line
    if(x){
        if(currentFileArena->file->cursorCol + x <= 0){
            currentFileArena->file->cursorCol = 0;
        } else if(
            currentFileArena->file->cursorCol + x > 0 && 
            currentFileArena->file->cursorCol + x <= currentFileArena->file->currentLine->length
        ){  
            if (x < 0) c = currentFileArena->file->currentLine->buffer[currentFileArena->file->cursorCol + x];
            if (x > 0) c = currentFileArena->file->currentLine->buffer[currentFileArena->file->cursorCol + x - 1];

            currentFileArena->file->cursorCol +=x;

        }else if(currentFileArena->file->cursorCol + x >= VIDEO_COLS && currentFileArena->file->currentLine->length >= VIDEO_COLS){
            // HERE WE SHOULD SCROLL RIGHT 
            currentFileArena->file->cursorCol = currentFileArena->file->currentLine->length - 1;
        }
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

    node = getNodeByIndex(&file->lines, y);
    line = (Line *)node->data;
    
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
    
    arena = currentFileArena->arena;

    x = currentFileArena->file->cursorCol;

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
    copyLen = 
        (x <= currentFileArena->file->currentLine->length)
        ?
            currentFileArena->file->currentLine->length - x
        :
            0
        ;
            
    memcpy(newLine->buffer, currentFileArena->file->currentLine->buffer + x, copyLen);
    newLine->length = copyLen;

    // We clear the current line position onwards
    memset(currentFileArena->file->currentLine->buffer + x, '\0', MAX_FILE_LINE_LENGTH - x);
    
    // REMOVED 'buffer[x-1] = \n' assignment logic here as it caused char eating
    
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
    currentFileArena->file->cursorCol = currentCursorX - LINE_COUNTER_WIDTH ;

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

    currentFileArena->file->cursorCol = 0;
    currentFileArena->file->cursorLine++;

    // activity flags
    currentFileArena->file->isModified = true;

    _updateCursor();
}

// PROMPT ELEMENT
char *ed_scanf(unsigned char x, unsigned char y, unsigned char maxChars ){
    int i = 0, j = 0, lenbuff = 0;
    char c = 0;
    
    static char buffer[MAX_FILE_LINE_LENGTH];

    memset(buffer, '\0', MAX_FILE_LINE_LENGTH);

    ed_putCursor(x,y);    

    while(c != CHAR_ENTER){
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
            } else if (c != CHAR_ENTER && i < maxChars && i < MAX_FILE_LINE_LENGTH - 1){
                buffer[i] = c;
                dw_charXY(textmemptr,c,x + i,y);
                i++;
                ed_putCursor(x + i, y);
            }
        }
    }

    return buffer;
}


void ed_putCursorEnd(){
    unsigned int tabCount = 0;

    tabCount = _calculateTabCount();
    currentFileArena->file->cursorCol = currentFileArena->file->currentLine->length;

    _updateCursor();
}

void ed_putCursorStart(){   
    currentFileArena->file->cursorCol = 0;

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
    

    ed_putCursor(currentCursorX, currentCursorY);
    ed_renderEvent = true;
}

void ed_putCursorLastLine(){
    Line *tmpLine;
    int lineposX;

    currentFileArena->file->currentLineNode = currentFileArena->file->lines->lastNode;

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
    // then we need to reset some flags so we can redraw the screen properly
    /* Sync file cursor */
    currentFileArena->file->scrollY = currentFileArena->file->lines->length - VIDEO_ROWS;
    currentFileArena->file->cursorLine = currentFileArena->file->lines->length;
    currentFileArena->file->cursorCol = currentCursorX - LINE_COUNTER_WIDTH;
    
    currentCursorY = currentFileArena->file->lines->length - currentFileArena->file->scrollY - 1;
        
    ed_putCursor(currentCursorX, currentCursorY);
    ed_renderEvent = true;
}

