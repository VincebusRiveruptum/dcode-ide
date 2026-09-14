#include "files.h"

Workspace *currentWorkspace = NULL;
EditorWindow *currentWindow = NULL;


//========================================================================
// TextAera managing
//========================================================================

/* CLOSE FILE ==================================================================*/
void f_closeFile(File *file){
    char arenaName[64];
    sprintf(arenaName, "%s", file->arena->name);

    mem_arena_free(file->arena);
    logger("[f_closeFile]: File %s closed successfully", arenaName);    
}

void f_closeTextArea(TextArea *textArea){
	logger(
		"[f_closeFile]: Checking %s refs in usage.", 
		textArea->file->name
	);    
	
	if(!f_checkFileRefs(textArea->file)){
		logger("[f_closeTextArea]: No refs, closing file.");
		f_closeFile(textArea->file);
	}

    mem_arena_free(textArea->arena);

    logger("[f_closeFile]: TextArea arena closed successfully");    
}

void f_closeCurrentTextArea(){
    char oldFileName[255];
	TextArea *nextTextArea = NULL;
	EditorWindow *toDelete = NULL, *wnd = NULL, *neighbor = NULL;
	Node *rec = NULL;

    memset(oldFileName, '\0', 255);

    if(
        !currentWindow || 
        !currentWindow->textArea
    ) 
		return;

    strcpy(oldFileName, currentWindow->textArea->file->name);

    f_deleteTextAreaFromWindow(
		currentWindow, 
		currentWindow->textArea
	);
	
	// Updates current textArea with the first 
	// textArea avalable on the list
	if (currentWindow->textAreaList->length > 0) {
		nextTextArea = 
            (TextArea *)(currentWindow->textAreaList->firstNode->data);
		currentWindow->textArea = nextTextArea;
	} else {
		// If no text areas, we close the entire window.
		currentWindow->textArea = NULL;
		
		if (currentWorkspace->windowList->length > 1) {
			toDelete = currentWindow;
			rec = currentWorkspace->windowList->firstNode;
			neighbor = NULL;
			
			while (rec != NULL) {
				wnd = (EditorWindow *)rec->data;
				if (wnd != toDelete) {
					neighbor = wnd;
					if (wnd->x + wnd->width + 1 == toDelete->x) {
						break;
					}
				}
				rec = rec->next;
			}
			
			if (neighbor != NULL) {
				if (toDelete->x > neighbor->x) {
					neighbor->width += toDelete->width + 1;
				} else {
					neighbor->x = toDelete->x;
					neighbor->width += toDelete->width + 1;
				}
			}
			
			f_cycleActiveWindow();
			f_deleteWindowFromWorkspace(currentWorkspace, toDelete);
			free(toDelete);
		}
	}
    
    ed_statusBarMessage("%s closed successfully.", oldFileName);
    logger("[f_closdeCurrentFile]: %s closed successfully.", oldFileName);

    ed_updateCursor();
	
	dw_requestRenderEvent(DW_RENDER_ALL);
    return;
}

//========================================================================
// EditorWindow managing
// TODO : This should be its own module.
//========================================================================

EditorWindow *f_createWindow(){
	EditorWindow *newWindow = NULL;

	newWindow = (EditorWindow*)malloc(sizeof(EditorWindow));

	if(!newWindow){
		logger("[f_createWindow]: Could not alloc for a window!.");
		return NULL;
	}

	newWindow->textAreaList = createList(NULL);
	newWindow->textArea = NULL;

	// By default will be minimized for now
	newWindow->status = WndStatus_MINIMIZED;
	newWindow->x = 0;
	newWindow->y = 0;

	newWindow->width = 0;
	newWindow->height = 0;

	newWindow->currentFileIndex = 0;
	newWindow->index = 0;
	newWindow->active = false;

	return newWindow;
}

// Inits workspace with all atributtes zeroed.
Workspace *f_createWorkspace(){
	char *fullPath = NULL;
	char pathBuf[512];
	Workspace *newWorkspace = NULL;

	if(currentWorkspace){
		logger("[f_createWorkspace]: workspace already initialized.");
		return NULL;
	}
		
	if (!hal_fs_getAbsoluteCurrentPath(pathBuf, sizeof(pathBuf))) {
		logger("[f_createWorkspace]: invalid fullpath.");
		return NULL;
	}

	fullPath = (char *)malloc(strlen(pathBuf) + 1);

	if (!fullPath) {
		logger("[f_createWorkspace]: malloc failed for fullPath.");
		return NULL;
	}
	strcpy(fullPath, pathBuf);

		
	newWorkspace = (Workspace*)malloc(sizeof(Workspace));
	memset(newWorkspace, 0, sizeof(Workspace));
	
	if(!newWorkspace){
		logger("[f_initWorkspac]: could not allow memory for workspace.");
		return NULL;
	}

	newWorkspace->fullPath = fullPath;
	newWorkspace->windowList = createList(NULL);	
	newWorkspace->currentWindow = NULL;

	if(!newWorkspace->windowList){
		logger("[f_initWorkspac]: could not allow memory for workspace window list.");
		return NULL;
	}

	return newWorkspace;
}

//========================================================================
//	Workspace - EditorWindow - File DECONSTRUCTOR
//========================================================================

// Free an entire textArea list
void f_freeTextAreaList(List *textAreaList){
	Node *rec=NULL;
	Node *tmp=NULL;
	TextArea *textArea=NULL;

	if(!textAreaList)
		return;

	rec = textAreaList->firstNode;

	while(rec){
		tmp = rec->next;
		textArea = (TextArea*)rec->data;
		if(textArea) {
			// TODO: Check if thre are no
			// shared file instances anywhere
			// if so, free it entirely
			if(!f_checkFileRefs(textArea->file))
				mem_arena_free(textArea->file->arena);
			
			mem_arena_free(textArea->arena);
		}
		free(rec);
		rec = tmp;
	}

	free(textAreaList);
}

// Free window list, fileList on each window and each file arena.
void f_freeWindowList(List *windowList){
	Node *rec=NULL, *tmp;
	EditorWindow *wnd = NULL;
	if(!windowList) {
		logger("[f_freeWorkspace]: workspace already free");
		return;
	}

	// Freeing windowList
	rec = windowList->firstNode;

	while(rec){
		tmp = rec->next;
		wnd = (EditorWindow *)rec->data;
		if(wnd) {
			f_freeTextAreaList(wnd->textAreaList);
			free(wnd);
		}
		free(rec);
		rec = tmp;
	}

	free(windowList);
}

// Free workspace, windows, fileList on each window and each file arena.
void f_freeWorkspace(){
	if(!currentWorkspace) {
		logger("[f_freeWorkspace]: workspace already free");
		return;
	}

	// Freeing windowList
	f_freeWindowList(currentWorkspace->windowList);

	free(currentWorkspace->fullPath);
	free(currentWorkspace);
	currentWorkspace = NULL;
	currentWindow = NULL;
}

// =======================================================================

TextArea *f_addTextAreaToWindow(EditorWindow *window, TextArea *textArea){
	if (!textArea || !window->textAreaList || !window){
		logger("[f_addTextAreaToWindow]: invalid data.");
		return NULL;
	}

	addGenericNode(&(window->textAreaList), (void*)textArea, NULL);

	return textArea;
}

EditorWindow *f_addWindowToWorkspace(Workspace *workspace, EditorWindow *window){
	if (!workspace || !workspace->windowList || !window ){
		logger("[f_addWindowToWorkspace]: invalid data.");
		return NULL;
	}

	addGenericNode(&(workspace->windowList), (void*)window, NULL);

	return window;
}

// ======

void f_deleteTextAreaFromWindow(
	EditorWindow *window, 
	TextArea *textArea
){
	if(
		!window || 
		!window->textAreaList ||
		!textArea
	){
		logger("[f_deleteTextAreaFromWindow]: invalid data.");
		return;
	}

	deleteNodeByPtr(&(window->textAreaList), (void*)textArea);

	f_closeTextArea(textArea);
	//f_closeFile(file);    
	
	return;
}

void f_deleteWindowFromWorkspace(Workspace *workspace, EditorWindow *window){
	Node *rec = NULL;
	File *file = NULL;

	if(!workspace || !workspace->windowList || !window){
		logger("[f_deleteWindowFromWorkspace]: invalid data.");
		return;
	}

	deleteNodeByPtr(&(workspace->windowList), (void*)window);

	// Close all files
	rec = window->textAreaList->firstNode;

	if(rec){
		while(rec){
			file = (File*)rec->data;

			if(file)
				f_closeFile(file);
			
			rec = rec->next;
		}
	}

	return;
}

void f_deleteWorkspace(Workspace *workspace){
	Node *rec = NULL;
	EditorWindow *window = NULL;

	if(!workspace ){
		logger("[f_deleteWorkspace]: Invalid data");
		return;
	}

	if(workspace->windowList){
		rec = workspace->windowList->firstNode;

		while(rec){
			window = (EditorWindow*)rec->data;

			if(window)
				f_deleteWindowFromWorkspace(workspace, window);

			rec = rec->next;
		}
	}

	free(workspace);
}

// ===============================

void f_splitWindow(){
	EditorWindow *newWnd = NULL;
	unsigned int half = 0;

	if(
		!currentWorkspace ||
		!currentWindow || 
		!currentWindow->textArea ||
		!currentWindow->textArea->file
	) return;

	newWnd = f_createWindow();
	if(!newWnd) return;

	newWnd->textArea = currentWindow->textArea;
	f_addTextAreaToWindow(newWnd, currentWindow->textArea);

	half = currentWindow->width / 2;
	newWnd->x = currentWindow->x + half + 1;
	newWnd->width = currentWindow->x + currentWindow->width - newWnd->x;
	currentWindow->width = half;
	newWnd->y = currentWindow->y;
	newWnd->height = currentWindow->height;

	f_addWindowToWorkspace(currentWorkspace, newWnd);

	currentWindow->active = false;
	newWnd->active = true;
	currentWorkspace->currentWindow = newWnd;
	currentWindow = newWnd;

  	dw_requestRenderEvent(DW_RENDER_ALL);
}

void f_cycleActiveWindow(){
	Node *currNode;
	Node *nextNode;

	if (!currentWorkspace || !currentWorkspace->windowList || currentWorkspace->windowList->length <= 1) return;

	currNode = currentWorkspace->windowList->firstNode;
	while (currNode != NULL) {
		if (currNode->data == currentWorkspace->currentWindow) {
			break;
		}
		currNode = currNode->next;
	}

	if (currNode != NULL) {
		((EditorWindow *)currNode->data)->active = false;

		nextNode = currNode->next;
		if (nextNode == NULL) {
			nextNode = currentWorkspace->windowList->firstNode;
		}

		((EditorWindow *)nextNode->data)->active = true;
		currentWorkspace->currentWindow = (EditorWindow *)nextNode->data;
		currentWindow = currentWorkspace->currentWindow;

	  	dw_requestRenderEvent(DW_RENDER_ALL);
	}
}
