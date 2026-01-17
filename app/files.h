#ifndef APP_FILES_H
#define APP_FILES_H

#include "STD.H"
#include "..\DEPS\DATA.H"

/* Const. ================================================================*/
/* Types =================================================================*/

typedef struct File {
    char *name;
    char *path;
    char *content;
    int size;
} File;

typedef struct FileList{
    List *files;
    unsigned short length;
} FileList;

/* Globals ==============================================================*/

extern FileList *fileTree;

/* Protypes =============================================================*/

bool openFolder(char *path);
bool openFile(char *fileName);
bool saveFile();

#endif