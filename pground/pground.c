#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <dos.h>    
#include <mem.h>
#include <i86.h>
#include <errno.h>
#include <time.h>

size_t fs_getFilePath(char *filename){
    size_t filenameLen;
    size_t pathIndex = 0;
    char *filenameIndex = NULL;

    if(!filename) return 0;

    filenameLen = strlen(filename);
    filenameIndex = filename + filenameLen;

    while((*filenameIndex) != '\\' && filenameIndex > filename){
        filenameIndex--;
        pathIndex++;
    }

    return (filenameLen - pathIndex);
}

int main(){
    char *testPath = "C:\\THIS\\IS\\A\\TEST\\PATH.EXE";
    char onlyPath[255] = {'\0'};

    strncpy(&onlyPath, testPath, fs_getFilePath(testPath));

    printf("This is the path in question : %s", onlyPath);
    
    return 0;
}
