#include "FS.H"

char currentWorkspacePath[255] = {'\0'};

/* TEST FUNCTIONS */
#if defined(__MSDOS__) || defined(__WATCOMC__)
char *fs_getAbsoluteCurrentPath(){
    return getcwd(NULL, 255);
}

#endif
/* TEST FUNCTIONS */
#if defined(__MSDOS__) || defined(__WATCOMC__)
char *fs_getDirectoryContents(){
    return getcwd(NULL, 255);
}

#endif

#if defined(__MSDOS__) || defined(__WATCOMC__)
void fs_printContents(const char *path){
    struct find_t archivo;
    unsigned int rc;
    char fullpath[255] = {'\0'};

    /* _dos_findfirst retorna 0 si encuentra correspondencia */
    /* Usamos _A_NORMAL | _A_SUBDIR para listar archivos y carpetas */
    rc = _dos_findfirst("*.*", _A_NORMAL | _A_SUBDIR, &archivo);

    while((rc == 0) && _dos_findnext(&archivo) == 0 ){
        printf(" - %s (Tamano: %ld bytes)\n", archivo.name, archivo.size);            
    }

    printf("%s\\", getcwd(&fullpath,255));

    return;
}

#elif defined(__linux__)
void fs_printContents(const char *path){
    DIR *dirp = opendir(path);
    struct dirent *entryptr;
    char *fullpath;
    if(!dirp){
        printf("There was an error trying to open the selected path.");
        return;
    }

    while(dirp){
        entryptr = readdir(dirp);

        if(entryptr){
            printf("\n%s", entryptr->d_name);
        }
    }

    printf("\n%s\\", getcwd(fullpath,255));

    closedir(dirp);
}
#endif

/* ============== */

/* We obtain a linked list of the files of a certain path*/
Directory *fs_getDirectoryFileList(const char *path){
    unsigned int rc;
    struct find_t foundFile;
    FileEntry *foundFileEntry;
    Directory *d;

    d = (Directory*) malloc(sizeof(Directory));

    if(!d){
        logger("[fs_getDirectoryFileList] Could not allocate memory for buffer directory list");
        return NULL;
    } 

    d->fileEntries = (List*)malloc(sizeof(List));
    memset(d->fileEntries, 0, sizeof(d->fileEntries));


    if(!d->fileEntries){
        logger("[fs_getDirectoryFileList] Could not allocate memory for buffer directory file list");
        return NULL;
    } 

    rc = _dos_findfirst("*.*", _A_NORMAL | _A_SUBDIR, &foundFile);

    do{
        // Packaging the found file to a file entry
        foundFileEntry = (FileEntry*)malloc(sizeof(FileEntry));
        
        if(!foundFileEntry){
            logger("[fs_getDirectoryFileList]: Fail to alloc found file entry");
            free(d);
            return NULL;
        }

        memset(foundFileEntry->name, '\0', 255);
        strncpy(foundFileEntry->name, foundFile.name, strlen(foundFile.name));

        foundFileEntry->isDirectory = (foundFile.attrib & _A_SUBDIR) ? true : false;
        foundFileEntry->size = foundFile.size;

        // Then we add the created file entry to the list
        addGenericNode(&d->fileEntries, foundFileEntry, NULL, NULL );
    }while((rc == 0) && _dos_findnext(&foundFile) == 0 );

    return d;
}

#ifdef STANDALONE

int main(){
    printf("Showing contents of current PATH.");
    fs_printContents(".");
    return 0;
}

#endif