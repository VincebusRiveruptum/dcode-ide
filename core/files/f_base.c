
#include "files.h"
#include "../config/config.h"

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
    //char searchArenaName[32] = {'\0'};
    int newFileCounter;
    
	Line *firstLine;
    MemoryArena *arena;
    File *newFile;	

    // If no filename param provided, we generate it
    if(filename == NULL){
        newFileCounter = _checkAvailableName();
        
        if(settings.DEFAULT_EXTENSION[0] == '\0'){
            logger(
				"[f_newFile]: Editor has no default file extension configuration yet!."
			);
            return;
        }
    
        sprintf(tempName, "newfile%d%s", newFileCounter, settings.DEFAULT_EXTENSION);
    }else{
        sprintf(tempName, "%s", filename);
    }

    arena = (MemoryArena *)mem_create_arena(tempName, MEM_ARENA_8K);

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
    newFile->name = 
		(char*)mem_arena_alloc(arena,sizeof(tempName) * sizeof(char));
    
    if(!newFile->name){
        logger("[f_newFile]: Could not assign temporary name to new file!");
        return;
    }
    
    memset(newFile->name, '\0', MAX_FILE_NAME * sizeof(char));
    strcpy(newFile->name, tempName);
    
	// TODO: maybe remove this attribute
	newFile->ext = f_getExtensionId(newFile->name);
    
    // Buffer will not be used for now as this is used for parsing to lines when opening a file.
    //newFile->buffer = NULL;
    newFile->bufferLength = 0;
    
    newFile->lines = (List*)mem_arena_alloc(arena,sizeof(List));
    memset(newFile->lines, 0, sizeof(List));
    
    newFile->deletedLines = (List*)mem_arena_alloc(arena, sizeof(List));
    memset(newFile->deletedLines, 0, sizeof(List));
    
	firstLine = (Line*)mem_arena_alloc(arena, sizeof(Line));
    
    if(!firstLine){
        logger("[f_newFile]: Could not create initial line to new file!");
        return;
    }
    
    firstLine->buffer = 
		(char*)mem_arena_alloc(
			arena, 
			sizeof(char) * MAX_FILE_LINE_LENGTH
		);
    
    if(!firstLine->buffer){
        logger("[f_newFile]: Could not create initial line BUFFER to new file!");
        return;
    }
    
    firstLine->length = 0;
    memset(firstLine->buffer, '\0', MAX_FILE_LINE_LENGTH);

    if(!newFile->lines){
        logger("[f_newFile]: Could not allocate list of lines to new file!");
        return;
    }
    
    addGenericNode(&newFile->lines,(void*) firstLine, arena);
        
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

    // Selection metadata
    newFile->selectedStartX = 0;
    newFile->selectedEndX = 0;
    newFile->selectedStartLine = 0;
    newFile->selectedEndLine = 0;
    
    newFile->selectedStartNode = NULL;
    newFile->selectedEndNode = NULL;

    newFile->isModified = false;
    newFile->isActive = false;

	newFile->currentFileSearch = NULL;

	// TODO: ADD newFile to currentWindow->fileList
    currentWindow->currentFile = newFile;

    ed_statusBarMessage("Created a new file.");
    ed_resetCursor();

    return;
}

/* OPEN FILE ==============================================================================*/

bool f_openFile(char *filename){
    //char searchArenaName[32];
    char *fileParsingBuffer = NULL;
	size_t fileSize = 0;

    FILE *fp = NULL;
    File *file = NULL;
    MemoryArena *arena = NULL;

	//memset(searchArenaName, '\0', 32);

    fp = fopen(filename, "r");

    // We are creating an arena per file
    if(fp == NULL){
        logger("\n[f_openFile]: Error: Could not open file %s", filename);
        return false;
    }

    /* We prepare the File arena */
	
	
	// TODO: 
	//	- Retrieve file size
	//	- Alloc arena with a size a bit larger to the file
	//	- If file line count increase when editing, resize memory space by migrating
	//	to a new file with a new arena.
	fileSize = mem_getFileClosestSize(fp);
    arena = mem_create_arena(filename + fs_getFileName(filename), fileSize);

    file = (File *)mem_arena_alloc(arena,sizeof(File));
    file->arena = arena;

    // We prepare the File struct 
    file = (File *)mem_arena_alloc(arena ,sizeof(File));
    file->name = (char*)mem_arena_alloc(arena, sizeof(char) * (strlen(filename) + 1));
    
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

    // Selection metadata
    file->selectedStartX = 0;
    file->selectedEndX = 0;
    file->selectedStartLine = 0;
    file->selectedEndLine = 0;
    
    file->selectedStartNode = NULL;
    file->selectedEndNode = NULL;


    if(!file->name){
        logger("\n[f_openFile]: Error: Could not allocate memory for file details");
        f_closeFile(file);
        fclose(fp);
        return false;
    }
        
    // Length of the file
    file->bufferLength = 0;

    while(!feof(fp)){
        fgetc(fp);
        file->bufferLength++;
    }
    file->bufferLength--;
    
    logger("\n[f_openFile]: File %s opened successfully, %d bytes", filename, file->bufferLength);
    
    rewind(fp); // or fseek(fp, 0, SEEK_SET);
    
    fileParsingBuffer = (char *)malloc(sizeof(char) * file->bufferLength + 1);
    
    logger("\n[f_openFile]: File buffer size %d (allocated %d)", file->bufferLength, file->bufferLength + 1);
    
    if(!fileParsingBuffer){
        logger("\n[f_openFile]: Error: Could not allocate memory for fileParsingBuffer");
        f_closeFile(file);
        fclose(fp);
        return false;
    }

	memset(fileParsingBuffer,'\0', sizeof(char)*file->bufferLength + 1);
	
    // Actually reading the file
    fread(fileParsingBuffer, sizeof(char), file->bufferLength, fp);

    // So the after opening hte file, it becomes the current file active
    file->currentFileSearch = NULL;

	// TODO: ADD newFile to currentWindow->fileList
	currentWindow->currentFile = file;

    _splitIntoLines(fileParsingBuffer, file->bufferLength, file, arena);

    // Line handling not the best way
    currentWindow->currentFile->currentLineNode = currentWindow->currentFile->lines->firstNode;

    currentWindow->currentFile->prevLine = NULL;
    currentWindow->currentFile->currentLine = 
        currentWindow->currentFile->lines->firstNode &&
        currentWindow->currentFile->lines->firstNode->data 
        ?
        currentWindow->currentFile->lines->firstNode->data 
        : 
        NULL ;

    currentWindow->currentFile->nextLine = 
        currentWindow->currentFile->lines->firstNode &&
        currentWindow->currentFile->lines->firstNode->next &&
        currentWindow->currentFile->lines->firstNode->next->data 
        ?
        currentWindow->currentFile->lines->firstNode->next->data 
        : 
        NULL ;

    currentWindow->currentFile->prevChar = '\0';
    currentWindow->currentFile->currentChar = 
        currentWindow->currentFile->currentLine->buffer[0] 
        ? currentWindow->currentFile->currentLine->buffer[0]
        : 0;
         
    currentWindow->currentFile->nextChar = 
        currentWindow->currentFile->currentLine->buffer[1] 
        ? currentWindow->currentFile->currentLine->buffer[1]
        : 0;

    fclose(fp);
    free(fileParsingBuffer);

    ed_statusBarMessage("Opened %s succesfully.", currentWindow->currentFile->name);
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
    
	oldFile = currentWindow->currentFile;

    if(!oldFile || !oldFile->arena){
        logger("\n[f_saveFile]: Error: No file selected");
        return;
    }

    if(!oldFile->name || oldFile->name[0] == '\0'){
        logger("\n[f_saveFile]: Error: No filename provided");
        return;
    }
    
    // We create a new arena for the file buffer
	// TODO: WE NEED TO REFLEX THE SIZE ON THE NEW ARENA FROM THE 
	// OLD ONE
    newArena = mem_create_arena(newArenaName, oldFile->arena->size);

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
    
    newFile->name = (char*)mem_arena_alloc(newArena,sizeof(char) * (strlen(oldFile->name) + 1));
    
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
    
    // Selection metadata, it will be reseted for now
    newFile->selectedStartX = 0;
    newFile->selectedEndX = 0;
    newFile->selectedStartLine = 0;
    newFile->selectedEndLine = 0;
    
    newFile->selectedStartNode = NULL;
    newFile->selectedEndNode = NULL;
    
    // We are going to travel the old file lines and copy them to the new file buffer
    currentNode = oldFile->lines->firstNode;
    lengthSum = _copyLines(oldFileileArena);

    // wE CANNOT COPY OLD POINTERS TO THE NEW FILE ARENA...
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
        return;
    }
    
    // We are going to dump all lines into the file
    while(currentNode != NULL){
        line = (Line *)currentNode->data;
        
        memcpy(fileParsingBuffer + offset, line->buffer, line->length);
        
        offset += line->length;         // Avoiding null terminator
        fileParsingBuffer[offset] = '\n';   // Replacing null terminator with newline
        offset++;

        currentNode = currentNode->next;
    }

    newFile->bufferLength = offset;

    // We dump the new file buffer to the file
    f_dumpBufferTofile(fileParsingBuffer, newFile->bufferLength, newFile->name);

    // We close the old file
    f_closeFile(oldFile);

    newFile->isModified = false;
    sprintf(newFile->name, "%s", newFile->name + fs_getFileName(newFile->name));

    // We add the new file arena and set it as the current file arena
	// TODO: ADD newFile to currentWindow->fileList
    currentWindow->currentFile = newFile;
	//currentFileSearch = &fileListSearchMetadata[newFile->fileIndex];

    free(fileParsingBuffer);

    ed_statusBarMessage("File %s saved successfully.", newFile->name);
    logger("[f_saveFile]: File %s saved successfully", newFile->name);
}

/* CLOSE FILE ==================================================================*/

void f_closeFile(File *file){
    char arenaName[64];
    sprintf(arenaName, "%s", file->arena->name);

    mem_arena_free(file->arena);
    
    file->arena = NULL;

    logger("[f_closeFile]: File %s closed successfully", arenaName);    
}

void f_triggerClose(bool end_program){
    int len = 0;
    char input = '\0';
    bool esc = false;
    char *filename = NULL;
    // endProgram IS A GLOBAL VARIABLE THO
    endProgram = end_program;

	// TODO: Check on all opened windows if there any opened files.
    if(!windowList || windowList->length == 0){
        logger(
			"[f_triggerClose]: No opened files, proceed to close app directly."
		);
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
        
        while(!(
            input == 'n' ||
            input == 'N' ||
            input == 'y' ||
            input == 'Y' ||
            (esc = hal_inp_isKeyPressed(HAL_KEY_ESC) == true) 

        )){
            input = hal_inp_getch();
        }

        if(esc == true) return;

        if(input == 'n' || input == 'N'){            
            // Here we should close the file or the program
            f_closeCurrentFile();
            return;
        } 
        
        ed_renderElements();
        
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

                ed_renderElements();

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

    // Close the file
    f_saveFile();
    f_closeCurrentFile();
}

void f_closeCurrentFile(){
    char oldFileName[255];
	File *nextFile = NULL;
	
    memset(oldFileName, '\0', 255);

    // If there are no current file opened, we fallback
    if(!currentWindow || !currentWindow->currentFile) 
		return;

    strcpy(oldFileName, currentWindow->currentFile->name);

    f_closeFile(currentWindow->currentFile);    
    f_flushSearchMetadata();

    // We find the next opened file
	nextFile = (File *)(currentWindow->fileList->firstNode->data);
	currentWindow->currentFile = nextFile;
    
    ed_statusBarMessage("%s closed successfully.", oldFileName);
    logger("[f_closdeCurrentFile]: %s closed successfully.", oldFileName);

    _updateCursor();
    ed_renderEvent = true;

    return;
}

// ============================================================================

void f_prepareFileNavDialog(){

	if(hal_inp_keysPressed(
		HAL_INP_TRIGGER_EDGE,
		2, 
		HAL_KEY_LALT, 
		HAL_KEY_LSHIFT
	)){
		f_onFileNavigation = true;
		/* 1. Dibujar el cuadro */
		dw_rectangle(
			textmemptr, 
			4, 
			4, 
			34, 
			16, 
			COLOR_RED, 
			COLOR_WHITE, 
			' ', 
			COLOR_WHITE, 
			COLOR_RED, 
			false, 
			DRAW_BORDER_SIMPLE, 
			NULL
		);

		ed_renderEvent = true;
	}
}

void f_drawFileNavDialog(){
    bool selected = false;
    File *fileptr;
	Node *currFileNode = NULL, *selectedNode = NULL;

    if (!currentWindow || !currentWindow->fileList) {
		logger("[f_drawFileNavDialog]: No window opened or no files opened in current window");
        return;
    }

	if(!hal_inp_isKeyDown(HAL_KEY_LALT)){
		f_onFileNavigation = false;
		return;
	}
    
    hal_inp_clearKeyboardBuffer();

	// Draw file list
	currFileNode = currentWindow->fileList->firstNode;

	if(!currFileNode){
		logger(
			"[f_drawFileNavDialog]: No valid node data."
		);
		return;
	}

	while(currFileNode){
		fileptr = (File*)currFileNode->data;
		
		if (fileptr) {
			selected = currentWindow->currentFileIndex;

			if(currentWindow->currentFile == currFileNode->data){
				selectedNode = currFileNode;
			}

			dw_writeBuffer(
				textmemptr, 
				"%s %s",
				5,
				5 + i,
				33,
				5 + i, 
				COLOR_WHITE, 
				COLOR_RED,
				(selected ? "*" : " "),
				fileptr->name
			);
		}

		currFileNode = currFileNode->next;
	}

	// LSHIFT edge detecting for traveling to the next file of 
	// the window.
	if(hal_inp_keysPressed(
		HAL_INP_TRIGGER_EDGE, 
		2,
		HAL_KEY_LALT,
		HAL_KEY_LSHIFT
	)){
		if(selectedNode && selectedNode->next)
			currentWindow->currentFile = (File*)selectedNode->next->data;
	}

	// Keyboard buffer update.
	hal_vid_refresh();
	hal_inp_updateKeyboard();
}
