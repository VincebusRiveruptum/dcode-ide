#ifndef APP_FILES_H
#define APP_FILES_H

#include "STD.H"
#include "..\APP\MAIN.H"

/* Const. ================================================================*/

#define MAX_FILE_CHUNK_SIZE 4096
#define MAX_FILE_CHUNKS 16
/* Types =================================================================*/

typedef struct File {
    char *name;
    char *path;
 
    struct FileExtension*extension;

    struct FileChunk *chunks[MAX_FILE_CHUNKS];
    unsigned short chunkLength;

    unsigned short size;
} File;

typedef struct FileList{
    List *files;
    unsigned short length;
} FileList;

typedef struct FileChunk {
    char *content;
    unsigned short offset;
    unsigned short size;
} FileChunk;

typedef struct FileExtension {
    char *name;
    char *description;
    char *icon;
} Extension;

/* Globals ==============================================================*/

extern FileList *fileTree;

/* Protypes =============================================================*/

void f_dumpToFile(char *buffer, char *filename);

#endif