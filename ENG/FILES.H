#ifndef APP_FILES_H
#define APP_FILES_H

#include "STD.H"
#include "..\APP\MAIN.H"

/* Const. ================================================================*/

#define MAX_FILE_LINE_LENGTH 256

/* Types =================================================================*/

typedef struct File {
    char *name;
    char *path;
    char *extension;

    char *buffer;
    unsigned short bufferLength;  // also known as file size

    bool isModified;
    bool isActive;
} File;

typedef struct FileArena {
    struct File *file;
    MemoryArena *arena;
} FileArena;


/* Globals ==============================================================*/

extern FileArena fileList[MAX_ARENAS];

/* Protypes =============================================================*/

void f_dumpToFile(char *filename);
void f_bufferDumpToFile(char *buffer, size_t bufferLength, char *filename);

void _initFileArenas();
FileArena *_getFileArena(char *filename);
FileArena *_addFileArena(FileArena *fileArena);
void _closeFile(FileArena *fileArena);
char *_getFileName(char *filename);
char *_getFileExtension(char *filename);
char *_getPath(char *filename);

#endif