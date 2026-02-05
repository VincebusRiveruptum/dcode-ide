
#include "FILES.H"

FileArena fileList[MAX_ARENAS];
MemoryArena *tmpPtrArena = NULL;
FileArena *currentFileArena = NULL;

void f_splitIntoLines(File *file, MemoryArena *arena) {
    char *start;
    char *end;    
    char *p;
    size_t lineLen;
    Line *line;

    start = file->buffer;
    end = file->buffer + file->bufferLength;
    p = start;

    file->lines = (List *)mem_arena_alloc(arena, NULL, sizeof(List));
    memset(file->lines, 0, sizeof(List));

    while (p < end) {
        if (*p == '\n') {
            lineLen = p - start;
            /* Strip trailing \r if present */
            if (lineLen > 0 && *(p - 1) == '\r') {
                lineLen--;
            }
            line = (Line *)mem_arena_alloc(arena, NULL, sizeof(Line));
            line->buffer = (char *)mem_arena_alloc(arena, NULL, lineLen + 1);
            memcpy(line->buffer, start, lineLen);
            line->buffer[lineLen] = '\0';
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
        line->buffer = (char *)mem_arena_alloc(arena, NULL, lineLen + 1);
        memcpy(line->buffer, start, lineLen);
        line->buffer[lineLen] = '\0';
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
    FILE *fp = fopen(filename, "w");
    int i=0;
    if(fp == NULL){
        logger("\n[f_dumpBufferTofile]: Error: Could not open file %s", filename);
        return;
    }
    
    for(i=0; i < bufferLength; i++){
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

// We get where the file extension starts
size_t f_getFileExtension(char *filename){
    size_t i=strlen(filename);
    
    while(i > 0 && filename[i] != '.'){ // Find the last dot
        i--;
    }

    return i;
}


bool f_openFile(char *filename){
    FILE *fp = fopen(filename, "r");
    File *file = NULL;
    MemoryArena *arena = NULL;
    FileArena *fileArena = NULL;
    char *shortFileName = NULL;
    
    // We are creating an arena per file
    if(fp == NULL){
        logger("\n[f_openFile]: Error: Could not open file %s", filename);
        return false;
    }

    /* We prepare the File arena */
    arena = mem_create_arena(filename + f_getFileName(filename), MEM_ARENA_FILE, MEM_ARENA_256K);

    fileArena = (FileArena *)mem_arena_alloc(arena, NULL ,sizeof(FileArena));
    fileArena->arena = arena;
    // We don't have the File struct yet, so we can't assign it to fileArena->file
    // fileArena->file = ...; 

    // We prepare the File struct 
    file = (File *)mem_arena_alloc(arena, NULL ,sizeof(File));
    
    file->name = (char*)mem_arena_alloc(arena, NULL, sizeof(char) * (strlen(filename) + 1));
    sprintf(file->name, "%s", filename);

    file->lines = NULL;
    file->scrollY = 0;
    file->scrollX = 0;
    file->cursorLine = 0;
    file->cursorCol = 0;
    
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
    
    file->buffer = (char *)mem_arena_alloc(arena, NULL, sizeof(char) * (file->bufferLength));
    
    memset(file->buffer, '\0', sizeof(char) * file->bufferLength);

    logger("\n[f_openFile]: File buffer size %d", file->bufferLength);
    
    if(!file->buffer){
        logger("\n[f_openFile]: Error: Could not allocate memory for file buffer");
        f_closeFile(fileArena);
        fclose(fp);
        return false;
    }
    // Actually reading the file
    fread(file->buffer, sizeof(char), file->bufferLength, fp);
    
    if(!file->buffer){
        logger("\n[f_openFile]: Error: Could not allocate memory for file buffer");
        
        fclose(fp);
        return false; 
    }

    // So the after opening hte file, it becomes the current file active
    currentFileArena = f_addFileArena(fileArena);

    f_splitIntoLines(file, arena);
    
    fclose(fp);
    return true;
}


size_t _copyLines(FileArena *old, FileArena *new){
    Node *currentNode;
    Line *oldLine, *newLine;
    size_t offset = 0;
    size_t lengthSum = 0;

    new->file->lines = NULL;
    
    currentNode = getNodeByIndex(&old->file->lines, 1);

    while(currentNode != NULL){
        oldLine = (Line *)currentNode->data;
        
        newLine = (Line *)mem_arena_alloc(new->arena, NULL, sizeof(Line));
        newLine->buffer = (char *)mem_arena_alloc(new->arena, NULL, sizeof(char) * oldLine->length + 1);
        memset(newLine->buffer, '\0', sizeof(char) * oldLine->length + 1);
        sprintf(newLine->buffer, "%s", oldLine->buffer);
        newLine->length = strlen(newLine->buffer);

        addGenericNode(&new->file->lines, (void *)newLine, NULL, new->arena);
        
        lengthSum += newLine->length + 1;
        currentNode = currentNode->next;
    }
    return lengthSum;
}

void f_saveFile(){
    // This will not use an arena for now
    Line *line;
    MemoryArena *oldArena, *newArena;
    Node *currentNode;
    FileArena *oldFileArena, *newFileArena;
    char *newArenaName = "NEW";
    size_t offset = 0;
    size_t lengthSum = 0;
    
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
    newArena = mem_create_arena(newArenaName, oldArena->type, MEM_ARENA_256K);
    
    newFileArena = (FileArena *)mem_arena_alloc(newArena, NULL, sizeof(FileArena));
    newFileArena->arena = newArena;
    newFileArena->file = (File *)mem_arena_alloc(newArena, NULL, sizeof(File));
    newFileArena->file->name = (char*)mem_arena_alloc(newArena, NULL, sizeof(char) * (strlen(oldFileArena->file->name) + 1));
    sprintf(newFileArena->file->name, "%s", oldFileArena->file->name);
    
    // We are going to travel the old file lines and copy them to the new file buffer
    currentNode = getNodeByIndex(&oldFileArena->file->lines, 0);
    lengthSum = _copyLines(oldFileArena, newFileArena);

    newFileArena->file->buffer = (char*)mem_arena_alloc(newArena, NULL, sizeof(char) * (lengthSum + 1));
    memset(newFileArena->file->buffer, '\0', sizeof(char) * (lengthSum + 1));

    if(currentNode == NULL){
        logger("\n[f_saveFile]: Error: No lines found");
        return;
    }
    
    // We are going to dump all lines into the file
    while(currentNode != NULL){
        line = (Line *)currentNode->data;
        
        memcpy(newFileArena->file->buffer + offset, line->buffer, line->length);
        
        offset += line->length;         // Avoiding null terminator
        newFileArena->file->buffer[offset] = '\n';   // Replacing null terminator with newline
        offset++;

        currentNode = currentNode->next;
    }

    newFileArena->file->bufferLength = offset;

    // We dump the new file buffer to the file
    f_dumpBufferTofile(newFileArena->file->buffer, newFileArena->file->bufferLength, newFileArena->file->name);

    // We close the old file
    f_closeFile(oldFileArena);
    
    // We add the new file arena and set it as the current file arena
    currentFileArena = f_addFileArena(newFileArena);

    newFileArena->file->isModified = false;
    sprintf(newFileArena->arena->name, "%s", newFileArena->file->name + f_getFileName(newFileArena->file->name));

    logger("[f_saveFile]: File %s saved successfully", newFileArena->file->name);
}

void f_triggerClose(){
    char input;
    if(currentFileArena->file->isModified == true){
        dw_writeBuffer(textmemptr, "File has been modified, save before closing?", 0, 2, 64, 0, COLOR_WHITE, COLOR_BLACK);   

        while(true){
            input = inPortb(0x60);
            
            if(input == 'y' || input == 'Y'){
                
                if(currentFileArena->file->name  == NULL || currentFileArena->file->name[0] == '\0'){
                    dw_writeBuffer(textmemptr, "File has no name, save as?", 0, 3, 32, 0, COLOR_WHITE, COLOR_BLACK);    
                    // We need to implement a simple input for setting the name of the file
                    // however i'm against the time so the name will be fixed for now
                    
                    f_saveFile();
                } else {
                    f_closeFile(currentFileArena);
                }
                break;
            } else if(input == 'n' || input == 'N'){
                f_closeFile(currentFileArena);
                break;
            }
        }
    }else{
        dw_writeBuffer(textmemptr, "File has no been modified, closing file", 0, 2, 64, 0, COLOR_WHITE, COLOR_BLACK);   
        logger("[f_triggerClose]: File has no been modified, closing file");
        f_closeFile(currentFileArena);
    }
    // TODO: If there are more files open, we need to switch to the next available file already open

    dw_fill(textmemptr, COLOR_BLUE, COLOR_LIGHT_BLUE, '°');
    ed_renderElements();
    
}


/* File list handling (File arena arrays) ===============================================*/

void f_initFileArenas(){
    int i=0;
    for(i; i < MAX_ARENAS; i++){
        fileList[i].file = NULL;
        fileList[i].arena = NULL;
    }
}

FileArena *f_getFileArena(char *filename){
    unsigned short i=0;
    
    for(i; i < MAX_ARENAS; i++){
        if(fileList[i].file && !strcmp(fileList[i].file->name, filename)) return &fileList[i];
    }   
    return NULL;
}
FileArena *f_addFileArena(FileArena *fileArena){
    unsigned short i=0;
    
    for(i; i < MAX_ARENAS; i++){
        if(fileList[i].file == NULL && fileList[i].arena == NULL){
            fileList[i] = *fileArena;
            return &fileList[i];
        }
    }
    return NULL;
}

void f_closeFile(FileArena *fileArena){
    char arenaName[64];
    sprintf(arenaName, "%s", fileArena->arena->name);

    fileArena->file = NULL;
    fileArena->arena = NULL;

    mem_arena_free(arenaName);

    logger("[f_closeFile]: File %s closed successfully", arenaName);    
}

void f_init(){
    f_initFileArenas();
}