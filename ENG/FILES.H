#ifndef APP_FILES_H
#define APP_FILES_H

#include "STD.H"
#include "..\APP\MAIN.H"
#include "..\DATA\DATA.H"

/* Const. ================================================================*/

#define MAX_FILE_LINE_LENGTH 384
#define MAX_FILE_NAME 256

#define FILE_EXTENSION_TXT 0
#define FILE_EXTENSION_C 1
#define FILE_EXTENSION_PYTHON 2
#define FILE_EXTENSION_JS 3

/* Types =================================================================*/

typedef struct Line {
    char *buffer;
    size_t length;
} Line;

typedef struct File {
    char *name;         // full name and path
    char *buffer;
    unsigned char ext;  // EXTENSION ID

    size_t bufferLength;  /* also known as file size */
    
    /* New line-based fields */
    List *lines;
    List *deletedLines;

    // Could be useful in the future
    Node *currentLineNode;
    
    Line *prevLine;
    Line *currentLine;
    Line *nextLine;

    // Current line position chars
    char prevChar;
    char currentChar;
    char nextChar;

    // This will be used for the editor to know if the previous line to the first visible was a comment
    // So the comment formmating will still function when scrolling down
    
    unsigned short scrollY;
    unsigned short scrollX;

    // Indicates the current cursor line index
    unsigned short cursorLine;

    // Indicates the current cursor column index
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

extern bool endProgram;

/* Protypes =============================================================*/

void f_dumpToFile(char *filename);
void f_dumpBufferTofile(char *buffer, size_t bufferLength, char *filename);

void f_initFileArenas();
FileArena *f_getFileArena(char *filename);
FileArena *f_addFileArena(FileArena *fileArena);
void f_closeFile(FileArena *fileArena);

size_t f_getFileName(char *filename);
char *f_getFileExtension(char *filename);

void f_init();
void f_newFile();
bool f_openFile(char *filename);
void f_saveFile();
void f_triggerClose();
#endif
