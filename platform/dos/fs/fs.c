#include "fs.h"

char currentWorkspacePath[255] = {'\0'};

size_t hal_fs_getFilePath(char *filename){
    char *last_slash;

    if(!filename) return 0;

    last_slash = strrchr(filename, '\\');
    if(!last_slash) return 0;

    return (size_t)(last_slash - filename);
}

char *hal_fs_getAbsoluteCurrentPath(char *strbuffer, size_t len){
    return getcwd(strbuffer, len);
}

void hal_fs_freeDirectory(Directory *d) {
    Node *current, *temp;
    if (!d) return;
    if (d->fileEntries) {
        current = d->fileEntries->firstNode;
        while (current != NULL) {
            temp = current;
            current = current->next;
            if (temp->data) free(temp->data);
            free(temp);
        }
        free(d->fileEntries);
    }
    free(d);
}

Directory *hal_fs_getDirectoryFileList(const char *path){
    unsigned int rc;
    struct find_t foundFile;
    FileEntry *foundFileEntry;
    Directory *d;
    char searchPattern[512] = {'\0'};
    size_t len = strlen(path);

    d = (Directory*) malloc(sizeof(Directory));
    
    logger("[hal_fs_getDirectoryFileList] Retrieving directory data from : %s", path);
    
    if(!d){
        logger("[hal_fs_getDirectoryFileList] Could not allocate memory for buffer directory list");
        return NULL;
    } 

    d->fileEntries = (List*)malloc(sizeof(List));
    memset(d->fileEntries, 0, sizeof(List));

    if(!d->fileEntries){
        logger("[hal_fs_getDirectoryFileList] Could not allocate memory for buffer directory file list");
        return NULL;
    } 

    if (len == 0) {
        strcpy(searchPattern, "*.*");
    } else if (path[len - 1] == '\\' || path[len - 1] == '/') {
        sprintf(searchPattern, "%s*.*", path);
    } else {
        sprintf(searchPattern, "%s\\*.*", path);
    }

    rc = _dos_findfirst(searchPattern, _A_NORMAL | _A_SUBDIR, &foundFile);

    do{
        foundFileEntry = (FileEntry*)malloc(sizeof(FileEntry));
        
        if(!foundFileEntry){
            logger("[hal_fs_getDirectoryFileList]: Fail to alloc found file entry");
            hal_fs_freeDirectory(d);
            return NULL;
        }

        memset(foundFileEntry->name, '\0', sizeof(foundFileEntry->name));
        strncpy(foundFileEntry->name, foundFile.name, sizeof(foundFileEntry->name) - 1);

        foundFileEntry->isDirectory = (foundFile.attrib & _A_SUBDIR) ? 1 : 0;
        foundFileEntry->size = foundFile.size;

        addGenericNode(&d->fileEntries, foundFileEntry, NULL);
    }while((rc == 0) && _dos_findnext(&foundFile) == 0 );

    return d;
}

// We get where the file extension starts
char *fs_getFileExtension(char *filename){
    size_t i=strlen(filename);
    
    while(i > 0 && filename[i] != '.'){ // Find the last dot
        i--;
    }

    return strrchr(filename, '.');
}

// We get where the file name starts
size_t fs_getFileName(char *filename){
    size_t length=0;
    size_t slashPos=0;
    
    while(filename[length] != '\0'){ // Find the last dot
        if(filename[length] == FS_PATH_SEPARATOR[0] ){ // Count the number of slashes
            slashPos = length;
        }
        length++;
    }

    if(slashPos == 0) return 0;

    return slashPos + 1;
}