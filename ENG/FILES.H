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

typedef struct FileList{
    List *files;
    unsigned short length;
} FileList;


/* Globals ==============================================================*/

extern FileList *fileList;

/* Protypes =============================================================*/

void f_dumpToFile(char *buffer, char *filename);

#endif