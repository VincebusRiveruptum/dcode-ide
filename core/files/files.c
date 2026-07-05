
#include "files.h"
#include "../config/config.h"

void f_flushSearchMetadata();

FileArena fileList[MAX_ARENAS];
SearchMetadata fileListSearchMetadata[MAX_ARENAS];

MemoryArena *tmpPtrArena = NULL;
FileArena *currentFileArena = NULL;
SearchMetadata *currentFileSearch = NULL;

bool f_onFileNavigation = false;

bool endProgram = false;

/* File arena managing and utlis ======================================================*/

//void f_init(){
//    f_initFileArenas();
//}

//void f_initFileArenas(){
//    int i = 0;
//    for(i = 0; i < MAX_ARENAS; i++){
//        fileList[i].file = NULL;
//        fileList[i].arena = NULL;
//   }
//}

//FileArena *f_getFileArena(char *filename){
//    unsigned short i = 0;
//    
//    for(i = 0; i < MAX_ARENAS; i++){
//        if(fileList[i].file && !strcmp(fileList[i].file->name, filename)) return &fileList[i];
//    }   
//    return NULL;
//}

//FileArena *f_addFileArena(FileArena *fileArena){
//    unsigned short i = 0;
//    
//    for(i = 0; i < MAX_ARENAS; i++){
//        if(fileList[i].file == NULL && fileList[i].arena == NULL){
//            fileList[i] = *fileArena;
//            
//            // We update the fileIndex, this is used for accessing quickly to the 
//            // fileArena array.
//            fileArena->file->fileIndex = i;
//
//            return &fileList[i];
//        }
//    }
//    return NULL;
//}

// Create new file
// This will travel all windows files and check whats the
// next available file number. (for a name like newfile1.c)

int _checkAvailableName(){
    // TRAVEL ALL WINDOWS
		// TRAVEL ALL FILES
		// Get name that match the 'newfile' name scheme
		// Add to a counter
	
	// Return counter...
    return index + 1;
}

void _splitIntoLines(char *buffer, size_t bufferLength, File *file) {
    char *start;
    char *end;    
    char *p;
    size_t lineLen;
    Line *line;

    start = buffer;
    end = buffer + bufferLength;
    p = start;

	if(!file || !file->arena){
		logger("[_splitIntoLines]: File has no arena");
		return;
	}

    file->lines = (List *)mem_arena_alloc(file->arena, sizeof(List));
    memset(file->lines, 0, sizeof(List));

    file->deletedLines = (List *)mem_arena_alloc(file->arena, sizeof(List));
    memset(file->deletedLines, 0, sizeof(List));

    while (p < end) {
        if (*p == '\n') {
            lineLen = p - start;
            /* Strip trailing \r if present */
            if (lineLen > 0 && *(p - 1) == '\r') {
                lineLen--;
            }
            line = (Line *)mem_arena_alloc(file->arena, sizeof(Line));
            line->buffer = (char *)mem_arena_alloc(file->arena, MAX_FILE_LINE_LENGTH);
            memset(line->buffer, '\0', MAX_FILE_LINE_LENGTH);
            memcpy(line->buffer, start, lineLen);
            line->length = lineLen;

            addGenericNode(&file->lines, line, NULL, file->arena);
            start = p + 1;
        }
        p++;
    }

    /* Handle last line or file without trailing \n */
    if (start <= end) {
        lineLen = end - start;
        if (lineLen > 0 && start[lineLen-1] == '\r') lineLen--;

        line = (Line *)mem_arena_alloc(file->arena, sizeof(Line));
        line->buffer = (char *)mem_arena_alloc(file->arena, MAX_FILE_LINE_LENGTH);
        memset(line->buffer, '\0', MAX_FILE_LINE_LENGTH);
        memcpy(line->buffer, start, lineLen);
        line->length = lineLen;
        addGenericNode(&file->lines, line, NULL, file->arena);
    }
}

size_t _copyLines(FileArena *old, FileArena *new){
    size_t lengthSum = 0;
    size_t lineLen = 0;
    Node *currentNode = NULL;
    Line *oldLine = NULL;
    Line *newLine = NULL;

    new->file->lines = NULL;
    
    currentNode = old->file->lines->firstNode;

    while(currentNode != NULL){
        oldLine = (Line *)currentNode->data;
        
        newLine = (Line *)mem_arena_alloc(new->arena, NULL, sizeof(Line));
        newLine->buffer = (char *)mem_arena_alloc(new->arena, NULL, sizeof(char) * MAX_FILE_LINE_LENGTH);
        memset(newLine->buffer, '\0', sizeof(char) * MAX_FILE_LINE_LENGTH);

        lineLen = strlen(oldLine->buffer);

        memcpy(newLine->buffer, oldLine->buffer, lineLen + 1);
        newLine->length = lineLen;

        addGenericNode(&new->file->lines, (void *)newLine, NULL, new->arena);
        
        lengthSum += newLine->length + 1;
        currentNode = currentNode->next;
    }
    return lengthSum;
}

void f_closeFile(File *file){
    char arenaName[64];
    sprintf(arenaName, "%s", file->arena->name);

    mem_arena_free(file->arena, NULL);
    
    file->arena = NULL;

    logger("[f_closeFile]: File %s closed successfully", arenaName);    
}

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


// We check if the filename is a default one (ie newfile1.c)

bool f_isDefaultFileName(){
    char filename[8] = {'\0'};
    bool res = false;

    if(!currentFileArena->file->name){
        logger("[f_isDefaultFileName]: current file has no name!");
        return true;
    }

    strncpy(filename, currentFileArena->file->name, 7);
    
    res = (strcmp(filename, "newfile") == 0) ? true : false;

    return res;
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

    arena = (MemoryArena *)mem_create_arena(tempName, MEM_ARENA_FILE, MEM_ARENA_8K);

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
    newFile->file->name = 
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
    
    addGenericNode(&newFile->lines,(void*) firstLine, NULL, arena);
        
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
    
	// SEARCH ARENA WILL CREATED ON FIRST SEARCH
	// TODO: REMOVE CODE BELOW
    //sprintf(searchArenaName, "SRCH%d", currentFileArena->file->fileIndex);
	//currentFileSearch->arena = mem_create_arena (searchArenaName, MEM_ARENA_METADATA, MEM_ARENA_2K);
    
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
    arena = mem_create_arena(filename + fs_getFileName(filename), MEM_ARENA_FILE, fileSize);

    file = (File *)mem_arena_alloc(arena,sizeof(File));
    file->arena = arena;

    // We prepare the File struct 
    file = (File *)mem_arena_alloc(arena, NULL ,sizeof(File));
    file->name = (char*)mem_arena_alloc(arena, NULL, sizeof(char) * (strlen(filename) + 1));
    
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
        
        if(f_isDefaultFileName() == true){
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
    int i = 0;
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

// ==== SEARCH BEHAVIOR ==================================

// * Each file arena has its own fileSearchMetadata, its not insidie the File definition because this could cause memory usage issues when
// there are many Word matches, so it separate for better memory perfomance and control

// * The search metadata stores all matches in a pointer array. So,

// * Every time the file changes the searchMetadata of the currentFile
// MUST be flush, so there are no dangling pointers nor references to a word
// address that changed.

// * The fileListSearchMetadata INDEX is parallel to fileList currentFileArena
// this makes sure there are no collisions when flushing or filling the 
// metadata of an already opened file.

// The access to the current search meta data index is easy with 
// currentFileArena->file->fileindex

// FLUSH METADATA

void f_flushSearchMetadata(){
    if(
		!currentWindow || 
		!currentWindow->currentFile ||
		!currentWindow->currentFile->currentFileSearch) 
	return;

    mem_arena_free(
		currentWindow->currentFile->currentFileSearch->arena, 
		NULL
	);
	
    currentWindow->currentFile->currentFileSearch->arena = NULL; // Ensure pointer is cleared

    //currentFileSearch->dialogInputIndex = 0;
    //memset(currentFileSearch->dialogInputBuffer, '\0', 255);

    currentWindow->currentFile->currentFileSearch->wordCount = 0;
    currentWindow->currentFile->currentFileSearch->words = NULL;
    currentWindow->currentFile->currentFileSearch->currentWordNode = NULL;
}

void f_allocSearchMetadata(){
	if(
		!currentWindow || 
		!currentWindow->currentFile ||
		!currentWindow->currentFile->currentFileSearch
	) {
		logger("[f_allocSearchMetadat]: No valid currentFile data");
		return;
	}
	
    mem_arena_init(
		currentWindow->currentFile->currentFileSearch->arena,
		currentWindow->currentFile->name,
		MEM_ARENA_2K
	);
    
    currentWindow->currentFile->currentFileSearch->dialogInputIndex = 0;

    memset(
		currentWindow->currentFile->currentFileSearch->dialogInputBuffer,
		'\0',
		255
	);

    currentWindow->currentFile->currentFileSearch->wordCount = 0;
    currentWindow->currentFile->currentFileSearch->words = NULL;
    currentWindow->currentFile->currentFileSearch->currentWordNode = NULL;
}

// ============================================================================

void f_prepareFileNavDialog(){

	if(hal_inp_keysPressed(HAL_INP_TRIGGER_EDGE, 2, HAL_KEY_LALT, HAL_KEY_LSHIFT)){
		f_onFileNavigation = true;
		/* 1. Dibujar el cuadro */
		dw_rectangle(textmemptr, 4, 4, 34, 16, COLOR_RED, COLOR_WHITE, ' ', COLOR_WHITE, COLOR_RED, false, DRAW_BORDER_SIMPLE, NULL);
		ed_renderEvent = true;
	}
}

void f_drawFileNavDialog(){
    int i;
    int selectedIndex = 0;
    bool selected = false;
    File *fileptr;
	Node *currFileNode = NULL, selectedNode = NULL;

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
	currFileNode = currentWindow->fileList->first;

	if(!currFileNode){
		logger(
			"[f_drawFileNavDialog]: No valid node data."
		);
		return;
	}

	while(currFileNode){
		fileptr = (File*)currFileNode->data;
		
		if (fileptr->file) {
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
				fileptr->file->name
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

int _goBackPath(char *path){
    int end, len;
    char *endptr;
    
    end = strlen(path) - 1;
    len = end + 1;

    if(len <= 3) return len;
    
    endptr = path + end;

    if(*endptr == '\\' ){
        *endptr = '\0';
        endptr--;
    }

    while(endptr > path && *(endptr) != '\\'){
        *endptr = '\0';
        endptr--;
    };
    
    return (int)(endptr - path) + 1;
}

/*
    This is basically VSCODE's quick open feature. The behavior is the following:
    Prompt that has pre-filled the absolute path of the current position, filalble with the left and right arrows
    Select file of the current selected path with the up and down arrows.
    The file list will be reactive depending on hte detected path from the input prompt.
*/
void f_quickOpenFileDialog(){
    int stepIndex = 0;
    int vis_offset = 0;
    int dialogStartY = 0;
    int dialogEndY = 0;
    int entriesLen = 0;
    int selectedEntry = 0;
    int entryIndex = 0;
    int clearMarkPoint = 0;
    int listHeight = 11;
    int entryScrollY = 0;
    int selectedEntryScrollY = 0;
    char scrollChar = '\0';
    char selectedEntryFullPath[512];
    char currentPath[255];
    bool isSelected = false;

    Directory *currPathDirectory = NULL;
    Node *node = NULL;
    FileEntry *fileEntry = NULL;
    FileEntry *selectedFileEntry = NULL;

    memset(selectedEntryFullPath, '\0', 512);
    memset(currentPath, '\0', 255);

    hal_fs_getAbsoluteCurrentPath(currentPath, 255);
    
    if(currentPath[0] == '\0'){
        logger("[ed_quickOpenFileDialog]: Failed to retrieve currentPath");
    }

    if (strlen(currentPath) < sizeof(currentPath) - 1) {
        strcat(currentPath, "\\");
    }

    vis_offset = (VIDEO_COLS / 4);

    dialogStartY = 2;
    dialogEndY = 18; 

    dw_rectangle(
		textmemptr, 
		vis_offset,
		dialogStartY,
		VIDEO_COLS - vis_offset,
		dialogEndY,
		COLOR_BLUE,
		COLOR_WHITE,
		' ',
		COLOR_WHITE,
		COLOR_BLUE,
		false,
		DRAW_BORDER_SIMPLE,
		"OPEN FILE"
	);

    stepIndex = strlen(currentPath);
    
    hal_inp_waitForRelease();

    do{
        // Key selection
        if(hal_inp_isKeyPressed(HAL_KEY_UP)){
            selectedEntry = 
            selectedEntry > 1
                ? selectedEntry - 1
                : 1; 

            if(selectedEntry - entryScrollY <= 0)
                entryScrollY--;

        }else if(hal_inp_isKeyPressed(HAL_KEY_DOWN)){
            selectedEntry = 
            selectedEntry < entriesLen
                ? selectedEntry + 1
                : entriesLen; 

            // We calculate the scrolling
            if(selectedEntry - entryScrollY - 2 > listHeight )
                entryScrollY++;
                
        }else if(hal_inp_isKeyPressed(HAL_KEY_ENTER)){
            // If we press enter, we have to detect if the entry is either a directory or a file
            // 
            if(selectedFileEntry && strcmp(selectedFileEntry->name, "..") == 0){         // .. path
                stepIndex = _goBackPath(currentPath);       
                selectedEntry = 0;
                selectedFileEntry = NULL;
            }else if(selectedFileEntry && selectedFileEntry->isDirectory){
                //// Nothing happens
                strcat(currentPath, selectedFileEntry->name);
                strcat(currentPath, "\\");
                stepIndex = strlen(currentPath);
                selectedEntry = 0;
                selectedFileEntry = NULL;
            }else if(selectedFileEntry){
                // We open the file
                // TODO: SANITIZE buffer by removing the chars until the last directory
                //snprintf(selectedEntryFullPath, sizeof(selectedEntryFullPath), "%s%s", currentPath, selectedFileEntry->name);
                sprintf(selectedEntryFullPath, "%s%s", currentPath, selectedFileEntry->name);
                f_openFile(selectedEntryFullPath);
                _updateCursor();
                ed_renderEvent = true;
                if (currPathDirectory) hal_fs_freeDirectory(currPathDirectory);
                return;
            }
        }else{

            // Freeing up list of files each time there is a change 
            // in the prompt.

            // Ok, for the file selection we have to clear mark a flag 
            // for the selected file
            // By
            if(currPathDirectory) hal_fs_freeDirectory(currPathDirectory);

            selectedFileEntry = NULL;

            currPathDirectory = hal_fs_getDirectoryFileList(currentPath);

            if(!currPathDirectory){
                logger(
                    "[ed_quickOpenFileDialog]: Could not get currPathDirectory or FileEntry list for selection!");
                return;
            }

            // Draw list of files
            entriesLen = currPathDirectory->fileEntries->length;
            node = currPathDirectory->fileEntries->firstNode;
            entryIndex = 0;
            selectedEntryScrollY = 0;

            while(node != NULL && (selectedEntryScrollY) - 1 <= listHeight){
                fileEntry = (FileEntry*)node->data;
                entryIndex++;
                // We write the filename under the prompt
                selectedEntryScrollY = entryIndex - entryScrollY;
                
                isSelected = (entryIndex == selectedEntry) ? true : false;
                        
                if(selectedEntryScrollY <= 0) continue;
                    // We mar the selected item or not
                if(isSelected == true){

                    selectedFileEntry = fileEntry;
                    dw_writeBuffer(
                        textmemptr,
                        "%s", 
                        vis_offset + 1, 
                        3 + selectedEntryScrollY + 1, 
                        VIDEO_COLS - vis_offset - 1, 
                        3 + selectedEntryScrollY + 1,
                        COLOR_BLUE, 
                        COLOR_WHITE, 
                        fileEntry->name
                    );
                }else{
                    dw_writeBuffer(
                        textmemptr,
                        "%s", 
                        vis_offset + 1, 
                        3 + selectedEntryScrollY + 1, 
                        VIDEO_COLS - vis_offset - 1, 
                        3 + selectedEntryScrollY + 1, 
                        COLOR_WHITE, 
                        COLOR_BLUE, 
                        fileEntry->name
                    );
                }

                // Draw scrollbar if there are more items than the 
                // the height of the list
                if(entriesLen - 2 > listHeight){
                    // UP ARROW
                    if((selectedEntryScrollY - 1) == 0){
                        scrollChar = 0x1E;
                    // DOWN ARROW
                    }else if(selectedEntryScrollY - 2 == listHeight){
                        scrollChar = 0x1F;
                    }else{
                        scrollChar = 0xB1;
                    }

                    dw_writeBuffer(
                        textmemptr,
                        "%c", 
                        VIDEO_COLS - vis_offset - 1, 
                        3 + selectedEntryScrollY + 1, 
                        VIDEO_COLS - vis_offset - 1, 
                        3 + selectedEntryScrollY + 1, 
                        COLOR_BLUE, 
                        COLOR_LIGHT_GRAY, 
                        scrollChar
                    );
                }

                node = node->next;
            }

            // Clear list container until touches bottom
            clearMarkPoint = selectedEntryScrollY;

            if(clearMarkPoint < dialogEndY - 5){
                while(clearMarkPoint < dialogEndY - 5){     
                    clearMarkPoint++;
                    dw_writeBuffer(
                        textmemptr,
                        "%s", 
                        vis_offset + 1, 
                        clearMarkPoint + 4, 
                        VIDEO_COLS - vis_offset - 1, 
                        clearMarkPoint + 4, 
                        COLOR_WHITE, 
                        COLOR_BLUE, 
                        "            "
                    );
                }
            }
            
            // Draw rect in the middle, 1/4 will be the start and the
            // end, so i it will always be in the center
            hal_vid_refresh();
            ed_async_scanf(
                vis_offset + 1, 
                3, (2 * vis_offset) - 1, 
                currentPath, 
                strlen(currentPath), 
                &stepIndex
            );
            
        }
        
        hal_inp_updateKeyboard();
    }while(!hal_inp_isKeyPressed(HAL_KEY_ESC));

    if(currPathDirectory) 
        hal_fs_freeDirectory(currPathDirectory);

    if (
		currentWindow &&
		currentWindow->currentFile
	){
        _updateCursor();
    }

    ed_renderEvent = true;
}
