#include "files.h"

// ============================================================================

void f_prepareFileNavDialog(){
	if(hal_inp_keysPressed(HAL_INP_TRIGGER_EDGE, 2, HAL_KEY_LALT, HAL_KEY_LSHIFT)){
		f_onFileNavigation = true;
		
		dw_requestRenderEvent(DW_RENDER_ALL);
	}

	if (f_onFileNavigation) {
		if (!hal_inp_isKeyDown(HAL_KEY_LALT)) {
			f_onFileNavigation = false;
			dw_requestRenderEvent(DW_RENDER_ALL);
			return;
		}

		if (hal_inp_keysPressed(HAL_INP_TRIGGER_EDGE, 2, HAL_KEY_LALT, HAL_KEY_LSHIFT)) {
			Node *currNode = currentWindow->fileList->firstNode;
			Node *selectedNode = NULL;
			while (currNode != NULL) {
				if (currNode->data == currentWindow->currentFile) {
					selectedNode = currNode;
					break;
				}
				currNode = currNode->next;
			}
			if (selectedNode != NULL) {
				if (selectedNode->next != NULL) {
					currentWindow->currentFile = (File *)selectedNode->next->data;
				} else {
					currentWindow->currentFile = (File *)currentWindow->fileList->firstNode->data;
				}
				dw_requestRenderEvent(DW_RENDER_ALL);
			}
		}
	}
}

void f_drawFileNavDialog(){
    bool selected = false;
    File *fileptr;
	Node *currFileNode = NULL;
	int i = 0;

    if (!currentWindow || !currentWindow->fileList) {
		logger("[f_drawFileNavDialog]: No window opened or no files opened in current window");
        return;
    }

	dw_rectangle(
		textmemptr, 
		4, 4, 34, 16, 
		COLOR_RED, COLOR_WHITE, ' ', COLOR_WHITE, COLOR_RED, 
		false, DRAW_BORDER_SIMPLE, NULL
	);

	currFileNode = currentWindow->fileList->firstNode;
	while(currFileNode != NULL){
		fileptr = (File*)currFileNode->data;
		
		if (fileptr != NULL) {
			selected = (currentWindow->currentFile == fileptr);

			dw_writeBuffer(
				textmemptr, 
				"%s %s",
				5, 5 + i, 33, 5 + i, 
				COLOR_WHITE, COLOR_RED,
				(selected ? "*" : " "),
				fileptr->name
			);
			i++;
		}

		currFileNode = currFileNode->next;
	}
}