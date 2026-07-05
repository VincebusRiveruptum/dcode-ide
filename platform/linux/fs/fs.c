#include "../../../hal/hal_fs.h"
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

char currentWorkspacePath[255] = {'\0'};

size_t hal_fs_getFilePath(char *filename) {
    char *last_slash;

    if (!filename) return 0;

    last_slash = strrchr(filename, '/');
    if (!last_slash) {
        last_slash = strrchr(filename, '\\');
    }
    if (!last_slash) return 0;

    return (size_t)(last_slash - filename);
}

char *hal_fs_getAbsoluteCurrentPath(char *strbuffer, size_t len) {
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

Directory *hal_fs_getDirectoryFileList(const char *path) {
    Directory *d;
    DIR *dir;
    struct dirent *entry;
    struct stat statbuf;
    char fullPath[512];
    const char *dir_to_open;

    d = (Directory*) malloc(sizeof(Directory));
    if (!d) return NULL;

    d->fileEntries = (List*) malloc(sizeof(List));
    if (!d->fileEntries) {
        free(d);
        return NULL;
    }
    memset(d->fileEntries, 0, sizeof(List));

    dir_to_open = (strlen(path) == 0) ? "." : path;
    dir = opendir(dir_to_open);
    if (!dir) {
        return d;
    }

    while ((entry = readdir(dir)) != NULL) {
        FileEntry *foundFileEntry = (FileEntry*) malloc(sizeof(FileEntry));
        if (!foundFileEntry) {
            closedir(dir);
            hal_fs_freeDirectory(d);
            return NULL;
        }
        memset(foundFileEntry->name, '\0', sizeof(foundFileEntry->name));
        snprintf(foundFileEntry->name, sizeof(foundFileEntry->name), "%s", entry->d_name);

        sprintf(fullPath, "%s/%s", dir_to_open, entry->d_name);
        if (stat(fullPath, &statbuf) == 0) {
            foundFileEntry->isDirectory = S_ISDIR(statbuf.st_mode) ? 1 : 0;
            foundFileEntry->size = statbuf.st_size;
        } else {
            foundFileEntry->isDirectory = 0;
            foundFileEntry->size = 0;
        }

        addGenericNode(&d->fileEntries, foundFileEntry, NULL);
    }
    closedir(dir);

    return d;
}

char *fs_getFileExtension(char *filename){
    return strrchr(filename, '.');
}

size_t fs_getFileName(char *filename){
    size_t length = 0;
    size_t slashPos = 0;
    char pathSep = FS_PATH_SEPARATOR[0];
    
    while(filename[length] != '\0'){
        if(filename[length] == pathSep){
            slashPos = length;
        }
        length++;
    }

    if(slashPos == 0) return 0;

    return slashPos + 1;
}
