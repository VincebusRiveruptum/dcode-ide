#include "FS.H"

char currentWorkspacePath[255] = {'\0'};

// Retruns the index until where the full filename string is 
// just the directory part only .
size_t fs_getFilePath(char *filename){
    char *last_slash;

    if(!filename) return 0;

    last_slash = strrchr(filename, '\\');
    if(!last_slash) return 0;

    return (size_t)(last_slash - filename);
}

/* TEST FUNCTIONS */
#if defined(__MSDOS__) || defined(__WATCOMC__)
char *fs_getAbsoluteCurrentPath(char *strbuffer, size_t len){
    return getcwd(strbuffer, len);
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

void fs_freeDirectory(Directory *d) {
    Node *current, *temp;
    if (!d) return;
    if (d->fileEntries) {
        current = d->fileEntries->firstNode;
        while (current != NULL) {
            temp = current;
            current = current->next;
            if (temp->data) free(temp->data);   /// free up FileEntry
            free(temp);                         // free node
        }
        free(d->fileEntries);
    }
    free(d);
}

/* We obtain a linked list of the files of a certain path*/
Directory *fs_getDirectoryFileList(const char *path){
    unsigned int rc;
    struct find_t foundFile;
    FileEntry *foundFileEntry;
    Directory *d;
    char searchPattern[512] = {'\0'};
    size_t len = strlen(path);

    d = (Directory*) malloc(sizeof(Directory));
    
    logger("[fs_getDirectoryFileList] Retrieving directory data from : %s", path);
    
    if(!d){
        logger("[fs_getDirectoryFileList] Could not allocate memory for buffer directory list");
        return NULL;
    } 

    d->fileEntries = (List*)malloc(sizeof(List));
    memset(d->fileEntries, 0, sizeof(List));


    if(!d->fileEntries){
        logger("[fs_getDirectoryFileList] Could not allocate memory for buffer directory file list");
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
        // Packaging the found file to a file entry
        foundFileEntry = (FileEntry*)malloc(sizeof(FileEntry));
        
        if(!foundFileEntry){
            logger("[fs_getDirectoryFileList]: Fail to alloc found file entry");
            fs_freeDirectory(d);
            return NULL;
        }

        memset(foundFileEntry->name, '\0', sizeof(foundFileEntry->name));
        strncpy(foundFileEntry->name, foundFile.name, sizeof(foundFileEntry->name) - 1);

        foundFileEntry->isDirectory = (foundFile.attrib & _A_SUBDIR) ? true : false;
        foundFileEntry->size = foundFile.size;

        // Then we add the created file entry to the list
        addGenericNode(&d->fileEntries, foundFileEntry, NULL, NULL );
    }while((rc == 0) && _dos_findnext(&foundFile) == 0 );

    return d;
}

#ifdef STANDALONE

int main(){
    char *testPath = "C:\\THIS\\IS\\A\\TEST\\PATH.EXE";
    char onlyPath[255] = {'\0'};

    printf("Showing contents of current PATH.");
    fs_printContents(".");

    strncpy(&onlyPath, testPath, fs_getFilePath(testPath));

    printf("This is the path in question : %s", onlyPath);
    
    return 0;
}

#endif