#include "FS.H"

char currentWorkspacePath[255] = {'\0'};

/* TEST FUNCTIONS */

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

#ifdef STANDALONE

int main(){
    printf("Showing contents of current PATH.");
    fs_printContents(".");
    return 0;
}

#endif