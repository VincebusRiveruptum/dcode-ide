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
        (MemoryArena*)mem_arena_create(
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
        if(
            hal_inp_keysPressed(
                HAL_INP_TRIGGER_EDGE,
                2,
                HAL_KEY_LCTRL,
                HAL_KEY_C
            ) == true
        ){
            ed_copy(textArea);
        } else if(
            hal_inp_keysPressed(
                HAL_INP_TRIGGER_EDGE,
                2,
                HAL_KEY_LCTRL,
                HAL_KEY_X
            ) == true
        ){
            ed_cut(textArea);
        } else if(
            hal_inp_keysPressed(
                HAL_INP_TRIGGER_EDGE,
                2,
                HAL_KEY_LCTRL,
                HAL_KEY_V
            ) == true
        ){
            ed_paste(textArea);
        }
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

    if(!textArea){
        logger("[ed_copy] Error: textArea is NULL.");
        exit(1);
    }

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
    }

    /* Update clipboard metadata */
    ed_globalClipboard.selectedStartX = textArea->selectedStartX;
    ed_globalClipboard.selectedEndX = textArea->selectedEndX;

    return;
}

/* Paste ================================================== */

void ed_paste(TextArea *textArea){
    if(
        !currentWorkspace ||
        !currentWorkspace->currentWindow ||
        !currentWorkspace->currentWindow->textArea
    ){
        logger("[ed_paste]: No workspace, window or current file.");
        return;
    }
}
