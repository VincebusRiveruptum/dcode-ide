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

void f_dumpBufferTofile(
    char *buffer, 
    size_t bufferLength, 
    char *filename
){
    size_t i = 0;
    FILE *fp = NULL;

    fp = fopen(filename, "w");
    if(fp == NULL){
        logger(
            "[f_dumpBufferTofile]: Error: Could not open file %s", 
            filename
        );

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

// This calculates a LIne full size
// This could be a macro
size_t f_getLineSize(){
    size_t lineSize = 0;

    lineSize = (
        ALIGN16(sizeof(Node)) +
        ALIGN16(sizeof(Line)) +
        ALIGN16(settings.MAX_LINE_LENGTH + 1)
    );

    logger("[f_getLineSize] Actual Line Object size : %d", lineSize);

    return lineSize;
}

// This checks if adding a new line could overflow the 
// textArea file memory arena.
// Returns boolean value.
// FUll = false
// space available = True
bool f_checkNewLineMemory(TextArea *textArea){

    if(
        !textArea ||
        !textArea->file ||
        !textArea->file->arena
    ){
        logger("[f_checNewLineMemory] Error. Invalid file universe.");
        exit(1);
    }
    
    if(
        (f_getLineSize() + textArea->file->arena->offset) >= 
        textArea->file->arena->size 
        &&
        (textArea->file->deletedLines->length == 0)
    ){
        
        logger("[f_checNewLineMemory] Error. OUT OF MEMORY FOR NEW LINE!.");
        return false;
    }

    
    return true;
}

// This function calculates the complete File object
// size from the content of the file to be opened.
size_t f_getFileClosestSize(FILE *fp, char *filename){
	size_t fileSize = 0, lineMem, fileMem, finalSize;
    int c;
    unsigned int lineCount = 0;

	if(!fp)
		return 0;

    while((c = fgetc(fp)) != EOF){
        if(c == '\n') 
            lineCount++;

        fileSize++;
    }
	rewind(fp);

    // if there is just one line with no line jump in
    // the file
    lineCount += (c != '\n');
     
    lineMem = (
        ALIGN16(sizeof(Node)) +
        ALIGN16(sizeof(Line)) +
        ALIGN16(settings.MAX_LINE_LENGTH + 1)
    );

    fileMem = (
        ALIGN16(sizeof(File)) +      // File struct
        ALIGN16(sizeof(List)) +      // Lines
        ALIGN16(sizeof(List)) +      // Deleted Lines
        ALIGN16(strlen(filename) + 1)
    );

    finalSize = (
        fileMem + (lineMem * lineCount) + settings.FILE_HEADROOM
    );

    logger(
        "[f_openFile]: File size: %d, File Obj size in bytes: %d, file instance size : %d, line breaks: %d",
        fileSize, 
        sizeof(File), 
        finalSize,
        lineCount
    );
 
	return finalSize;
}

// This basically adds more headroom to a file when memory arena gets
// full after editing the file.
void f_resizeTextArea(TextArea *textArea){
    File *newFile = NULL;
    MemoryArena *oldArena = NULL;
    
    if(
        !textArea ||
        !textArea->file ||
        !textArea->file->arena
    ){
        logger("[f_resizeTextArea] Error : Invalid textArea, file or arena.");
        exit(1);
    }
    
    logger("[f_resizeTextArea]: Resizing textArea file %s", textArea->file->name);
    // We clone the file into the new Arena
    oldArena = textArea->file->arena;
    newFile = f_copyFileObject(textArea->file, true);
    
    textArea->file = newFile;
    // Update textArea metadata
    f_refreshTextArea(textArea);
    
    // Freeing up old arena
    mem_arena_free(oldArena);
    
    return;
}

// This checks if a file is already bein refered in
// a TextArea instance in all windows and workspaces.
bool f_checkFileRefs(File *file){
    Node *wndNode, *taNode;
    List *taList = NULL;
    TextArea *textArea = NULL;
    if(!currentWorkspace){
        logger("[f_checkFileRefs]: Error: No currentWorskapce.");
        exit(1);
    } 

    if(!currentWorkspace->windowList){
        logger("[f_checkFileRefs]: Error: No window list.");
        exit(1);
    } 

    wndNode = currentWorkspace->windowList->firstNode;

    while(wndNode){

        taList = ((EditorWindow*)(wndNode->data))->textAreaList;

        if(!taList){
            logger("[f_checkFileRefs]: Error: No textArea list.");
            exit(1);
        }
        
        taNode = taList->firstNode;

        while(taNode){
            textArea = (TextArea*)taNode->data;

            if(
                !textArea ||
                !textArea->file
            ){
                logger("[f_checkFileRefs]: Error: Invalid textArea or data.");
                exit(1);
            }
            
            if(
                strcmp(file->name, textArea->file->name) == 0 &&
                (file == textArea->file)
            )   return true;

            taNode = taNode->next;
        }

        wndNode = wndNode->next;
    }

    return false;
}

// This will check if the currentWindow textArea
// file is refenced in other windows
bool f_checkCurrFileRefs(File *file){
    Node *wndNode;
    TextArea *textArea = NULL;
    if(!currentWorkspace){
        logger("[f_checkFileRefs]: Error: No currentWorskapce.");
        exit(1);
    } 

    if(!currentWorkspace->windowList){
        logger("[f_checkFileRefs]: Error: No window list.");
        exit(1);
    } 

    wndNode = currentWorkspace->windowList->firstNode;

    while(wndNode){
        textArea = ((EditorWindow*)(wndNode->data))->textArea;

        if(
            (strcmp(textArea->file->name, file->name) == 0) &&
            textArea->file == file
        ) return true;


        wndNode = wndNode->next;
    }

    return false;
}

// This update OLD file pointer references in textAreaList
// window and workspace  with the NEW right reference.
// Returns count of file refs updated.
int f_updateFileRefs(File *oldFile, File *newFile){
    Node *wndNode, *taNode;
    List *taList = NULL;
    TextArea *textArea = NULL;
    int refCount = 0;

    if(!currentWorkspace){
        logger("[f_updateFileRefs]: Error: No currentWorskapce.");
        exit(1);
    } 

    if(!currentWorkspace->windowList){
        logger("[f_updateFileRefs]: Error: No window list.");
        exit(1);
    } 

    wndNode = currentWorkspace->windowList->firstNode;

    while(wndNode){

        taList = ((EditorWindow*)(wndNode->data))->textAreaList;

        if(!taList){
            logger("[f_updateFileRefs]: Error: No textArea list.");
            exit(1);
        }
        
        taNode = taList->firstNode;

        while(taNode){
            textArea = (TextArea*)taNode->data;

            if(
                !textArea ||
                !textArea->file
            ){
                logger("[f_updateFileRefs]: Error: Invalid textArea or data.");
                exit(1);
            }
            
            if(
                strcmp(oldFile->name, textArea->file->name) == 0 &&
                (oldFile == (textArea->file))
            ){
                logger("[f_updateFileRefs]: Found ref %d.", refCount);
                textArea->file = newFile;

                f_refreshTextArea(textArea);
                
                refCount ++; 
            };

            taNode = taNode->next;
        }

        wndNode = wndNode->next;
    }

    return refCount;
}

// This creates a FIle object replica.
File *f_copyFileObject(File *oldFile, bool headroom){
	char *newArenaName = "NEW";
    File *newFile = NULL;
    MemoryArena *newArena = NULL;

    newArena = (MemoryArena*)mem_arena_create(
        newArenaName, 
        oldFile->arena->size + (headroom ? settings.FILE_HEADROOM : 0)
    );

    if(!newArena){
        logger("[f_saveFile]: Could not create swapping arena!");
        exit(1);
    }
    
    newFile = (File *)mem_arena_alloc(newArena, sizeof(File));
    
    if(!newFile){
        logger("[f_saveFile]: Could not create swapping FILE!");
        exit(1);
    }
    
    memset(newFile, 0, sizeof(File));
    
    newFile->arena = newArena;
    
    newFile->name = (char*)mem_arena_alloc(newArena, sizeof(char) * (strlen(oldFile->name) + 1));
    
    if(!newFile->name){
        logger("[f_saveFile]: Could not allocate file name!");
        exit(1);
    }
    
    sprintf(newFile->name, "%s", oldFile->name);

    newFile->ext = f_getExtensionId(newFile->name);
    
    _copyLines(oldFile, newFile);
    
    newFile->deletedLines = createList(newFile->arena);

    return newFile;
}

// This calculates the total number of characrers of the file 
// instance in the textArea 
unsigned long f_getFileFullLength(TextArea *textArea){
    Node *rec = NULL;
    Line *line = NULL;
    unsigned long count = 0;
    
    if(
        !textArea ||
        !textArea->file ||
        !textArea->file->lines ||
        !textArea->file->lines->firstNode ||
        !textArea->file->lines->lastNode
    ){
        logger("[f_getFileFullLength] Error: Invalid file universe.");
        exit(1);
    }

    rec = textArea->file->lines->firstNode;

    while(rec){
        line = (Line *)rec->data;

        if(!line){
            logger("[f_getFileFullLength] Error: Line is NULL.");
            exit(1);
        }

        count += line->length;

        rec = rec->next;
    }
    
    return count;
}

/* NEW FILE ==============================================================================*/

void f_newFile(char *filename){
    static char tempName[MAX_FILE_NAME] = {'\0'};
    int newFileCounter;
    size_t arenaSize = 0;
    
	Line *firstLine;
    MemoryArena *fileArena = NULL;
    File *file = NULL;
    
    TextArea *textArea = NULL;

    if (!currentWorkspace) {
        currentWorkspace = f_createWorkspace();
        if (!currentWorkspace) {
            logger("[f_newFile]: Failed initializing workspace");
            exit(1);
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
            exit(1);
        }
        sprintf(tempName, "newfile%d%s", newFileCounter, settings.DEFAULT_EXTENSION);
    }else{
        sprintf(tempName, "%s", filename);
    }

    // TODO: IMPLEMENT MEMORY RESIZE ON DEMAND
    // WHEN THE FILE SIZE INCREASES WEN
    // EDITING.
    arenaSize = settings.FILE_HEADROOM;
    
	fileArena = (MemoryArena *)mem_arena_create(tempName, arenaSize);
	
    if(!fileArena){
        logger("[f_newFile]: Failed creating memory fileArena");
        exit(1);
    }

    file = (File *)mem_arena_alloc(fileArena, sizeof(File));
    if(!file){
        logger("[f_newFile]: Could not create new file!.");
        exit(1);
    }

    file->arena = fileArena;

    file->name = (char*)mem_arena_alloc(fileArena, sizeof(tempName) * sizeof(char));
    if(!file->name){
        logger("[f_newFile]: Could not assign temporary name to new file!");
        exit(1);
    }
    
    strcpy(file->name, tempName);

	file->ext = f_getExtensionId(file->name);
    file->bufferLength = 0;
    
    file->lines = (List*)mem_arena_alloc(fileArena, sizeof(List));
    file->deletedLines = (List*)mem_arena_alloc(fileArena, sizeof(List));

	firstLine = (Line*)mem_arena_alloc(fileArena, sizeof(Line));
    if(!firstLine){
        logger("[f_newFile]: Could not create initial line to new file!");
        exit(1);
    }
    
    firstLine->buffer = 
        (char*)mem_arena_alloc(fileArena, sizeof(char) * (MAX_FILE_LINE_LENGTH + 1));
    if(!firstLine->buffer){
        logger("[f_newFile]: Could not create initial line BUFFER to new file!");
        exit(1);
    }
    
    firstLine->length = 0;

    addGenericNode(&file->lines, (void*)firstLine, fileArena);
        
    // We create the textArea file wrapper
    textArea = f_createTextArea(file->name);
    textArea->file = file;
    textArea->currentLineNode = file->lines->firstNode;
    textArea->currentLine = firstLine;
    textArea->searchMetadata = f_createSearchMetadata(textArea->file->name);
    
    f_addTextAreaToWindow(currentWindow, textArea);
    currentWindow->textArea = textArea;

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
    TextArea *textArea = NULL;
    MemoryArena *fileArena = NULL;

    if (!currentWorkspace) {
        currentWorkspace = f_createWorkspace();
        if (!currentWorkspace) {
            logger("[f_openFile]: Failed initializing workspace");
            exit(1);
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

	fileSize = f_getFileClosestSize(fp, filename);
    fileArena = mem_arena_create(fs_getFileName(filename), fileSize);

    if(!fileArena){
        logger("[f_openFile]: Failed creating memory arena");
        fclose(fp);
        exit(1);
    }

    file = (File *)mem_arena_alloc(fileArena, sizeof(File));
	
    if(!file){
        logger("[f_openFile]: Failed allocating File struct");
        fclose(fp);
        exit(1);
    }

    memset(file, 0, sizeof(File));
    file->arena = fileArena;
    file->name = (char*)mem_arena_alloc(fileArena, sizeof(char) * (strlen(filename) + 1));

    if(!file->name){
        logger("[f_openFile]: Error: Could not allocate memory for file details");
        fclose(fp);
        exit(1);
    }

    sprintf(file->name, "%s", filename);
    file->ext = f_getExtensionId(file->name);

    file->lines = NULL;
    file->deletedLines = NULL;

    fseek(fp, 0, SEEK_END);

    file->bufferLength = ftell(fp);
    rewind(fp);
    
    fileParsingBuffer = (char *)malloc(file->bufferLength + 1)
    ;
    if(!fileParsingBuffer){
        logger("[f_openFile]: Error: Could not allocate memory for fileParsingBuffer");
        fclose(fp);
        exit(1);
    }

	memset(fileParsingBuffer, '\0', file->bufferLength + 1);
	
    file->bufferLength = fread(fileParsingBuffer, sizeof(char), file->bufferLength, fp);

    textArea = f_createTextArea(file->name);

    if(!textArea){
        logger("[f_openFile]: Error: Could not allocate memory for textArea");
        fclose(fp);
        exit(1);
    }
    
    textArea->file = file;
	textArea->searchMetadata = f_createSearchMetadata(file->name);
    currentWindow->textArea = textArea;
    
    f_addTextAreaToWindow(currentWindow, textArea);

    _splitIntoLines(fileParsingBuffer, file->bufferLength, file);

    currentWindow->textArea->currentLineNode = 
        currentWindow->textArea->file->lines->firstNode;
    currentWindow->textArea->prevLine = NULL;
    currentWindow->textArea->currentLine = 
        currentWindow->textArea->file->lines->firstNode &&
        currentWindow->textArea->file->lines->firstNode->data 
        ? currentWindow->textArea->file->lines->firstNode->data 
        : NULL;

    currentWindow->textArea->nextLine = 
        currentWindow->textArea->file->lines->firstNode &&
        currentWindow->textArea->file->lines->firstNode->next &&
        currentWindow->textArea->file->lines->firstNode->next->data 
        ? currentWindow->textArea->file->lines->firstNode->next->data 
        : NULL;

    currentWindow->textArea->prevChar = '\0';
    if (currentWindow->textArea->currentLine != NULL) {
        currentWindow->textArea->currentChar = 
            currentWindow->textArea->currentLine->buffer[0] 
            ? currentWindow->textArea->currentLine->buffer[0]
            : 0;
             
        currentWindow->textArea->nextChar = 
            currentWindow->textArea->currentLine->buffer[1] 
            ? currentWindow->textArea->currentLine->buffer[1]
            : 0;
    } else {
        currentWindow->textArea->currentChar = 0;
        currentWindow->textArea->nextChar = 0;
    }

    fclose(fp);
    free(fileParsingBuffer);

    ed_statusBarMessage(
        "Opened %s successfully.", 
        currentWindow->textArea->file->name
    );

  	dw_requestRenderEvent(DW_RENDER_ALL);

	return true;
}

/* SAVE FILE ==============================================================================*/
void f_saveFile(){
	size_t offset = 0;
    char *fileParsingBuffer = NULL;
    Line *line = NULL;
    File *oldFile = NULL;
    Node *currNode = NULL;
    Node *currentNode = NULL;
    TextArea *textArea = NULL;
    unsigned long fulllen = 0;
    
    Node *currentTextAreaNode = NULL;

    textArea = currentWindow->textArea;

    if(!textArea){
        logger("[f_saveFile]: Error: Invalid textArea");
        exit(1);
        return;
    }

	oldFile = textArea->file;

    if(!oldFile || !oldFile->arena){
        logger("[f_saveFile]: Error: No file selected");
        exit(1);
        return;
    }

    if(!oldFile->name || oldFile->name[0] == '\0'){
        logger("[f_saveFile]: Error: No filename provided");
        exit(1);
        return;
    }
    
    currentTextAreaNode = 
        getNodeByDataPtr(
            &currentWindow->textAreaList,
            (void*)textArea
        );

    textArea = f_copyTextArea(textArea, false);

    if(currentTextAreaNode)
        currentTextAreaNode->data = (TextArea*)textArea;
    

    /* Replace oldFile with newFile in active window's fileList in-place */
    // Update all textArea FIle old pointer no the new pointer.
    // and free up old file arena.
    f_updateFileRefs(oldFile, textArea->file);

    textArea->file->isModified = false;

    fulllen = f_getFileFullLength(textArea);

    // Dump content to file
	fileParsingBuffer = (char*)malloc(sizeof(char) + fulllen);
    
    if(!fileParsingBuffer){
        logger("[f_copyTextArea]: Could not allocate file buffer!");
        exit(1);
        return;
    }

    memset(fileParsingBuffer, '\0', sizeof(char) * fulllen);

	currentNode = textArea->file->lines->firstNode;

    if(currentNode == NULL){
        logger("\n[f_textArea]: Error: No lines found");
        free(fileParsingBuffer);
        exit(1);
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

    textArea->file->bufferLength = offset;

    f_dumpBufferTofile(
        fileParsingBuffer, 
        textArea->file->bufferLength, 
        textArea->file->name
    );
    
    free(fileParsingBuffer);

    currentWindow->textArea = textArea;

    ed_statusBarMessage("File %s saved successfully.", textArea->file->name);
    logger("[f_saveFile]: File %s saved successfully", textArea->file->name);

  	dw_requestRenderEvent(DW_RENDER_ALL);
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

    if(
        !currentWindow ||
        !currentWindow->textArea ||
        !currentWindow->textArea->file
    ){
        logger("[f_triggerClose]: No active file, proceed to close app directly.");
        if (end_program) {
            endProgram = true;
        }
        return;
    }

    if(currentWindow->textArea->file->isModified == true){
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
            f_closeCurrentTextArea();
            return;
        } 
        
        //ed_renderWindows(currentWorkspace);
        if(_isDefaultFileName() == true){
            dw_writeBuffer(
                textmemptr, 
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

            if(esc == true) 
                return;

            strcpy(currentWindow->textArea->file->name, filename);
            
        }
        
        f_saveFile();
    }

    f_closeCurrentTextArea();

  	dw_requestRenderEvent(DW_RENDER_ALL);
}

void f_setCurrentFileAsModified(){
    if(
        !currentWorkspace ||
        !currentWorkspace->currentWindow ||
        !currentWorkspace->currentWindow->textArea ||
        !currentWorkspace->currentWindow->textArea->file
    )   return;
    
    currentWorkspace->currentWindow->textArea->file->isModified = true;
}