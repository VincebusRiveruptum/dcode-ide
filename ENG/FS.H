#ifndef ENG_FS_H
#define ENG_FS_H

//#include "..\APP\MAIN.H"

#include <stdio.h>
#include <stdlib.h>

#include "..\DEPS\DATA\DATA.H"
#include "..\DEPS\LOG\LOG.H"
#include ".\FILES.H"

#if defined(__MSDOS__) || defined(__WATCOMC__)
#include <dos.h>
#include <direct.h>

#elif defined(__linux__)
#include <dirent.h>
#include <unistd.h>
#endif

typedef struct FileEntry{
    char name[256];
    unsigned char isDirectory;
    long size;
}FileEntry;

typedef struct Directory{
    List *fileEntries;
}Directory;

extern char currentWorkspacePath[255];
Directory *fs_getDirectoryFileList(const char *path);
void fs_freeDirectory(Directory *d);
#endif