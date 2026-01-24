
#include "FILES.H"

List *fileList = NULL;
MemoryArena *tmpPtrArena = NULL;

void f_dumpToFile(char *filename){
    FILE *fp = fopen(filename, "w");
    int i=0;
    int j=0;
    if(fp == NULL){
        printf("Error opening file\n");
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

void f_bufferDumpToFile(char *buffer, size_t bufferLength, char *filename){
    FILE *fp = fopen(filename, "w");
    int i=0;
    if(fp == NULL){
        printf("Error opening file\n");
        return;
    }
    
    for(i=0; i < bufferLength; i++){
        fputc(buffer[i], fp);
    }
    
    fclose(fp);   
}

char *_getFileName(char *filename){
    unsigned short length=0;
    unsigned short slashPos=0;
    char * file = (char *)mem_arena_alloc(NULL, filename, 9);
    if(!file) return NULL;
    
    memset(file, 0, 9);

    while(filename[length] != '\0'){ // Find the last dot
        if(filename[length] == '/' || filename[length] == '\\' ){ // Count the number of slashes
            slashPos = length;
        }
        length++;
    }
    
    strncpy(file, filename + slashPos, 9);
    file[9] = '\0';
    return file;
}
char *_getFileExtension(char *filename){
    unsigned short i=0;
    char *extension = (char *)mem_arena_alloc(NULL, filename, 8);
    if(!extension) return NULL;

    memset(extension, 0, 8);

    while(filename[i] != '.'){ // Find the last dot
        i++;
    }
    
    strncpy(extension, filename + i + 1, 8);
    extension[8] = '\0';
    return extension;
}

char *_getPath(char *filename){
    unsigned short length=0;
    unsigned short slashPos=0;
    char *path = (char *)mem_arena_alloc(NULL, filename, 260);
    if(!path) return NULL;
    
    memset(path, 0, 260);

    while(filename[length] != '\0'){ // Find the last dot
        if(filename[length] == '/' || filename[length] == '\\' ){ // Count the number of slashes
            slashPos = length;
        }
        length++;
    }
    
    strncpy(path, filename, length - slashPos);
    path[length - slashPos] = '\0';
    return path;
}

void f_openFile(char *filename){
    FILE *fp = fopen(filename, "r");
    File *openedFile;
    unsigned short i=0;
    char lineBuffer[MAX_FILE_LINE_LENGTH];
    unsigned short bufferOffset=0;
    MemoryArena *arena = NULL;
    
    // We are creating an arena per file
    arena = mem_create_arena(filename, MEM_ARENA_FILE, MEM_ARENA_16K);
    
    openedFile = (File *)mem_arena_alloc(arena, NULL ,sizeof(File));

    memset(lineBuffer, 0 , MAX_FILE_LINE_LENGTH);

    if(!openedFile){
        logger("\n[f_openFile]: Error: Could not allocate memory for file struct");
        fclose(fp);
        return;
    }
 
    
    if(fp == NULL){
        logger("\n[f_openFile]: Error: Could not open file %s", filename);
        return;
    }

    openedFile->name = _getFileName(filename);
    openedFile->path = _getPath(filename);
    openedFile->extension = _getFileExtension(filename);

    if(!openedFile->name || !openedFile->path || !openedFile->extension){
        logger("\n[f_openFile]: Error: Could not allocate memory for file details");
        fclose(fp);
        return;
    }

    while(!feof(fp)){
        fgetc(fp);
    }
    
    openedFile->bufferLength = ftell(fp);
    
    logger("\n[f_openFile]: File %s opened successfully, %d bytes", filename, openedFile->bufferLength);
    
    rewind(fp);
    
    openedFile->buffer = (char *)mem_arena_alloc(arena, NULL, openedFile->bufferLength + 200);
    
    if(!openedFile->buffer){
        logger("\n[f_openFile]: Error: Could not allocate memory for file buffer");
        fclose(fp);
        return;
    }
    
    fread(openedFile->buffer, sizeof(char), openedFile->bufferLength, fp);
    
    fclose(fp);
    
    // Just for test purposes
    f_bufferDumpToFile(openedFile->buffer, openedFile->bufferLength, "btest.txt");
}