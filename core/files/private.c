
#include "files.h"

int _checkAvailableName(){
    // TRAVEL ALL WINDOWS
		// TRAVEL ALL FILES
		// Get name that match the 'newfile' name scheme
		// Add to a counter
	
	// Return counter...
    return index + 1;
}

void _splitIntoLines(char *buffer, size_t bufferLength, File *file) {
    char *start;
    char *end;    
    char *p;
    size_t lineLen;
    Line *line;

    start = buffer;
    end = buffer + bufferLength;
    p = start;

	if(!file || !file->arena){
		logger("[_splitIntoLines]: File has no arena");
		return;
	}

    file->lines = (List *)mem_arena_alloc(file->arena, sizeof(List));
    memset(file->lines, 0, sizeof(List));

    file->deletedLines = (List *)mem_arena_alloc(file->arena, sizeof(List));
    memset(file->deletedLines, 0, sizeof(List));

    while (p < end) {
        if (*p == '\n') {
            lineLen = p - start;
            /* Strip trailing \r if present */
            if (lineLen > 0 && *(p - 1) == '\r') {
                lineLen--;
            }
            line = (Line *)mem_arena_alloc(file->arena, sizeof(Line));
            line->buffer = (char *)mem_arena_alloc(file->arena, MAX_FILE_LINE_LENGTH);
            memset(line->buffer, '\0', MAX_FILE_LINE_LENGTH);
            memcpy(line->buffer, start, lineLen);
            line->length = lineLen;

            addGenericNode(&file->lines, line, file->arena);
            start = p + 1;
        }
        p++;
    }

    /* Handle last line or file without trailing \n */
    if (start <= end) {
        lineLen = end - start;
        if (lineLen > 0 && start[lineLen-1] == '\r') lineLen--;

        line = (Line *)mem_arena_alloc(file->arena, sizeof(Line));
        line->buffer = (char *)mem_arena_alloc(file->arena, MAX_FILE_LINE_LENGTH);
        memset(line->buffer, '\0', MAX_FILE_LINE_LENGTH);
        memcpy(line->buffer, start, lineLen);
        line->length = lineLen;
        addGenericNode(&file->lines, line, file->arena);
    }
}

size_t _copyLines(File *old, File *new){
    size_t lengthSum = 0;
    size_t lineLen = 0;
    Node *currentNode = NULL;
    Line *oldLine = NULL;
    Line *newLine = NULL;

    new->lines = NULL;
    
    currentNode = old->lines->firstNode;

    while(currentNode != NULL){
        oldLine = (Line *)currentNode->data;
        
        newLine = (Line *)mem_arena_alloc(new->arena, sizeof(Line));
        newLine->buffer = (char *)mem_arena_alloc(new->arena, sizeof(char) * MAX_FILE_LINE_LENGTH);
        memset(newLine->buffer, '\0', sizeof(char) * MAX_FILE_LINE_LENGTH);

        lineLen = strlen(oldLine->buffer);

        memcpy(newLine->buffer, oldLine->buffer, lineLen + 1);
        newLine->length = lineLen;

        addGenericNode(&new->lines, (void *)newLine, new->arena);
        
        lengthSum += newLine->length + 1;
        currentNode = currentNode->next;
    }
    return lengthSum;
}

int _goBackPath(char *path){
    int end, len;
    char *endptr;
    
    end = strlen(path) - 1;
    len = end + 1;

    if(len <= 3) return len;
    
    endptr = path + end;

    if(*endptr == '\\' ){
        *endptr = '\0';
        endptr--;
    }

    while(endptr > path && *(endptr) != '\\'){
        *endptr = '\0';
        endptr--;
    };
    
    return (int)(endptr - path) + 1;
}

// We check if the filename is a default one (ie newfile1.c)
bool _isDefaultFileName(){
    char filename[8] = {'\0'};
    bool res = false;
	File *currentFile = NULL;

	currentFile = currentWindow->currentFile;

    if(!currentFile){
        logger("[_isDefaultFileName]: No current file!");
        return true;
    }

    if(!currentFile->name){
        logger("[_isDefaultFileName]: current file has no name!");
        return true;
    }

    strncpy(filename, currentFile->name, 7);
    
    res = (strcmp(filename, "newfile") == 0) ? true : false;

    return res;
}
