#include "editor.h"

Clipboard ed_clipboard = {
    NULL,
    NULL,
    0,
    0
};

void ed_initClipboard(){
    MemoryArena *arena = NULL;

    arena = 
        (MemoryArena*)mem_create_arena(
            "clipboard", 
            MEM_ARENA_2K /* Could be a setting*/
        );

    if(!arena){
        logger(
            "[ed_initClipboard]: Could not instantiate"
            " clipboard arena."
        );
        exit(1);
    }

    ed_clipboard.arena = arena;

    ed_clearClipboard();
}
/*
    This is the glue function between the presentation (main.c)
    and the module layer (ed_clip.c)

    This functions listen the keyboard strokes for the clip-
    board operations.
*/

void ed_prepareClipboard(){
    TextArea *textArea;

    if(
        !currentWorkspace ||
        !currentWorkspace->currentWindow ||
        !currentWorkspace->currentWindow->textArea
    ){
        logger("[ed_prepareClipboard] Error: No file universe!.");
        exit(1);
    }

    textArea = currentWorkspace->currentWindow->textArea;

    if(ed_isSelectedText(currentWorkspace) == true){
        /* Copy */
        if(hal_inp_keysPressed(
            HAL_INP_TRIGGER_EDGE,
            2,
            HAL_KEY_LCTRL,
            HAL_KEY_C
        )){
            ed_copy(textArea);
        }else if(
            hal_inp_keysPressed(
            HAL_INP_TRIGGER_EDGE,
            2,
            HAL_KEY_LCTRL,
            HAL_KEY_X
        )){
            ed_cut(textArea);
        }
        }else if(
            hal_inp_keysPressed(
            HAL_INP_TRIGGER_EDGE,
            2,
            HAL_KEY_LCTRL,
            HAL_KEY_V
        )){
            ed_paste(textArea);
        }
    }
}

void ed_freeClipboard(){
    mem_arena_free(ed_clipboard.arena);
}

/* Clear clipboard ======================================== */

void ed_clearClipboard(){
    mem_arena_reset(ed_clipboard.arena);
    
    ed_clipboard.lines = createList(ed_clipboard.arena);
    
    if(!ed_clipboard.lines){
        logger("[ed_copy] Error : Could not create line list.");
        exit(1);
    }

    ed_clipboard.selectedStartX = 0;
    ed_clipboard.selectedEndX = 0;
}

void ed_freeClipboard(){
    freeArena(ed_clipboard.arena);
}

/* Cut ==================================================== */

void ed_cut(){
    ed_copy();
    ed_deleteSelection();
}

/* Copy =================================================== */
/* Copy selected lines into the clipboard's line list */
void ed_copy(TextArea *textArea){
    Node *start, *afterEnd, *rec;
    Line *currLine = NULL, *newLine = NULL;

    if(!textArea){
        logger("[ed_copy] Error: textArea is NULL.");
        exit(1);
    }

    start = textArea->selectedStartNode;
    afterEnd = 
        textArea->selectedEndNode &&
        textArea->selectedEndNode->next
        ? textArea->selectedEndNode->next
        ? NULL;

    rec = start;

    while(rec != afterEnd){
        /* Copy node */
        currLine = (Line*)rec->data;

        if(!currLine){
            logger("[ed_copy] Error: currLine must not be NULL.");
            exit(1);
        }

        newLine = ed_dupLine(currLine, ed_clipboard.arena);

        addGenericNode(&ed_clipboard.lines, newLine, ed_clipboard.arena);
    }

    /* Update clipboard metadata */
    ed_clipboard.selectedStartX = textArea->selectedStartX;
    ed_clipboard.selectedEndX = textArea->selectedEndX;

    return;
}

/* Paste ================================================== */

void ed_paste(){
    TextArea *textArea = NULL;

    if(
        !currentWorkspace ||
        !currentWorkspace->currentWindow ||
        !currentWorkspace->currentWindow->textArea
    ){
        logger("[ed_paste]: No workspace, window or current file.");
        return;
    }

    textArea = 
        currentWorkspace->currentWindow->textArea;

}
