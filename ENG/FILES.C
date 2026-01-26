
#include "FILES.H"

FileArena fileList[MAX_ARENAS];
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
    char * file = (char *)mem_arena_alloc(NULL, filename, 10);
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
    File *file = NULL;
    MemoryArena *arena = NULL;
    FileArena *fileArena = NULL;
    
    // We are creating an arena per file
    if(fp == NULL){
        logger("\n[f_openFile]: Error: Could not open file %s", filename);
        return;
    }

    // We prepare the File arena
    arena = mem_create_arena(filename, MEM_ARENA_FILE, MEM_ARENA_16K);

    fileArena = (FileArena *)mem_arena_alloc(arena, NULL ,sizeof(FileArena));
    fileArena->arena = arena;
    // We don't have the File struct yet, so we can't assign it to fileArena->file
    // fileArena->file = ...; 

    _addFileArena(fileArena);
    
    // We prepare the File struct 
    file = (File *)mem_arena_alloc(arena, NULL ,sizeof(File));

    file->name = _getFileName(filename);
    file->path = _getPath(filename);
    file->extension = _getFileExtension(filename);

    if(!file->name || !file->path || !file->extension){
        logger("\n[f_openFile]: Error: Could not allocate memory for file details");
        _closeFile(fileArena);
        fclose(fp);
        return;
    }

    // Assign the file struct to the file arena
    fileArena->file = file;

    // Length of the file
    fseek(fp, 0, SEEK_END);
    
    file->bufferLength = ftell(fp);
    
    logger("\n[f_openFile]: File %s opened successfully, %d bytes", filename, file->bufferLength);
    
    rewind(fp); // or fseek(fp, 0, SEEK_SET);
    
    file->buffer = (char *)mem_arena_alloc(arena, NULL, sizeof(char) * (file->bufferLength + 1));

    logger("\n[f_openFile]: File buffer size %d", file->bufferLength);

    if(!file->buffer){
        logger("\n[f_openFile]: Error: Could not allocate memory for file buffer");
        _closeFile(fileArena);
        fclose(fp);
        return;
    }
    // Actually reading the file
    fread(file->buffer, sizeof(char), file->bufferLength, fp);
    file->buffer[file->bufferLength] = '\0';
    
    if(!file->buffer){
        logger("\n[f_openFile]: Error: Could not allocate memory for file buffer");
        
        fclose(fp);
        return;
    }
    
    fclose(fp);
    
    // Just for test purposes
    //f_bufferDumpToFile(file->buffer, file->bufferLength, "btest.txt");
    // logger("\n[f_openFile]: %s", file->buffer);
}


/* File list handling (File arena arrays) ===============================================*/

void _initFileArenas(){
    int i=0;
    for(i; i < MAX_ARENAS; i++){
        fileList[i].file = NULL;
        fileList[i].arena = NULL;
    }
}

FileArena *_getFileArena(char *filename){
    unsigned short i=0;
    
    for(i; i < MAX_ARENAS; i++){
        if(fileList[i].file && !strcmp(fileList[i].file->name, filename)) return &fileList[i];
    }   
    return NULL;
}
FileArena *_addFileArena(FileArena *fileArena){
    unsigned short i=0;
    
    for(i; i < MAX_ARENAS; i++){
        if(fileList[i].file == NULL && fileList[i].arena == NULL){
            fileList[i] = *fileArena;
            return &fileList[i];
        }
    }
    return NULL;
}

void _closeFile(FileArena *fileArena){
    mem_arena_free(fileArena->arena);
    fileArena->file = NULL;
    fileArena->arena = NULL;
}

void f_init(){
    _initFileArenas();
}