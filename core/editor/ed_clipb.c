#include "editor.h"

Clipboard ed_clipboard = {
    NULL,
    0,
    0
};

MemoryArena *clipboardArena;

void ed_initClipboard(){
    clipboardArena = 
        (MemoryArena*)mem_create_arena(
            "clipboard", 
            settings.CLIPBOARD_SIZE
        );

    if(!clipboardArena){
        logger(
            "[ed_initClipboard]: Could not instantiate"
            " clipboard arena."
        );

        return;
    }

    ed_clearClipboard();
}

// Clear clipboard ========================================

void ed_clearClipboard(){
    mem_arena_reset(clipboardArena);
    ed_clipboard.lines = createList(clipboardArena);
    
    if(!lines){
        logger("[ed_copy]: COuld not create li")
        return;
    }

    ed_clipboard.selectedStartX = 0;
    ed_clipboard.selectedEndX = 0;
}

void ed_freeClipboard(){
    freeArena(clipboardArena);
}

// Cut ====================================================

void ed_cut(){
    ed_copy();
    ed_deleteSelection();
}

// Copy ===================================================
// Copy selected lines into the clipboard's line list
void ed_copy(){
    Node *start, *afterEnd, *rec;
    File *currentFile = NULL;
    Line *currLine = NULL, *newLine = NULL;

    if(
        !currentWorkspace ||
        !currentWorkspace->currentWindow ||
        !currentWorkspace->currentWindow->currentFile
    ){
        logger("[ed_paste]: No workspace, window or current file.")
        return;
    }

    start = currentFile->selectedStartNode;
    afterEnd = 
        currentFile->selectedEndNode &&
        currentFile->selectedEndNode->next
        ? currentFile->selectedEndNode->next
        ? NULL;

    rec = start;

    while(rec != afterEnd){
        // Copy node
        currLine = (Line*)rec->data;

        if(!currLine){
            logger("[ed_copy]: currLine must not be NULL.")
            return;
        }

        newLine = ed_dupLine(currLine, clipboardArena);

        addGenericNode(&ed_clipboard.lines), newLine);
    }

    // Update clipboard metadata
    ed_clipboard.selectedStartX = currentFile->selectedStartX;
    ed_clipboard.selectedEndX = currentFile->selectedEndX;

    return;
}

// Paste ==================================================

void ed_paste(){
    File *currentFile = NULL;

    if(
        !currentWorkspace ||
        !currentWorkspace->currentWindow ||
        !currentWorkspace->currentWindow->currentFile
    ){
        logger("[ed_paste]: No workspace, window or current file.")
        return;
    }

    currentFile = 
        currentWorkspace->currentWindow->currentFile;

}
