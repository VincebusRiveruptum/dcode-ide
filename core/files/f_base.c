#include "files.h"

bool f_onFileNavigation = false;
bool endProgram = false;

void f_dumpToFile(char *filename){
    FILE *fp = fopen(filename, "w");
    int i=0;
    int j=0;
    if(fp == NULL){
        logger("\n[f_dumpToFile]: Error: Could not open file %s", filename);
        return;
    }
    
    for(i=0; i < VIDEO_ROWS; i++){
        for(j=0; j < VIDEO_COLS; j++){
            fputc(textmemptr[i * VIDEO_COLS + j], fp);
        }
        fputc('\n', fp);
    }
    
    fclose(fp);   
}

void f_dumpBufferTofile(char *buffer, size_t bufferLength, char *filename){
    size_t i = 0;
    FILE *fp = NULL;

    fp = fopen(filename, "w");
    if(fp == NULL){
        logger("\n[f_dumpBufferTofile]: Error: Could not open file %s", filename);
        return;
    }
    
    // We ignore the last character, as it is a line jump, this could
    // add a new line each time we save...
    for(i=0; i < bufferLength - 1; i++){
        fputc(buffer[i], fp);
    }
    
    fclose(fp);   
}

unsigned char f_getExtensionId(char *filename){
    char *ext = fs_getFileExtension(filename);

    if(!ext) return FILE_EXTENSION_TXT;
    
    if(strcmp(ext, ".c") == 0 || strcmp(ext, ".C") == 0) return FILE_EXTENSION_C;
    if(strcmp(ext, ".cpp") == 0 || strcmp(ext, ".CPP") == 0) return FILE_EXTENSION_C;
    if(strcmp(ext, ".h") == 0 || strcmp(ext, ".H") == 0) return FILE_EXTENSION_C;
    if(strcmp(ext, ".txt") == 0 || strcmp(ext, ".TXT") == 0) return FILE_EXTENSION_TXT;
    if(strcmp(ext, ".py") == 0 || strcmp(ext, ".PY") == 0) return FILE_EXTENSION_PYTHON;
    if(strcmp(ext, ".js") == 0 || strcmp(ext, ".JS") == 0) return FILE_EXTENSION_JS;

    return FILE_EXTENSION_TXT;
}

/* NEW FILE ==============================================================================*/

void f_newFile(char *filename){
    static char tempName[MAX_FILE_NAME] = {'\0'};
    int newFileCounter;
    
	Line *firstLine;
    MemoryArena *arena;
    File *newFile;	
    size_t arenaSize = 0;

    if (!currentWorkspace) {
        currentWorkspace = f_createWorkspace();
        if (!currentWorkspace) {
            logger("[f_newFile]: Failed initializing workspace");
            return;
        }
    }
    if (!currentWindow) {
        currentWindow = f_createWindow();
        f_addWindowToWorkspace(currentWorkspace, currentWindow);
        currentWorkspace->currentWindow = currentWindow;
        
        currentWindow->x = 0;
        currentWindow->y = 0;
        currentWindow->width = VIDEO_COLS - 1;
        currentWindow->height = VIDEO_ROWS - 2;
        currentWindow->active = true;
    }

    if(filename == NULL){
        newFileCounter = _checkAvailableName();
        if(settings.DEFAULT_EXTENSION[0] == '\0'){
            logger("[f_newFile]: Editor has no default file extension configuration yet!.");
            return;
        }
        sprintf(tempName, "newfile%d%s", newFileCounter, settings.DEFAULT_EXTENSION);
    }else{
        sprintf(tempName, "%s", filename);
    }

    arenaSize = 
		settings.MAX_FILE_INSTANCE_SIZE > 0 
		? settings.MAX_FILE_INSTANCE_SIZE 
		: MEM_ARENA_256K;
    
	arena = (MemoryArena *)mem_arena_create(tempName, arenaSize);
	
    if(!arena){
        logger("[f_newFile]: Failed creating memory arena");
        return;
    }

    newFile = (File *)mem_arena_alloc(arena, sizeof(File));
    if(!newFile){
        logger("[f_newFile]: Could not create new file!.");
        return;
    }

    newFile->arena = arena;
    newFile->name = (char*)mem_arena_alloc(arena, sizeof(tempName) * sizeof(char));
    if(!newFile->name){
        logger("[f_newFile]: Could not assign temporary name to new file!");
        return;
    }
    
    strcpy(newFile->name, tempName);

	newFile->ext = f_getExtensionId(newFile->name);
    newFile->bufferLength = 0;
    
    newFile->lines = (List*)mem_arena_alloc(arena, sizeof(List));
    newFile->deletedLines = (List*)mem_arena_alloc(arena, sizeof(List));

	firstLine = (Line*)mem_arena_alloc(arena, sizeof(Line));
    if(!firstLine){
        logger("[f_newFile]: Could not create initial line to new file!");
        return;
    }
    
    firstLine->buffer = (char*)mem_arena_alloc(arena, sizeof(char) * (MAX_FILE_LINE_LENGTH + 1));
    if(!firstLine->buffer){
        logger("[f_newFile]: Could not create initial line BUFFER to new file!");
        return;
    }
    
    firstLine->length = 0;

    addGenericNode(&newFile->lines, (void*)firstLine, arena);
        
    newFile->scrollY = 0;
    newFile->scrollX = 0;
    newFile->cursorLine = 0;
    newFile->cursorCol = 0; 

    newFile->currentLineNode = newFile->lines->firstNode;
    newFile->prevLine = NULL;
    newFile->currentLine = firstLine;
    newFile->nextLine = NULL;
    
    newFile->prevChar = 0;
    newFile->currentChar = 0;
    newFile->nextChar = 0;

    newFile->selectedStartX = 0;
    newFile->selectedEndX = 0;
    newFile->selectedStartLine = 0;
    newFile->selectedEndLine = 0;
    newFile->selectedStartNode = NULL;
    newFile->selectedEndNode = NULL;

    newFile->isModified = false;
    newFile->isActive = false;
	newFile->currentFileSearch = f_createSearchMetadata(newFile->name);

    f_addFileToWindow(currentWindow, newFile);
    currentWindow->currentFile = newFile;

    ed_statusBarMessage("Created a new file.");
    ed_updateCursor();

  	dw_requestRenderEvent(DW_RENDER_ALL);
}

/* OPEN FILE ==============================================================================*/

bool f_openFile(char *filename){
    char *fileParsingBuffer = NULL;
	size_t fileSize = 0;
    FILE *fp = NULL;
    File *file = NULL;
    MemoryArena *arena = NULL;
    size_t defaultMax = 0;

    if (!currentWorkspace) {
        currentWorkspace = f_createWorkspace();
        if (!currentWorkspace) {
            logger("[f_openFile]: Failed initializing workspace");
            return false;
        }
    }
    if (!currentWindow) {
        currentWindow = f_createWindow();
        f_addWindowToWorkspace(currentWorkspace, currentWindow);
        currentWorkspace->currentWindow = currentWindow;
        
        currentWindow->x = 0;
        currentWindow->y = 0;
        currentWindow->width = VIDEO_COLS - 1;
        currentWindow->height = VIDEO_ROWS - 2;
        currentWindow->active = true;
    }

    fp = fopen(filename, "r");
    if(fp == NULL){
        logger("\n[f_openFile]: Error: Could not open file %s", filename);
        return false;
    }

	fileSize = mem_getFileClosestSize(fp);

    defaultMax =
		(settings.MAX_FILE_INSTANCE_SIZE > 0) 
		? settings.MAX_FILE_INSTANCE_SIZE 
		: MEM_ARENA_512K;

    if (fileSize < defaultMax) {
        fileSize = defaultMax;
    } else {
        fileSize += defaultMax;
    }

    arena = mem_arena_create(fs_getFileName(filename), fileSize);
    if(!arena){
        logger("[f_openFile]: Failed creating memory arena");
        fclose(fp);
        return false;
    }

    file = (File *)mem_arena_alloc(arena, sizeof(File));
	
    if(!file){
        logger("[f_openFile]: Failed allocating File struct");
        fclose(fp);
        return false;
    }
    memset(file, 0, sizeof(File));
    file->arena = arena;

    file->name = (char*)mem_arena_alloc(arena, sizeof(char) * (strlen(filename) + 1));
    if(!file->name){
        logger("[f_openFile]: Error: Could not allocate memory for file details");
        fclose(fp);
        return false;
    }
    sprintf(file->name, "%s", filename);
    file->ext = f_getExtensionId(file->name);

    file->lines = NULL;
    file->deletedLines = NULL;
    file->scrollY = 0;
    file->scrollX = 0;
    file->cursorLine = 0;
    file->cursorCol = 0;
    file->prevChar = '\0';
    file->currentChar = '\0';
    file->nextChar = '\0';

    file->selectedStartX = 0;
    file->selectedEndX = 0;
    file->selectedStartLine = 0;
    file->selectedEndLine = 0;
    file->selectedStartNode = NULL;
    file->selectedEndNode = NULL;
	
	file->currentFileSearch = f_createSearchMetadata(file->name);
    fseek(fp, 0, SEEK_END);

    file->bufferLength = ftell(fp);
    rewind(fp);
    
    fileParsingBuffer = (char *)malloc(file->bufferLength + 1);
    if(!fileParsingBuffer){
        logger("[f_openFile]: Error: Could not allocate memory for fileParsingBuffer");
        fclose(fp);
        return false;
    }
	memset(fileParsingBuffer, '\0', file->bufferLength + 1);
	
    file->bufferLength = fread(fileParsingBuffer, sizeof(char), file->bufferLength, fp);

    f_addFileToWindow(currentWindow, file);
    currentWindow->currentFile = file;

    _splitIntoLines(fileParsingBuffer, file->bufferLength, file);

    currentWindow->currentFile->currentLineNode = currentWindow->currentFile->lines->firstNode;
    currentWindow->currentFile->prevLine = NULL;
    currentWindow->currentFile->currentLine = 
        currentWindow->currentFile->lines->firstNode &&
        currentWindow->currentFile->lines->firstNode->data 
        ? currentWindow->currentFile->lines->firstNode->data 
        : NULL;

    currentWindow->currentFile->nextLine = 
        currentWindow->currentFile->lines->firstNode &&
        currentWindow->currentFile->lines->firstNode->next &&
        currentWindow->currentFile->lines->firstNode->next->data 
        ? currentWindow->currentFile->lines->firstNode->next->data 
        : NULL;

    currentWindow->currentFile->prevChar = '\0';
    if (currentWindow->currentFile->currentLine != NULL) {
        currentWindow->currentFile->currentChar = 
            currentWindow->currentFile->currentLine->buffer[0] 
            ? currentWindow->currentFile->currentLine->buffer[0]
            : 0;
             
        currentWindow->currentFile->nextChar = 
            currentWindow->currentFile->currentLine->buffer[1] 
            ? currentWindow->currentFile->currentLine->buffer[1]
            : 0;
    } else {
        currentWindow->currentFile->currentChar = 0;
        currentWindow->currentFile->nextChar = 0;
    }

    fclose(fp);
    free(fileParsingBuffer);

    ed_statusBarMessage("Opened %s successfully.", currentWindow->currentFile->name);

  	dw_requestRenderEvent(DW_RENDER_ALL);

	return true;
}

/* SAVE FILE ==============================================================================*/

void f_saveFile(){
	size_t offset = 0;
    size_t lengthSum = 0;
    
	char *newArenaName = "NEW";
    char *fileParsingBuffer = NULL;
    
    Line *line = NULL;
    MemoryArena *newArena = NULL;
    Node *currentNode = NULL;
    File *oldFile = NULL, *newFile = NULL;
    Node *currNode = NULL;
    
	oldFile = currentWindow->currentFile;

    if(!oldFile || !oldFile->arena){
        logger("[f_saveFile]: Error: No file selected");
        return;
    }

    if(!oldFile->name || oldFile->name[0] == '\0'){
        logger("[f_saveFile]: Error: No filename provided");
        return;
    }
    
    newArena = mem_arena_create(newArenaName, oldFile->arena->size);
    if(!newArena){
        logger("[f_saveFile]: Could not create swapping arena!");
        return;
    }
    
    newFile = (File *)mem_arena_alloc(newArena, sizeof(File));
    if(!newFile){
        logger("[f_saveFile]: Could not create swapping FILE!");
        return;
    }

    memset(newFile, 0, sizeof(File));
    newFile->arena = newArena;
    
    newFile->name = (char*)mem_arena_alloc(newArena, sizeof(char) * (strlen(oldFile->name) + 1));
    if(!newFile->name){
        logger("[f_saveFile]: Could not allocate file name!");
        return;
    }
    
    sprintf(newFile->name, "%s", oldFile->name);
    newFile->ext = f_getExtensionId(newFile->name);

    newFile->scrollY = oldFile->scrollY;
    newFile->scrollX = oldFile->scrollX;
    newFile->cursorLine = oldFile->cursorLine;
    newFile->cursorCol = oldFile->cursorCol;
    
    newFile->prevChar = oldFile->prevChar;
    newFile->currentChar = oldFile->currentChar;
    newFile->nextChar = oldFile->nextChar;

    newFile->isActive = oldFile->isActive;
    
    newFile->selectedStartX = 0;
    newFile->selectedEndX = 0;
    newFile->selectedStartLine = 0;
    newFile->selectedEndLine = 0;
    newFile->selectedStartNode = NULL;
    newFile->selectedEndNode = NULL;
    
    currentNode = oldFile->lines->firstNode;
    lengthSum = _copyLines(oldFile, newFile);

    newFile->currentLineNode = getNodeByIndex(&(newFile->lines), newFile->cursorLine);
    if (newFile->currentLineNode) {
        newFile->prevLine = 
            newFile->currentLineNode->prev &&
            newFile->currentLineNode->prev->data
            ? newFile->currentLineNode->prev->data
            : NULL ;

        newFile->currentLine = newFile->currentLineNode->data;
        newFile->nextLine = 
            newFile->currentLineNode->next &&
            newFile->currentLineNode->next->data
            ? newFile->currentLineNode->next->data
            : NULL;
    } else {
        newFile->prevLine = NULL;
        newFile->currentLine = NULL;
        newFile->nextLine = NULL;
    }
    
    fileParsingBuffer = (char*)malloc(sizeof(char) * (lengthSum + 1));
    if(!fileParsingBuffer){
        logger("[f_saveFile]: Could not allocate file buffer!");
        return;
    }
    memset(fileParsingBuffer, '\0', sizeof(char) * (lengthSum + 1));

    if(currentNode == NULL){
        logger("\n[f_saveFile]: Error: No lines found");
        free(fileParsingBuffer);
        return;
    }
    
    while(currentNode != NULL){
        line = (Line *)currentNode->data;
        memcpy(fileParsingBuffer + offset, line->buffer, line->length);
        offset += line->length;
        fileParsingBuffer[offset] = '\n';
        offset++;
        currentNode = currentNode->next;
    }

    newFile->bufferLength = offset;
    f_dumpBufferTofile(fileParsingBuffer, newFile->bufferLength, newFile->name);

    /* Replace oldFile with newFile in active window's fileList in-place */
    currNode = currentWindow->fileList->firstNode;
    while (currNode != NULL) {
        if (currNode->data == oldFile) {
            currNode->data = newFile;
            break;
        }
        currNode = currNode->next;
    }

    f_closeFile(oldFile);

    newFile->isModified = false;
	newFile->currentFileSearch = f_createSearchMetadata(newFile->name);

    currentWindow->currentFile = newFile;

    free(fileParsingBuffer);

    ed_statusBarMessage("File %s saved successfully.", newFile->name);
    logger("[f_saveFile]: File %s saved successfully", newFile->name);

  	dw_requestRenderEvent(DW_RENDER_ALL);
}

/* CLOSE FILE ==================================================================*/

void f_closeFile(File *file){
    char arenaName[64];
    sprintf(arenaName, "%s", file->arena->name);

	f_freeSearchMetadata(file->currentFileSearch);
    mem_arena_free(file->arena);
    logger("[f_closeFile]: File %s closed successfully", arenaName);    
}

void f_closeCurrentFile(){
    char oldFileName[255];
	File *nextFile = NULL;
	
	Window *toDelete = NULL, *wnd = NULL;
	Node *rec = NULL;
	Window *neighbor = NULL;

    memset(oldFileName, '\0', 255);

    if(!currentWindow || !currentWindow->currentFile) 
		return;

    strcpy(oldFileName, currentWindow->currentFile->name);

    f_deleteFileFromWindow(
		currentWindow, 
		currentWindow->currentFile
	);
	
	if (currentWindow->fileList->length > 0) {
		nextFile = (File *)(currentWindow->fileList->firstNode->data);
		currentWindow->currentFile = nextFile;
	} else {

		currentWindow->currentFile = NULL;
		
		if (currentWorkspace->windowList->length > 1) {
			toDelete = currentWindow;
			rec = currentWorkspace->windowList->firstNode;
			neighbor = NULL;
			
			while (rec != NULL) {
				wnd = (Window *)rec->data;
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

void f_triggerClose(bool end_program){
    int len = 0;
    char input = '\0';
    bool esc = false;
    char *filename = NULL;
    endProgram = end_program;

    if(
		!currentWorkspace ||
		!currentWorkspace->windowList ||
		currentWorkspace->windowList->length == 0
	){
        logger("[f_triggerClose]: No opened workspace or windows, proceed to close app directly.");
        if (end_program) {
            endProgram = true;
        }
        return;
    }

    if(!currentWindow || !currentWindow->currentFile){
        logger("[f_triggerClose]: No active file, proceed to close app directly.");
        if (end_program) {
            endProgram = true;
        }
        return;
    }

    if(currentWindow->currentFile->isModified == true){
        dw_writeBuffer(
			textmemptr, 
			"File modified, save? Y/N ",
			0,
			VIDEO_ROWS - 1 ,
			26, VIDEO_ROWS - 1, 
			settings.STATUSBAR_COLOR_TEXT, 
			settings.STATUSBAR_COLOR_BG
		);

        hal_vid_refresh();
        hal_inp_clearKeyboardBuffer();
        while(!(
            input == 'n' ||
            input == 'N' ||
            input == 'y' ||
            input == 'Y' ||
            (esc = hal_inp_isKeyPressed(HAL_KEY_ESC)) == true) 
        ){
            input = hal_inp_getch();
        }

        if(esc == true){
            logger("[t_triggerClose]: User canceled file close.");            
            dw_requestRenderEvent(DW_RENDER_ALL);
            endProgram = false;
            return;
        };

        if(input == 'n' || input == 'N'){            
            f_closeCurrentFile();
            return;
        } 
        
        //ed_renderWindows(currentWorkspace);
        if(_isDefaultFileName() == true){
            dw_writeBuffer(textmemptr, 
				"File name: ",
				0,
				VIDEO_ROWS - 1, 
				10,
				VIDEO_ROWS - 1, 
				settings.STATUSBAR_COLOR_TEXT, 
				settings.STATUSBAR_COLOR_BG
			);

            hal_vid_refresh();
            
            while((len <= 3 || len > 12)){
                dw_writeBuffer(
					textmemptr,
					"",
					11,
					VIDEO_ROWS - 1,
					VIDEO_COLS - 1,
					VIDEO_ROWS - 1,
					settings.STATUSBAR_COLOR_TEXT,
					settings.STATUSBAR_COLOR_BG
				);

                filename = ed_scanf(11, VIDEO_ROWS - 1, 32);
                
                if(filename == NULL) return;

                len = strlen(filename);

                //ed_renderWindows(currentWorkspace);

                if(len <= 3 || len > 12){
                    dw_writeBuffer(
						textmemptr,
						"Invalid filename! Try again",
						0,
						VIDEO_ROWS - 1,
						30,
						VIDEO_ROWS - 1,
						settings.STATUSBAR_COLOR_TEXT,
						settings.STATUSBAR_COLOR_BG
					);

                    hal_vid_refresh();
                }
            }
            if(esc == true) return;
            strcpy(currentWindow->currentFile->name, filename);
        }
    }

    f_saveFile();
    f_closeCurrentFile();

  	dw_requestRenderEvent(DW_RENDER_ALL);
}

void f_setCurrentFileAsModified(){
    if(
        !currentWorkpace ||
        !currentWorkpace->currentWindow ||
        !currentWorkpace->currentWindow->currentFile ||
    )   return;
    
    currentWorkpace->currentWindow->currentFile->isModified = true;
}