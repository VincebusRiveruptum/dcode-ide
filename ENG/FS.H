#ifndef ENG_FS_H
#define ENG_FS_H

//#include "..\APP\MAIN.H"

#include <stdio.h>
#include <stdlib.h>

#if defined(__MSDOS__) || defined(__WATCOMC__)
#include <dos.h>

#elif defined(__linux__)
#include <dirent.h>
#include <unistd.h>
#endif

typedef struct FileEntry{
    char name[256];
    unsigned char isDirectory;
    long size;
}FileEntry;

extern char currentWorkspacePath[255];

#endif