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
    }  
    
    if(
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

    if(ed_globalClipboard.lines->length == 1){
        dw_requestRenderEvent(DW_RENDER_LINE);
    }else{
        /* TODO : Still WIP */
    }
}

/* Copy =================================================== */
/* Copy selected lines into the clipboard's line list */
void ed_copy(TextArea *textArea){
    Node *rec, *end;
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

    /*  
        Copy text block nodes must be ascendent, so we have 
        order Start and end nodes by the index.
    */
    
    /* Start ...*/
    if(
        mini(textArea->selectedStartLine, textArea->selectedEndLine) ==
        textArea->selectedStartLine    
    ){
        rec =  textArea->selectedStartNode;

    }else{
        rec =  textArea->selectedEndNode;
    }

    /* End ... */
    if(
        maxi(textArea->selectedStartLine, textArea->selectedEndLine) ==
        textArea->selectedStartLine
    ){
        end = textArea->selectedStartNode;
    }else{
        end = textArea->selectedEndNode;
    }

    end = 
        end->next
        ? end->next
        : NULL ;

    while(rec != end){
        /* Copy node */
        currLine = (Line*)rec->data;

        if(!currLine){
            continue;
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
    Line *innerStart = NULL, *innerEnd = NULL;
    Node *currentLineNode = NULL, *next = NULL;
    Node *innerStartNode = NULL, *innerEndNode = NULL;
    
    Line *start = NULL, *end = NULL;
    Node *startNode = NULL, *endNode = NULL;
    List *lines = NULL;

    unsigned short cursorCol = 0;
    unsigned short starti, endi, len;
    
    /* left side */
    static char auxbuff[MAX_FILE_LINE_LENGTH] = { '\0' };
    /* rigth side */
    static char auxbuff2[MAX_FILE_LINE_LENGTH] = { '\0' };
    
    if(
        !currentWorkspace ||
        !currentWorkspace->currentWindow ||
        !currentWorkspace->currentWindow->textArea ||
        !currentWorkspace->currentWindow->textArea->currentLineNode ||
        !currentWorkspace->currentWindow->textArea->currentLine
    ){
        logger("[ed_paste]: No workspace, window, current file or current line.");
        return;
    }
    
    if(on_selection_tool == true){
        ed_deleteSelection();
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

    starti = 
        (unsigned short)
        mini(
            (int)ed_globalClipboard.selectedStartX, 
            (int)ed_globalClipboard.selectedEndX
        );

    endi = 
        (unsigned short)
        maxi(
            (int)ed_globalClipboard.selectedStartX, 
            (int)ed_globalClipboard.selectedEndX
        );

    len = endi - starti;

    if(f_checkNewLineMemory(textArea, len)){
        f_resizeTextArea(textArea);
    }

    currentLineNode = textArea->currentLineNode;
    currentLine = textArea->currentLine;
    /* Single line paste, simpler */
    if(ed_globalClipboard.lines->length == 1){

        
        line = (Line *) ed_globalClipboard.lines->firstNode->data;

        memset(auxbuff, '\0', MAX_FILE_LINE_LENGTH);
        memcpy(auxbuff, currentLine->buffer, strlen(currentLine->buffer));
                
        memcpy(
            auxbuff + cursorCol, 
            line->buffer + starti,  
            len
        );
        
        *(auxbuff + cursorCol + len) = '\0';
        /* Heres the bug */
        memcpy(
            auxbuff + strlen(auxbuff), 
            currentLine->buffer + cursorCol,  
            strlen(currentLine->buffer + cursorCol)
        );

        memset(currentLine->buffer, '\0', MAX_FILE_LINE_LENGTH);
        memcpy(currentLine->buffer, auxbuff, strlen(auxbuff));
        
        currentLine->length = strlen(currentLine->buffer);
        textArea->cursorCol += len;

        dw_requestRenderEvent(DW_RENDER_LINE);
        
    }else if(ed_globalClipboard.lines->length >= 2){

        logger("[ed_paste] Debug: Cloning clipboard line list.");

        lines = 
            f_cloneLinesTempList(
                textArea->file->arena, 
                ed_globalClipboard.lines
            );
        
        if(
            !lines ||
            !lines->firstNode ||
            !lines->firstNode->data ||
            !lines->lastNode ||
            !lines->lastNode->data
        ){
            logger("[ed_paste] Error: Could not copy lines to be pasted.");
            exit(1);
        }


        /* 
            Joining inner lines to election starting and
            ending positions
        */

        startNode = lines->firstNode;
        start = (Line*)startNode->data;
        endNode = lines->lastNode;
        end = (Line *) endNode->data;
        
        innerStartNode = startNode->next;
        innerStart = (Line*)innerStartNode->data;
        
        innerEndNode = endNode->prev;
        innerEnd = (Line*)innerEndNode->data;
        
        if(currentLineNode->next){
            endNode->next = currentLineNode->next;
            currentLineNode->next->prev = endNode;
        }
        
        innerStartNode->prev = currentLineNode;
        currentLineNode->next = innerStartNode;
        
        /* ==== */
        /* We glue the outer sides  of the list*/
        memset(auxbuff, '\0', MAX_FILE_LINE_LENGTH);
        memset(auxbuff2, '\0', MAX_FILE_LINE_LENGTH);
        memcpy(auxbuff, currentLine->buffer, textArea->cursorCol);
        memcpy(
            auxbuff2, 
            currentLine->buffer + textArea->cursorCol, 
            strlen(currentLine->buffer + textArea->cursorCol)
        );
        

        /* start */
        memcpy(
            auxbuff + textArea->cursorCol, 
            start->buffer + starti, 
            strlen(start->buffer + starti)
        );

        *(auxbuff + strlen(auxbuff)) = '\0';
        /**(auxbuff2 + strlen(auxbuff2)) = '\0';
        */

        memset(currentLine->buffer, '\0', MAX_FILE_LINE_LENGTH);
        memcpy(currentLine->buffer, auxbuff, strlen(auxbuff));
        currentLine->length = strlen(auxbuff);
        
        /* end */
        memset(end->buffer + endi, 0, MAX_FILE_LINE_LENGTH - endi);
        memcpy(end->buffer + endi, auxbuff2, strlen(auxbuff2));
                
        end->length = strlen(end->buffer);
        
        *(end->buffer + strlen(end->buffer)) = '\0';

        /* recycle first temp line as in not being used */
        ed_softDeleteLine(textArea, lines->firstNode);

        /* Update textArea line list len*/
        textArea->file->lines->length += lines->length ;

        /* Update cursor*/
        dw_requestRenderEvent(DW_RENDER_ALL);

        /* We free the list instance BUT NOT THE NODES IN IT*/
        free(lines);
    }else{
        /* NOOP*/
    }


}
