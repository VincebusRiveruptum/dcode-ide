#ifndef HAL_FS_H
#define HAL_FS_H

#include "STD.H"
#include "..\DEPS\DATA\DATA.H"

typedef struct FileEntry{
    char name[256];
    unsigned char isDirectory;
    long size;
} FileEntry;

typedef struct Directory{
    List *fileEntries;
} Directory;

extern char currentWorkspacePath[255];

size_t hal_fs_getFilePath(char *filename);
char *hal_fs_getAbsoluteCurrentPath(char *strbuffer, size_t len);
Directory *hal_fs_getDirectoryFileList(const char *path);
void hal_fs_freeDirectory(Directory *d);

#endif
