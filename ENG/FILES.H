#ifndef APP_FILES_H
#define APP_FILES_H

#include "STD.H"
#include "..\APP\MAIN.H"
#include "..\DATA\DATA.H"

/* Const. ================================================================*/

#define MAX_FILE_LINE_LENGTH 256

/* Types =================================================================*/

typedef struct Line {
    char *buffer;
    size_t length;
} Line;

typedef struct File {
    char *name;
    char *path;
    char *extension;

    char *buffer;

    size_t bufferLength;  /* also known as file size */
    
    /* New line-based fields */
    List *lines;
    unsigned short scrollY;
    unsigned short scrollX;

    unsigned short cursorLine;
    unsigned short cursorCol;

    bool isModified;
    bool isActive;
} File;

typedef struct FileArena {
    struct File *file;
    MemoryArena *arena;
} FileArena;


/* Globals ==============================================================*/

extern FileArena fileList[MAX_ARENAS];

extern FileArena *currentFileArena;

/* Protypes =============================================================*/

void f_dumpToFile(char *filename);
void f_bufferDumpToFile(char *buffer, size_t bufferLength, char *filename);

void _initFileArenas();
FileArena *_getFileArena(char *filename);
FileArena *_addFileArena(FileArena *fileArena);
void _closeFile(FileArena *fileArena);
char *_getFileName(MemoryArena *arena, char *filename);
char *_getFileExtension(MemoryArena *arena, char *filename);
char *_getPath(MemoryArena *arena, char *filename);

#endif