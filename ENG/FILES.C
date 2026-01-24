
#include "FILES.H"

FileList *fileList = NULL;

void f_dumpToFile(char *buffer, char *filename){
    FILE *fp = fopen(filename, "w");
    int i=0;
    int j=0;
    if(fp == NULL){
        printf("Error opening file\n");
        return;
    }
    
    for(i=0; i < VIDEO_ROWS; i++){
        for(j=0; j < VIDEO_COLS; j++){
            fputc(buffer[i * VIDEO_COLS + j], fp);
        }
        fputc('\n', fp);
    }
    
    fclose(fp);   
}

char *_getFileName(char *filename){
    unsigned short length=0;
    unsigned short slashPos=0;
    char * file = (char *)mem_arena_alloc(fileArena, 9);
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
    char *extension = (char *)mem_arena_alloc(fileArena, 8);
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
    char *path = (char *)mem_arena_alloc(fileArena, 260);
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
    File *openedFile = (File *)mem_arena_alloc(fileArena,sizeof(File));
    Node *fileNode = NULL;
    unsigned short i=0;
    
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
    
    rewind(fp);
    
    openedFile->buffer = (char *)mem_arena_alloc(fileArena, openedFile->bufferLength + 1);
    
    if(!openedFile->buffer){
         logger("\n[f_openFile]: Error: Could not allocate memory for file buffer");
         fclose(fp);
         return;
    }
    
    while(!feof(fp)){
        openedFile->buffer[i] = fgetc(fp);
        i++;
    }

    openedFile->buffer[i] = '\0';
    
    fclose(fp);

    // We insert it to the file list
    fileNode = (Node *)mem_arena_alloc(fileArena,sizeof(Node));

    if(fileNode == NULL){
        logger("\n[f_openFile]: Error: Could not allocate memory for file node");
        return;
    }

    fileNode->next = NULL;
    fileNode->prev = NULL;
    fileNode->data = (void *)openedFile;

    if(fileList == NULL){
        fileList = (FileList *)mem_arena_alloc(fileArena,sizeof(FileList));
        fileList->files = NULL;
        fileList->length = 0;
    }

    addToList(fileList, fileNode);
}