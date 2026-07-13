#include "editor.h"

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
