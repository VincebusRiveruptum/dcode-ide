#include "files.h"

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

	newWindow->fileList = NULL;
	newWindow->currentFile = NULL;

	// By default will be minimized for now
	newWindow->status = WndStatus.MINIMIZED;
	newWindow->x = 0;
	newWindow->y = 0;

	newWindow->width = 0;
	newWindow->height = 0;

	newWindow->currentFileIndex = 0;
	newWindow->index = 0;

	return newWindow;
}

// Inits workspace with all atributtes zeroed.
Workspace *f_initWorkspace(){
	char *fullPath = NULL;
	Workspace *newWorkspace = NULL;

	if(currentWorkSpace){
		logger("[f_initWorkspace]: workspace already initialized.");
		return NULL;
	}
		
	fullPath = hal_fs_getAbsoluteCurrentPath();
		
	if(!fullPath){
		logger("[f_initWorkspac]: invalid fullpath.");	
		return NULL;
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

// Free a entire fileList
void f_freeFileList(List *fileList){
	Node *rec=NULL;
	File *file=NULL;

	if(!fileList)
		return;

	rec = fileList->firstNode;

	while(rec){
		file = (File*)rec->data;
		mem_arena_free(file->arena);
		rec = rec->next;
	}

	return;
}

// Free window list, fileList on each window and each file arena.
void f_freeWindowList(List *windowList){
	Node *rec=NULL, *tmp;
	if(!windowList)
		logger("[f_freeWorkspace]: workspace already free");
		return;

	// Freeing windowList
	rec = windowList->firstNode;

	while(rec){
		tmp = rec->next
		// Freeing fileList
		f_freeFileList(rec->fileList);
		free(rec);

		rec = tmp;
	}

	free(windowList);
}

// Free workspace, windows, fileList on each window and each file arena.
void f_freeWorkspace(){
	Node *rec=NULL, *tmp;
	Node *fileNode=NULL;

	if(!currentWorkSpace)
		logger("[f_freeWorkspace]: workspace already free");
		return;

	// Freeing windowList
	f_freeWindowList(currentWorkSpace->windowList);

	// Already previously freed
	//free(currentWorkSpace->currentWindow);
	free(currentWorkSpace->fullPath);
	free(currentWorkSpace);
}

// =======================================================================

File *f_addFileToWindow(Window *window, File *file){
	if (!file || !window->fileList || !window){
		logger("[f_addFileToWindow]: invalid data.");
		return NULL;
	}

	addGenericNode(window->fileList, (void*)file, NULL);

	return file;
}

Window *f_addWindowToWorkspace(Workspace *workspace, Window *window){
	if (!workspace || !workspace->windowList || !window ){
		logger("[f_addWindowToWorkspace]: invalid data.");
		return NULL;
	}

	addGenericNode(workspace->windowList, (void*)window, NULL);

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