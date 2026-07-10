#include "files.h"

Workspace *currentWorkspace = NULL;
Window *currentWindow = NULL;

//========================================================================
// Window managing
// TODO : This should be its own module.
//========================================================================

Window *f_createWindow(){
	Window *newWindow = NULL;

	newWindow = (Window*)malloc(sizeof(Window));

	if(!newWindow){
		logger("[f_createWindow]: Could not alloc for a window!.");
		return NULL;
	}

	newWindow->fileList = createList(NULL);
	newWindow->currentFile = NULL;

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
	Workspace *newWorkspace = NULL;

	if(currentWorkspace){
		logger("[f_createWorkspace]: workspace already initialized.");
		return NULL;
	}
		
    {
        char pathBuf[512];
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
    }
		
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
//	Workspace - Window - File DECONSTRUCTOR
//========================================================================

// Free an entire fileList
void f_freeFileList(List *fileList){
	Node *rec=NULL;
	Node *tmp=NULL;
	File *file=NULL;

	if(!fileList)
		return;

	rec = fileList->firstNode;

	while(rec){
		tmp = rec->next;
		file = (File*)rec->data;
		if(file) {
			mem_arena_free(file->arena);
		}
		free(rec);
		rec = tmp;
	}

	free(fileList);
}

// Free window list, fileList on each window and each file arena.
void f_freeWindowList(List *windowList){
	Node *rec=NULL, *tmp;
	Window *wnd = NULL;
	if(!windowList) {
		logger("[f_freeWorkspace]: workspace already free");
		return;
	}

	// Freeing windowList
	rec = windowList->firstNode;

	while(rec){
		tmp = rec->next;
		wnd = (Window *)rec->data;
		if(wnd) {
			f_freeFileList(wnd->fileList);
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

File *f_addFileToWindow(Window *window, File *file){
	if (!file || !window->fileList || !window){
		logger("[f_addFileToWindow]: invalid data.");
		return NULL;
	}

	addGenericNode(&(window->fileList), (void*)file, NULL);

	return file;
}

Window *f_addWindowToWorkspace(Workspace *workspace, Window *window){
	if (!workspace || !workspace->windowList || !window ){
		logger("[f_addWindowToWorkspace]: invalid data.");
		return NULL;
	}

	addGenericNode(&(workspace->windowList), (void*)window, NULL);

	return window;
}

void f_deleteFileFromWindow(Window *window, File *file){
	if(!window || !window->fileList || !file){
		logger("[f_deleteFileFromWindow]: invalid data.");
		return;
	}

	deleteNodeByPtr(&(window->fileList), (void*)file, file->arena);

	return;
}

void f_deleteWindowFromWorkspace(Workspace *workspace, Window *window){
	if(!workspace || !workspace->windowList || !window){
		logger("[f_deleteWindowFromWorkspace]: invalid data.");
		return;
	}

	deleteNodeByPtr(&(workspace->windowList), (void*)window, NULL);

	return;
}

void f_splitWindow(){
	Window *newWnd = NULL;
	unsigned int half = 0;

	if(!currentWorkspace || !currentWindow || !currentWindow->currentFile) return;

	newWnd = f_createWindow();
	if(!newWnd) return;

	newWnd->currentFile = currentWindow->currentFile;
	f_addFileToWindow(newWnd, currentWindow->currentFile);

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
		((Window *)currNode->data)->active = false;

		nextNode = currNode->next;
		if (nextNode == NULL) {
			nextNode = currentWorkspace->windowList->firstNode;
		}

		((Window *)nextNode->data)->active = true;
		currentWorkspace->currentWindow = (Window *)nextNode->data;
		currentWindow = currentWorkspace->currentWindow;

	  	dw_requestRenderEvent(DW_RENDER_ALL);
	}
}
