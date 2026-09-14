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
			Node *currNode = currentWindow->textAreaList->firstNode;
			Node *selectedNode = NULL;
			while (currNode != NULL) {
				if (currNode->data == currentWindow->textArea) {
					selectedNode = currNode;
					break;
				}
				currNode = currNode->next;
			}
			if (selectedNode != NULL) {
				if (selectedNode->next != NULL) {
					currentWindow->textArea = (TextArea *)selectedNode->next->data;
				} else {
					currentWindow->textArea = (TextArea *)currentWindow->textAreaList->firstNode->data;
				}
				dw_requestRenderEvent(DW_RENDER_ALL);
			}
		}
	}
}

void f_drawFileNavDialog(){
    bool selected = false;
    TextArea *textArea;
    File *file;
	Node *textAreaNode = NULL;
	int i = 0;

    if (
		!currentWindow ||
		!currentWindow->textAreaList
	) {
		logger("[f_drawFileNavDialog]: No window opened or no files opened in current window");
        return;
    }

	dw_rectangle(
		textmemptr, 
		4, 4, 34, 16, 
		COLOR_RED, COLOR_WHITE, ' ', COLOR_WHITE, COLOR_RED, 
		false, DRAW_BORDER_SIMPLE, NULL
	);

	textAreaNode = currentWindow->textAreaList->firstNode;

	while(textAreaNode != NULL){
		//file = (File*)textAreaNode->data;
		textArea = (TextArea*)textAreaNode->data;
		
		if (file != NULL) {
			selected = (currentWindow->textArea->file == file);

			dw_writeBuffer(
				textmemptr, 
				"%s %s",
				5, 5 + i, 33, 5 + i, 
				COLOR_WHITE, COLOR_RED,
				(selected ? ">" : " "),
				file->name
			);
			i++;
		}

		textAreaNode = textAreaNode->next;
	}
}