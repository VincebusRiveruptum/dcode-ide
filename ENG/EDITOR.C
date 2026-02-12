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
    File *file;
    Node *nextLineNode, *currentLineNode, *prevLineNode;
    Line *nextLine, *currentLine, *prevLine;
    int nextLineStartIndex = 0, currentLineStartIndex = 0, prevLineStartIndex = 0;

    file = currentFileArena->file;

    // If the number of lines is less than the screen height
    // and the current Y cursor position is less than the nmber of lines
    if( y > 0 && !(
        currentCursorY + currentFileArena->file->scrollY < currentFileArena->file->lineCount - 1
        )
    )    return;
    
    // We check if we are in the current line
    currentLineNode = getNodeByIndex(&file->lines, file->scrollY + currentCursorY);
    currentLine = (Line *)currentLineNode->data;
    currentLineStartIndex = currentLine ? currentLine->length : 0;

    if(file->scrollY + currentCursorY + y >= 0){
        nextLineNode = currentLineNode->next;

        if(nextLineNode){
            nextLine = (Line *)nextLineNode->data;
            nextLineStartIndex = nextLine->length;
        }
    }
    
    if(file->scrollY + currentCursorY - 1 >= 0){
        prevLineNode = currentLineNode->prev;

        if(prevLineNode){
            prevLine = (Line *)prevLineNode->data;
            prevLineStartIndex = prevLine->length;
        }
    }
    

    if( currentCursorY + y < 0 ){
        if(file->scrollY > 0){
            file->scrollY--;
        } else {
            currentCursorY = 0;
        }
    } else if( currentCursorY + y >= VIDEO_ROWS ){
        if(file->lines && file->scrollY + VIDEO_ROWS < file->lines->length){
            file->scrollY++;
        } else {
            currentCursorY = VIDEO_ROWS - 1;
        }
    } else {
        currentCursorY += y;
    }

    // We can't move the cursor past the end of the line
    if(LINE_COUNTER_WIDTH - 1 < currentCursorX + x && currentCursorX + x <= currentLine->length + LINE_COUNTER_WIDTH){   
        if(currentCursorX + x < 0){
            currentCursorX = 0;

        } else if(currentCursorX + x >= VIDEO_COLS){
            currentCursorX = VIDEO_COLS - 1;
        } else {
            currentCursorX += x;
        }
        
        // Line jumping logic
        if( y < 0 && prevLineStartIndex < currentCursorX){
            currentCursorX = prevLineStartIndex + LINE_COUNTER_WIDTH;
        }

        if( y > 0 && nextLineStartIndex < currentCursorX){
            currentCursorX = nextLineStartIndex + LINE_COUNTER_WIDTH;
        }
    }
    
    /* Sync file cursor */
    file->cursorLine = file->scrollY + currentCursorY;
    file->cursorCol = currentCursorX; /* Simplified for now, doesn't account for scrollX yet */

    ed_putCursor(currentCursorX, currentCursorY);
    ed_renderEvent = true;
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

    x = currentCursorX - LINE_COUNTER_WIDTH;
    y = currentCursorY + file->scrollY;
    
    if (x < 0) x = 0;
    if (y < 0) y = 0;
        

    node = getNodeByIndex(&file->lines, y);
    
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

    currentCursorX++;

    currentFileArena->file->isModified = true;
    ed_renderEvent = true;
}

void ed_backspace(){
        // We type the char at 
    // X : currentCursorX + LINE_COUNTER_WIDTH + 1
    // Y : currentCursorY + file->scrollY + 1 
    Node *node, *prevNode;
    Line *line, *prevLine;

    int x = 0;
    int y = 0;

    File *file;
    file = currentFileArena->file;

    x = currentCursorX - LINE_COUNTER_WIDTH;
    y = currentCursorY + file->scrollY;

    if (x < 0) x = 0;
    if (y < 0) y = 0;

    
    // If we are at the first character of the line
    if(x == 0){

        node = getNodeByIndex(&file->lines, y);

        if(!node){
            logger("[ed_backspace] node is null", 0);
            return;
        }

        line = (Line *)node->data;
        // wE Delete the current line but also we need to copy the current line content to 
        // the last character of the previous line
        prevNode = node->prev;

        // We are at the first line of the file
        if(!prevNode) return;
        
        prevLine = (Line *)prevNode->data;

        if(!prevLine){
            logger("[ed_backspace] prevLine is null", 0);
            return;
        }

        if(prevLine->length > 0) prevLine->length--;
        currentCursorX = prevLine->length + LINE_COUNTER_WIDTH;

        memcpy(prevLine->buffer + prevLine->length, line->buffer, line->length);
        prevLine->length += line->length;

        // NOT FOR NOW
        //deleteNode(&file->lines, y);

        if(node->next){
            prevNode->next = node->next;
            node->next->prev = prevNode;
        }
        
        currentFileArena->file->lineCount--;
        currentCursorY--;
    }else{
        node = getNodeByIndex(&file->lines, y);
        line = (Line *)node->data;
        
        memcpy(line->buffer + x - 1, line->buffer + x, line->length - x);    
        line->length--;
        currentCursorX--;
    }

    currentFileArena->file->isModified = true;      
    ed_renderEvent = true;
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

    x = currentCursorX - LINE_COUNTER_WIDTH;
    y = currentCursorY + file->scrollY;
    
    if (x < 0) x = 0;
    if (y < 0) y = 0;

    node = getNodeByIndex(&file->lines, y);
    line = (Line *)node->data;
    
    memcpy(line->buffer + x, line->buffer + x + 1, line->length - x);    
    line->length--;

    currentFileArena->file->isModified = true;
    ed_renderEvent = true;
}

void ed_newLine(){
    unsigned int newLinePos = 0, x = 0;
    Node *node;
    Line *line, *newLine;
    MemoryArena *arena;
    
    File *file;
    file = currentFileArena->file;

    arena = currentFileArena->arena;
    
    node = getNodeByIndex(&file->lines, file->cursorLine);
    if (!node) return; /* Should not happen */
    line = (Line *)node->data;

    x = currentCursorX - LINE_COUNTER_WIDTH;
    /* Insert the new line after the current line */
    newLinePos = file->cursorLine + 1;

    logger("[ed_newLine]:newLinePos: %d", newLinePos);

    newLine = (Line*)mem_arena_alloc(arena, NULL, sizeof(Line));
    newLine->length = 0;
    newLine->buffer = (char*)mem_arena_alloc(arena, NULL, sizeof(char) * MAX_FILE_LINE_LENGTH);
    memset(newLine->buffer, '\0', MAX_FILE_LINE_LENGTH);

    memcpy(newLine->buffer, line->buffer + x, line->length - x);
    newLine->length = line->length - x;

    memset(line->buffer + x, '\0', MAX_FILE_LINE_LENGTH - x );
    line->buffer[x] = '\n';
    line->length = x + 1;

    /* No need to set '\n' as it's a line buffer */
    insertGenericNode(&file->lines, newLine, arena, newLinePos);

    /* Move cursor down and scroll if necessary */
    if (currentCursorY + 1 >= VIDEO_ROWS) {
        file->scrollY++;
    } else {
        currentCursorY++;
    }

    currentCursorX = LINE_COUNTER_WIDTH;
    file->cursorLine = file->scrollY + currentCursorY;
    file->cursorCol = currentCursorX;
    
    currentFileArena->file->isModified = true;
    currentFileArena->file->lineCount++;
    ed_renderEvent = true;
}

