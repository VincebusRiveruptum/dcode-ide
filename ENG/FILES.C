
#include "FILES.H"

void f_dumpToFile(char *buffer, char *filename){
    FILE *fp = fopen(filename, "w");
    int i=0;
    int j=0;
    if(fp == NULL){
        printf("Error opening file\n");
        return;
    }
    
    for(i=0; i < VIDEO_ROWS; i++){
        for(j=0; j < VIDEO_COLS; j++){
            fputc(buffer[i * VIDEO_COLS + j], fp);
        }
        fputc('\n', fp);
    }
    
    fclose(fp);   
}
