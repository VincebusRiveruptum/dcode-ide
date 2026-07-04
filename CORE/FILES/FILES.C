
#include "FILES.H"
#include "../CONFIG/CONFIG.H"

void f_flushSearchMetadata();

FileArena fileList[MAX_ARENAS];
SearchMetadata fileListSearchMetadata[MAX_ARENAS];

MemoryArena *tmpPtrArena = NULL;
FileArena *currentFileArena = NULL;
SearchMetadata *currentFileSearch = NULL;

bool endProgram = false;

/* File arena managing and utlis ======================================================*/

void f_init(){
    f_initFileArenas();
}

void f_initFileArenas(){
    int i = 0;
    for(i = 0; i < MAX_ARENAS; i++){
        fileList[i].file = NULL;
        fileList[i].arena = NULL;
    }
}

FileArena *f_getFileArena(char *filename){
    unsigned short i = 0;
    
    for(i = 0; i < MAX_ARENAS; i++){
        if(fileList[i].file && !strcmp(fileList[i].file->name, filename)) return &fileList[i];
    }   
    return NULL;
}
FileArena *f_addFileArena(FileArena *fileArena){
    unsigned short i = 0;
    
    for(i = 0; i < MAX_ARENAS; i++){
        if(fileList[i].file == NULL && fileList[i].arena == NULL){
            fileList[i] = *fileArena;
            
            // We update the fileIndex, this is used for accessing quickly to the 
            // fileArena array.
            fileArena->file->fileIndex = i;

            return &fileList[i];
        }
    }
    return NULL;
}

void f_closeFile(FileArena *fileArena){
    char arenaName[64];
    sprintf(arenaName, "%s", fileArena->arena->name);

    fileArena->file = NULL;
    
    mem_arena_free(fileArena->arena, NULL);
    
    fileArena->arena = NULL;

    logger("[f_closeFile]: File %s closed successfully", arenaName);    
}

/* Create new file */
int f_checkAvailableName(){
    FileArena *fileArena;
    char *fileName, *occurence;
    int i=0, index=0, prevIndex=0;

    for(i=0; i < MAX_ARENAS; i++){
        fileArena = &fileList[i];

        if(!fileArena) continue;
        if(!fileArena->file) continue;
        
        fileName = fileArena->file->name;
        prevIndex = index;
        
        occurence = strstr(fileName, "newfile");
        
        if(!occurence) continue;
        
        // We travel the occurence and find the number at the end
        index = strtol(occurence + 7, NULL, 10);
        
        if(prevIndex > index){
            index = prevIndex;
        }
    }

    return index + 1;
}

void f_splitIntoLines(char *buffer, size_t bufferLength, File *file, MemoryArena *arena) {
    char *start;
    char *end;    
    char *p;
    size_t lineLen;
    Line *line;

    start = buffer;
    end = buffer + bufferLength;
    p = start;

    file->lines = (List *)mem_arena_alloc(arena, NULL, sizeof(List));
    memset(file->lines, 0, sizeof(List));

    file->deletedLines = (List *)mem_arena_alloc(arena, NULL, sizeof(List));
    memset(file->deletedLines, 0, sizeof(List));

    while (p < end) {
        if (*p == '\n') {
            lineLen = p - start;
            /* Strip trailing \r if present */
            if (lineLen > 0 && *(p - 1) == '\r') {
                lineLen--;
            }
            line = (Line *)mem_arena_alloc(arena, NULL, sizeof(Line));
            line->buffer = (char *)mem_arena_alloc(arena, NULL, MAX_FILE_LINE_LENGTH);
            memset(line->buffer, '\0', MAX_FILE_LINE_LENGTH);
            memcpy(line->buffer, start, lineLen);
            line->length = lineLen;

            addGenericNode(&file->lines, line, NULL, arena);
            start = p + 1;
        }
        p++;
    }

    /* Handle last line or file without trailing \n */
    if (start <= end) {
        lineLen = end - start;
        if (lineLen > 0 && start[lineLen-1] == '\r') lineLen--;

        line = (Line *)mem_arena_alloc(arena, NULL, sizeof(Line));
        line->buffer = (char *)mem_arena_alloc(arena, NULL, MAX_FILE_LINE_LENGTH);
        memset(line->buffer, '\0', MAX_FILE_LINE_LENGTH);
        memcpy(line->buffer, start, lineLen);
        line->length = lineLen;
        addGenericNode(&file->lines, line, NULL, arena);
    }
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


// We get where the file name starts
size_t f_getFileName(char *filename){
    size_t length=0;
    size_t slashPos=0;
    
    while(filename[length] != '\0'){ // Find the last dot
        if(filename[length] == '/' || filename[length] == '\\' ){ // Count the number of slashes
            slashPos = length;
        }
        length++;
    }

    if(slashPos == 0) return 0;

    return slashPos + 1;
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
    char *ext = f_getFileExtension(filename);

    if(!ext) return FILE_EXTENSION_TXT;
    
    if(strcmp(ext, ".c") == 0 || strcmp(ext, ".C") == 0) return FILE_EXTENSION_C;
    if(strcmp(ext, ".cpp") == 0 || strcmp(ext, ".CPP") == 0) return FILE_EXTENSION_C;
    if(strcmp(ext, ".h") == 0 || strcmp(ext, ".H") == 0) return FILE_EXTENSION_C;
    if(strcmp(ext, ".txt") == 0 || strcmp(ext, ".TXT") == 0) return FILE_EXTENSION_TXT;
    if(strcmp(ext, ".py") == 0 || strcmp(ext, ".PY") == 0) return FILE_EXTENSION_PYTHON;
    if(strcmp(ext, ".js") == 0 || strcmp(ext, ".JS") == 0) return FILE_EXTENSION_JS;

    return FILE_EXTENSION_TXT;
}

// We get where the file extension starts
char *f_getFileExtension(char *filename){
    size_t i=strlen(filename);
    
    while(i > 0 && filename[i] != '.'){ // Find the last dot
        i--;
    }

    return strrchr(filename, '.');
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

/* NEW FILE ==============================================================================*/

void f_newFile(char *filename){
    MemoryArena *newArena;
    FileArena *newFileArena;
    
    char tempName[MAX_FILE_NAME] = {'\0'};
    char searchArenaName[32] = {'\0'};
    int newFileCounter;
    Line *firstLine;

    // If no filename param provided, we generate it
    if(filename == NULL){
        newFileCounter = f_checkAvailableName();
        
        if(settings.DEFAULT_EXTENSION[0] == '\0'){
            logger("[f_newFile]: Editor has no default file extension configuration yet!.");
            return;
        }
    
        sprintf(tempName, "newfile%d%s", newFileCounter, settings.DEFAULT_EXTENSION);
    }else{
        sprintf(tempName, "%s", filename);
    }

    newArena = (MemoryArena *)mem_create_arena(tempName, MEM_ARENA_FILE, MEM_ARENA_512K);

    if(!newArena){
        logger("[f_newFile]: Failed creating memory arena");
        return;
    }

    newFileArena = (FileArena *)mem_arena_alloc(newArena, NULL, sizeof(FileArena));
    
    if(!newFileArena){
        logger("[f_newFile]: Could not create new file arena!.");
        return;
    }
    
    newFileArena->arena = newArena;
    newFileArena->file = (File *)mem_arena_alloc(newArena, NULL, sizeof(File));

    if(!newFileArena->file){
        logger("[f_newFile]: Could not create new file!");
        return;
    }

    newFileArena->file->name = (char*)mem_arena_alloc(newArena, NULL, sizeof(tempName) * sizeof(char));
    
    if(!newFileArena->file->name){
        logger("[f_newFile]: Could not assign temporary name to new file!");
        return;
    }
    
    memset(newFileArena->file->name, '\0', MAX_FILE_NAME * sizeof(char));
    strcpy(newFileArena->file->name, tempName);
    newFileArena->file->ext = f_getExtensionId(newFileArena->file->name);
    
    // Buffer will not be used for now as this is used for parsing to lines when opening a file.
    //newFileArena->file->buffer = NULL;
    newFileArena->file->bufferLength = 0;
    
    newFileArena->file->lines = (List*)mem_arena_alloc(newArena, NULL, sizeof(List));
    memset(newFileArena->file->lines, 0, sizeof(List));
    
    newFileArena->file->deletedLines = (List*)mem_arena_alloc(newArena, NULL, sizeof(List));
    memset(newFileArena->file->deletedLines, 0, sizeof(List));
    firstLine = (Line*)mem_arena_alloc(newArena, NULL, sizeof(Line));
    
    if(!firstLine){
        logger("[f_newFile]: Could not create initial line to new file!");
        return;
    }
    
    firstLine->buffer = (char*)mem_arena_alloc(newArena, NULL, sizeof(char) * MAX_FILE_LINE_LENGTH);
    
    if(!firstLine->buffer){
        logger("[f_newFile]: Could not create initial line BUFFER to new file!");
        return;
    }
    
    firstLine->length = 0;
    memset(firstLine->buffer, '\0', MAX_FILE_LINE_LENGTH);

    if(!newFileArena->file->lines){
        logger("[f_newFile]: Could not allocate list of lines to new file!");
        return;
    }
    
    addGenericNode(&newFileArena->file->lines,(void*) firstLine, NULL, newArena);
        
    newFileArena->file->scrollY = 0;
    newFileArena->file->scrollX = 0;
    newFileArena->file->cursorLine = 0;
    newFileArena->file->cursorCol = 0; 

    newFileArena->file->currentLineNode = newFileArena->file->lines->firstNode;

    newFileArena->file->prevLine = NULL;
    newFileArena->file->currentLine = firstLine;
    newFileArena->file->nextLine = NULL;
    
    newFileArena->file->prevChar = 0;
    newFileArena->file->currentChar = 0;
    newFileArena->file->nextChar = 0;

    // Selection metadata
    newFileArena->file->selectedStartX = 0;
    newFileArena->file->selectedEndX = 0;
    newFileArena->file->selectedStartLine = 0;
    newFileArena->file->selectedEndLine = 0;
    
    newFileArena->file->selectedStartNode = NULL;
    newFileArena->file->selectedEndNode = NULL;

    newFileArena->file->isModified = false;
    newFileArena->file->isActive = false;
    

    currentFileArena = f_addFileArena(newFileArena);
    currentFileSearch = &fileListSearchMetadata[currentFileArena->file->fileIndex];

    sprintf(searchArenaName, "SRCH%d", currentFileArena->file->fileIndex);
    currentFileSearch->arena = mem_create_arena (searchArenaName, MEM_ARENA_METADATA, MEM_ARENA_2K);
    
    ed_statusBarMessage("Created a new file.");

    ed_resetCursor();
    return;
}

/* OPEN FILE ==============================================================================*/

bool f_openFile(char *filename){
    char searchArenaName[32];
    FILE *fp = NULL;
    File *file = NULL;
    MemoryArena *arena = NULL;
    FileArena *fileArena = NULL;
    char *fileParsingBuffer = NULL;

    memset(searchArenaName, '\0', 32);

    fp = fopen(filename, "r");

    // We are creating an arena per file
    if(fp == NULL){
        logger("\n[f_openFile]: Error: Could not open file %s", filename);
        return false;
    }

    /* We prepare the File arena */
    arena = mem_create_arena(filename + f_getFileName(filename), MEM_ARENA_FILE, MEM_ARENA_512K);

    fileArena = (FileArena *)mem_arena_alloc(arena, NULL ,sizeof(FileArena));
    fileArena->arena = arena;
    // We don't have the File struct yet, so we can't assign it to fileArena->file
    // fileArena->file = ...; 

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
        f_closeFile(fileArena);
        fclose(fp);
        return false;
    }
    
    // Assign the file struct to the file arena
    fileArena->file = file;
    
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
        f_closeFile(fileArena);
        fclose(fp);
        return false;
    }

	memset(fileParsingBuffer,'\0', sizeof(char)*file->bufferLength + 1);
	
    // Actually reading the file
    fread(fileParsingBuffer, sizeof(char), file->bufferLength, fp);

    // So the after opening hte file, it becomes the current file active
    currentFileArena = f_addFileArena(fileArena);
    currentFileSearch = &fileListSearchMetadata[currentFileArena->file->fileIndex];

    sprintf(searchArenaName, "SRCH%d", currentFileArena->file->fileIndex);
    currentFileSearch->arena = mem_create_arena (searchArenaName, MEM_ARENA_METADATA, MEM_ARENA_2K);
    
    f_splitIntoLines(fileParsingBuffer, file->bufferLength, file, arena);

    // Line handling not the best way
    currentFileArena->file->currentLineNode = currentFileArena->file->lines->firstNode;

    currentFileArena->file->prevLine = NULL;
    currentFileArena->file->currentLine = 
        currentFileArena->file->lines->firstNode &&
        currentFileArena->file->lines->firstNode->data 
        ?
        currentFileArena->file->lines->firstNode->data 
        : 
        NULL ;

    currentFileArena->file->nextLine = 
        currentFileArena->file->lines->firstNode &&
        currentFileArena->file->lines->firstNode->next &&
        currentFileArena->file->lines->firstNode->next->data 
        ?
        currentFileArena->file->lines->firstNode->next->data 
        : 
        NULL ;

    currentFileArena->file->prevChar = '\0';
    currentFileArena->file->currentChar = 
        currentFileArena->file->currentLine->buffer[0] 
        ? currentFileArena->file->currentLine->buffer[0]
        : 0;
         
    currentFileArena->file->nextChar = 
        currentFileArena->file->currentLine->buffer[1] 
        ? currentFileArena->file->currentLine->buffer[1]
        : 0;

    fclose(fp);
    free(fileParsingBuffer);

    ed_statusBarMessage("Opened %s succesfully.", currentFileArena->file->name);
    return true;
}

/* SAVE FILE ==============================================================================*/

void f_saveFile(){
    // This will not use an arena for now
    Line *line;
    MemoryArena *oldArena, *newArena;
    Node *currentNode;
    FileArena *oldFileArena, *newFileArena;
    char *newArenaName = "NEW";
    size_t offset = 0;
    size_t lengthSum = 0;
    char *fileParsingBuffer = NULL;
    
    oldFileArena = currentFileArena;
    oldArena = oldFileArena->arena;

    if(!oldFileArena || !oldFileArena->file){
        logger("\n[f_saveFile]: Error: No file selected");
        return;
    }

    if(oldFileArena->file->name == NULL || oldFileArena->file->name[0] == '\0'){
        logger("\n[f_saveFile]: Error: No filename provided");
        return;
    }
    
    // We create a new arena for the file buffer
    newArena = mem_create_arena(newArenaName, oldArena->type, MEM_ARENA_512K);

    if(!newArena){
        logger("[f_saveFile]: Could not create swapping arena!");
        return;
    }
    
    newFileArena = (FileArena *)mem_arena_alloc(newArena, NULL, sizeof(FileArena));

    if(!newFileArena){
        logger("[f_saveFile]: Could not create swapping FILE arena!");
        return;
    }
    memset(newFileArena, 0, sizeof(FileArena));

    newFileArena->arena = newArena;
    newFileArena->file = (File *)mem_arena_alloc(newArena, NULL, sizeof(File));
    if(!newFileArena->file){
         logger("[f_saveFile]: Could not create swapping FILE struct!");
         return;
    }
    memset(newFileArena->file, 0, sizeof(File));
    newFileArena->file->name = (char*)mem_arena_alloc(newArena, NULL, sizeof(char) * (strlen(oldFileArena->file->name) + 1));
    
    if(!newFileArena->file->name){
        logger("[f_saveFile]: Could not allocate file name!");
        return;
    }
    
    sprintf(newFileArena->file->name, "%s", oldFileArena->file->name);
    newFileArena->file->ext = f_getExtensionId(newFileArena->file->name);

    newFileArena->file->scrollY = oldFileArena->file->scrollY;
    newFileArena->file->scrollX = oldFileArena->file->scrollX;
    newFileArena->file->cursorLine = oldFileArena->file->cursorLine;
    newFileArena->file->cursorCol = oldFileArena->file->cursorCol;
    
    newFileArena->file->prevChar = oldFileArena->file->prevChar;
    newFileArena->file->currentChar = oldFileArena->file->currentChar;
    newFileArena->file->nextChar = oldFileArena->file->nextChar;

    newFileArena->file->isActive = oldFileArena->file->isActive;
    
    // Selection metadata, it will be reseted for now
    newFileArena->file->selectedStartX = 0;
    newFileArena->file->selectedEndX = 0;
    newFileArena->file->selectedStartLine = 0;
    newFileArena->file->selectedEndLine = 0;
    
    newFileArena->file->selectedStartNode = NULL;
    newFileArena->file->selectedEndNode = NULL;
    
    // We are going to travel the old file lines and copy them to the new file buffer
    currentNode = oldFileArena->file->lines->firstNode;
    lengthSum = _copyLines(oldFileArena, newFileArena);

    // wE CANNOT COPY OLD POINTERS TO THE NEW FILE ARENA...
    newFileArena->file->currentLineNode = getNodeByIndex(&(newFileArena->file->lines), newFileArena->file->cursorLine);
    if (newFileArena->file->currentLineNode) {
        newFileArena->file->prevLine = 
            newFileArena->file->currentLineNode->prev &&
            newFileArena->file->currentLineNode->prev->data
            ? newFileArena->file->currentLineNode->prev->data
            : NULL ;

        newFileArena->file->currentLine = newFileArena->file->currentLineNode->data;
        newFileArena->file->nextLine = 
            newFileArena->file->currentLineNode->next &&
            newFileArena->file->currentLineNode->next->data
            ? newFileArena->file->currentLineNode->next->data
            : NULL;
    } else {
        newFileArena->file->prevLine = NULL;
        newFileArena->file->currentLine = NULL;
        newFileArena->file->nextLine = NULL;
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

    newFileArena->file->bufferLength = offset;

    // We dump the new file buffer to the file
    f_dumpBufferTofile(fileParsingBuffer, newFileArena->file->bufferLength, newFileArena->file->name);

    // We close the old file
    f_closeFile(oldFileArena);
    
    // We add the new file arena and set it as the current file arena
    currentFileArena = f_addFileArena(newFileArena);

    currentFileSearch = &fileListSearchMetadata[newFileArena->file->fileIndex];

    newFileArena->file->isModified = false;
    sprintf(newFileArena->arena->name, "%s", newFileArena->file->name + f_getFileName(newFileArena->file->name));

    free(fileParsingBuffer);

    ed_statusBarMessage("File %s saved successfully.", newFileArena->file->name);
    logger("[f_saveFile]: File %s saved successfully", newFileArena->file->name);
}

/* CLOSE FILE ==================================================================*/

void f_triggerClose(bool end_program){
    int len = 0;
    char input = '\0';
    bool esc = false;
    char *filename = NULL;
    // endProgram IS A GLOBAL VARIABLE THO
    endProgram = end_program;

    if(!currentFileArena || !currentFileArena->file || !currentFileArena->arena ){
        logger("[f_triggerClose]: No opened files, proceed to close app directly.");
        return;
    }

    if(currentFileArena->file->isModified == true){
        dw_writeBuffer(textmemptr, "File modified, save? Y/N ",0,VIDEO_ROWS - 1 ,26, VIDEO_ROWS - 1, settings.STATUSBAR_COLOR_TEXT, settings.STATUSBAR_COLOR_BG);
        
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
            dw_writeBuffer(textmemptr, "File name: ",0,VIDEO_ROWS - 1, 10,VIDEO_ROWS - 1, settings.STATUSBAR_COLOR_TEXT, settings.STATUSBAR_COLOR_BG);
            
            while((len <= 3 || len > 12)){
                dw_writeBuffer(textmemptr, "",11,VIDEO_ROWS - 1, VIDEO_COLS - 1, VIDEO_ROWS - 1, settings.STATUSBAR_COLOR_TEXT, settings.STATUSBAR_COLOR_BG);
                filename = ed_scanf(11, VIDEO_ROWS - 1, 32);
                
                if(filename == NULL) return;

                len = strlen(filename);

                ed_renderElements();
                if(len <= 3 || len > 12){
                    dw_writeBuffer(textmemptr, "Invalid filename! Try again",0,VIDEO_ROWS - 1,30, VIDEO_ROWS - 1, settings.STATUSBAR_COLOR_TEXT, settings.STATUSBAR_COLOR_BG);
                }
            }
            if(esc == true) return;
            strcpy(currentFileArena->file->name, filename);
        }
    }

    // Close the file
    f_saveFile();
    f_closeCurrentFile();
}

void f_closeCurrentFile(){
    int i = 0;
    char oldFileName[255];

    memset(oldFileName, '\0', 255);

    // If there are no current file opened, we fallback
    if(!currentFileArena) return;

    strcpy(oldFileName, currentFileArena->file->name);

    f_closeFile(currentFileArena);    
    f_flushSearchMetadata();

    // We find the next opened file
    i = 0;

    while(i < MAX_ARENAS && fileList[i].file == NULL && fileList[i].arena == NULL){
        i++;
    }

    if(i == MAX_ARENAS){
        currentFileArena = NULL;
    }else{
        currentFileArena = &fileList[i];
        currentFileSearch = &fileListSearchMetadata[i];
    }

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
    if(!currentFileSearch) return;

    mem_arena_free(currentFileSearch->arena, NULL);
    currentFileSearch->arena = NULL; // Ensure pointer is cleared

    //currentFileSearch->dialogInputIndex = 0;
    //memset(currentFileSearch->dialogInputBuffer, '\0', 255);

    currentFileSearch->wordCount = 0;
    currentFileSearch->words = NULL;
    currentFileSearch->currentWordNode = NULL;
}

void f_allocSearchMetadata(){
    if(currentFileSearch) return;
    if( !currentFileArena ||
        !currentFileArena->file ||
        !currentFileArena->file->name
    ){
        logger("[f_allocSearchMetadat]: No valid currentFile data");
        return;
    }
    
    mem_arena_init(currentFileSearch->arena , currentFileArena->file->name, MEM_ARENA_METADATA, MEM_ARENA_2K);
    
    currentFileSearch->dialogInputIndex = 0;
    memset(currentFileSearch->dialogInputBuffer, '\0', 255);

    currentFileSearch->wordCount = 0;
    currentFileSearch->words = NULL;
    currentFileSearch->currentWordNode = NULL;
}

void f_showFileSwitcher(){
    int i;
    int selectedIndex = 0;
    bool selected = false;
    FileArena *fileptr;

    if (!currentFileArena || !currentFileArena->file) {
        return;
    }
    
    hal_inp_clearKeyboardBuffer();
    
    /* Encontrar el índice seleccionado inicial antes del bucle */
    for(i = 0; i < MAX_ARENAS; i++) {
        if(fileList[i].file != NULL && 
           strcmp(fileList[i].file->name, currentFileArena->file->name) == 0) {
            selectedIndex = i;
            break;
        }
    }

    while(hal_inp_isKeyDown(HAL_KEY_LALT)){
        /* 1. Dibujar el cuadro */
        dw_rectangle(textmemptr, 4, 4, 34, 16, COLOR_RED, COLOR_WHITE, ' ', COLOR_WHITE, COLOR_RED, false, DRAW_BORDER_SIMPLE, NULL);

        /* 2. Dibujar la lista de archivos */
        for(i = 0; i < MAX_ARENAS; i++){
            fileptr = &fileList[i];
            if (fileptr->file != NULL) {
                selected = (i == selectedIndex);
                dw_writeBuffer(textmemptr, "%s %s", 5, 5 + i, 33, 5 + i, 
                               COLOR_WHITE, COLOR_RED, (selected ? "*" : " "), fileptr->file->name);
            }
        }

        /* 3. Detectar pulsación de borde de LSHIFT dentro del bucle */
        if(hal_inp_keysPressed(HAL_INP_TRIGGER_EDGE, 2, HAL_KEY_LALT, HAL_KEY_LSHIFT)){
            /* Avanzar al siguiente archivo abierto válido */
            do {
                selectedIndex = (selectedIndex + 1) % MAX_ARENAS;
            } while(fileList[selectedIndex].file == NULL);

            currentFileArena = &fileList[selectedIndex];
        }

        /* 4. Actualizar el buffer del teclado manualmente en este bucle */
        hal_inp_updateKeyboard();
        
        /* En DOS es bueno dar un brevísimo retardo/yield aquí para no saturar la CPU */
        hal_sleep_ms(10); /* O similar */
    }

    ed_renderEvent = true;
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

    dw_rectangle(textmemptr, vis_offset, dialogStartY, VIDEO_COLS - vis_offset, dialogEndY, COLOR_BLUE, COLOR_WHITE, ' ', COLOR_WHITE, COLOR_BLUE, false, DRAW_BORDER_SIMPLE, "OPEN FILE");

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

    if (currentFileArena && currentFileArena->file){
        _updateCursor();
    }

    ed_renderEvent = true;
}
