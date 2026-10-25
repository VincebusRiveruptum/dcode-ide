#include "editor.h"

Clipboard ed_globalClipboard = {
    NULL,
    NULL,
    0,
    0
};

void ed_initClipboard(){
    MemoryArena *arena = NULL;

    arena = 
        mem_arena_create(
            "clipboard", 
            MEM_ARENA_4K /* Could be a setting*/
        );

    if(!arena){
        logger(
            "[ed_initClipboard]: Could not instantiate clipboard arena."
        );
        exit(1);
    }

    ed_globalClipboard.arena = arena;

    ed_resetClipboard();
}

/*
    This is the glue function between the presentation (main.c)
    and the module layer (ed_clipb.c)

    This functions listen the keyboard strokes for the clip-
    board operations.
*/

void ed_clipboard(){
    TextArea *textArea;

    if(
        !currentWorkspace ||
        !currentWorkspace->currentWindow ||
        !currentWorkspace->currentWindow->textArea
    ){
        logger("[ed_clipboard] Error: No file universe!.");
        exit(1);
    }

    textArea = currentWorkspace->currentWindow->textArea;

    if(ed_isSelectedText(currentWorkspace) == true){
        /* Copy */
        /* TODO: disable CTRL + C exitting */
        if(
            hal_inp_keysPressed(
                HAL_INP_TRIGGER_EDGE,
                2,
                HAL_KEY_LALT,
                HAL_KEY_C
            ) == true
        ){
            ed_copy(textArea);
        } else if(
            hal_inp_keysPressed(
                HAL_INP_TRIGGER_EDGE,
                2,
                HAL_KEY_LALT,
                HAL_KEY_X
            ) == true
        ){
            ed_cut(textArea);
        }
    } else if(
            hal_inp_keysPressed(
                HAL_INP_TRIGGER_EDGE,
                2,
                HAL_KEY_LALT,
                HAL_KEY_V
            ) == true
        ){
            ed_paste(textArea);
    }

    return;
}

void ed_freeClipboard(){
    mem_arena_free(ed_globalClipboard.arena);
}

/* Clear clipboard ======================================== */

void ed_resetClipboard(){
    mem_arena_reset(ed_globalClipboard.arena);
    
    ed_globalClipboard.lines = createList(ed_globalClipboard.arena);
    
    if(!ed_globalClipboard.lines){
        logger("[ed_copy] Error : Could not create line list.");
        exit(1);
    }

    ed_globalClipboard.selectedStartX = 0;
    ed_globalClipboard.selectedEndX = 0;
}

/* ========================================================*/
/*
    This checks if the clipboard memory arena have 
    enough headroom for the lines to copy, if not
    we have to resize it

    Returns the size needed if no headroom
    Returns 0 if there is headroom
*/

size_t ed_getClipboardHeadroom(TextArea *textArea){
    size_t size, len;
    unsigned short startLine, endLine;
    unsigned short start, end;

    if(!textArea){
        logger("[ed_getClipboardHeadroom] Error: textArea is NULL");
        exit(1);
    }

    startLine = textArea->selectedStartLine;
    endLine = textArea->selectedEndLine;

    len = abs(endLine - startLine) | 1;

    size = len * f_getLineSize();
    
    if(
         size >= 
         (textArea->arena->size - textArea->arena->offset) 
    ){
        /* Needs resizing*/
        return size;
    }

    /* Enough space*/
    return 0;
}

/* Cut ==================================================== */

void ed_cut(TextArea *textArea){
    ed_copy(textArea);
    ed_deleteSelection();
}

/* Copy =================================================== */
/* Copy selected lines into the clipboard's line list */
void ed_copy(TextArea *textArea){
    Node *start, *afterEnd, *rec;
    Line *currLine = NULL, *newLine = NULL;
    size_t headroom = 0;
    MemoryArena *newArena = NULL;

    if(!textArea){
        logger("[ed_copy] Error: textArea is NULL.");
        exit(1);
    }
    
    /*  Check if the clipboard's memory arena has
        enough headroom for the new lines to be copied.
    */
    if((headroom = ed_getClipboardHeadroom(textArea)) > 0){
        /* Re*/
        mem_arena_free(ed_globalClipboard.arena);
        newArena = mem_arena_create("clipboard", headroom + MEM_ARENA_2K);

        if(!newArena){
            logger("[ed_copy] Warning: Not enough memory for clipboard, cancelling operation.");
            return;
        }

        ed_globalClipboard.arena = newArena;

        logger("[ed_copy] Info: Clipboard arena resized succesfully.");
    }

    ed_resetClipboard();

    start = textArea->selectedStartNode;
    afterEnd = 
        textArea->selectedEndNode &&
        textArea->selectedEndNode->next
        ? textArea->selectedEndNode->next
        : NULL;

    rec = start;

    while(rec != afterEnd){
        /* Copy node */
        currLine = (Line*)rec->data;

        if(!currLine){
            logger("[ed_copy] Error: currLine must not be NULL.");
            exit(1);
        }

        newLine = ed_dupLine(currLine, ed_globalClipboard.arena);

        addGenericNode(&ed_globalClipboard.lines, newLine, ed_globalClipboard.arena);
    
        rec = rec->next;
    }

    /* Update clipboard metadata */
    ed_globalClipboard.selectedStartX = textArea->selectedStartX;
    ed_globalClipboard.selectedEndX = textArea->selectedEndX;

    return;
}

/* Paste ================================================== */
/* self explainatory ... */
void ed_paste(TextArea *textArea){
    Line *line, *currentLine = NULL;

    static char auxBuffer[MAX_FILE_LINE_LENGTH] = { '\0' };
    unsigned short cursorCol = 0;
    
    if(
        !currentWorkspace ||
        !currentWorkspace->currentWindow ||
        !currentWorkspace->currentWindow->textArea
    ){
        logger("[ed_paste]: No workspace, window or current file.");
        return;
    }

    cursorCol = textArea->cursorCol;
    
    if(
        !ed_globalClipboard.lines ||
        !ed_globalClipboard.lines->length > 0 ||
        !ed_globalClipboard.lines->firstNode ||
        !ed_globalClipboard.lines->firstNode->data
    ){
        logger("[ed_paste]: Info: No lines in clipboard to paste..");
        return;
    }

    logger("[ed_paste] Clipboard length : %d", ed_globalClipboard.lines->length);

    /* Single line paste, simpler */
    if(ed_globalClipboard.lines->length == 1){
        currentLine = textArea->currentLine;
        
        line = (Line *) ed_globalClipboard.lines->firstNode->data;

        memset(auxBuffer, '\0', MAX_FILE_LINE_LENGTH);
        memcpy(auxBuffer, currentLine->buffer, strlen(currentLine->buffer));
                
        memcpy(
            auxBuffer + cursorCol, 
            line->buffer + ed_globalClipboard.selectedStartX,  
            abs(ed_globalClipboard.selectedEndX - ed_globalClipboard.selectedStartX)
        );

        logger("[TEST0] %s", auxBuffer);

        memcpy(
            auxBuffer + strlen(auxBuffer), 
            currentLine->buffer + cursorCol,  
            strlen(currentLine->buffer + cursorCol)
        );

        logger("[TEST1] %s", auxBuffer);

        memset(currentLine->buffer, '\0', MAX_FILE_LINE_LENGTH);
        memcpy(currentLine->buffer, auxBuffer, strlen(auxBuffer));

        logger("[TEST2] %s", currentLine->buffer);

    }else{
        /* Multi line paste */

    }


}
