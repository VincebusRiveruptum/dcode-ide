
#include "files.h"

int _checkAvailableName(void){
	int maxIndex = 0;
    int index = 0;
    char *fileName;
    char *match;
	
    Node *wNode = NULL;
    Node *fNode = NULL;

	Window *wnd = NULL;
	File *file = NULL;

    if (!currentWorkspace || !currentWorkspace->windowList) return 1;

    wNode = currentWorkspace->windowList->firstNode;
    
	// Window travel
	while(wNode){
        wnd = (Window *)wNode->data;
        
		if(wnd && wnd->fileList){
            fNode = wnd->fileList->firstNode;
            
			// File travel
			while(fNode){
				file = (File *)fNode->data;
                
				if(file && file->name){
                    fileName = file->name;
                    match = strstr(fileName, "newfile");

					// Check if there is a match
					// and if the match position 
					// is at the beginnig of the 
					// filename, so at the same 
					// pointer of filename.
                    if(
						match && 
						match == fileName 
					){
                        index = strtol(match + 7, NULL, 10);
                        if(index > maxIndex){
                            maxIndex = index;
                        }
                    }
                }
                fNode = fNode->next;
            }
        }
        wNode = wNode->next;
    }
    return maxIndex + 1;
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
    if (!file->lines) {
        logger("[_splitIntoLines]: Failed to allocate lines list");
        return;
    }
    memset(file->lines, 0, sizeof(List));

    file->deletedLines = (List *)mem_arena_alloc(file->arena, sizeof(List));
    if (!file->deletedLines) {
        logger("[_splitIntoLines]: Failed to allocate deletedLines list");
        return;
    }
    memset(file->deletedLines, 0, sizeof(List));

    while (p < end) {
        if (*p == '\n') {
            lineLen = p - start;
            /* Strip trailing \r if present */
            if (lineLen > 0 && *(p - 1) == '\r') {
                lineLen--;
            }
            if (lineLen >= MAX_FILE_LINE_LENGTH) {
                lineLen = MAX_FILE_LINE_LENGTH - 1;
            }
            line = (Line *)mem_arena_alloc(file->arena, sizeof(Line));
            if (line != NULL) {
                line->buffer = (char *)mem_arena_alloc(file->arena, MAX_FILE_LINE_LENGTH);
                if (line->buffer != NULL) {
                    memset(line->buffer, '\0', MAX_FILE_LINE_LENGTH);
                    memcpy(line->buffer, start, lineLen);
                    line->length = lineLen;
                    addGenericNode(&file->lines, line, file->arena);
                }
            }
            start = p + 1;
        }
        p++;
    }

    /* Handle last line or file without trailing \n */
    if (start <= end) {
        lineLen = end - start;
        if (lineLen > 0 && start[lineLen-1] == '\r') lineLen--;
        if (lineLen >= MAX_FILE_LINE_LENGTH) {
            lineLen = MAX_FILE_LINE_LENGTH - 1;
        }

        line = (Line *)mem_arena_alloc(file->arena, sizeof(Line));
        if (line != NULL) {
            line->buffer = (char *)mem_arena_alloc(file->arena, MAX_FILE_LINE_LENGTH);
            if (line->buffer != NULL) {
                memset(line->buffer, '\0', MAX_FILE_LINE_LENGTH);
                memcpy(line->buffer, start, lineLen);
                line->length = lineLen;
                addGenericNode(&file->lines, line, file->arena);
            }
        }
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

    if(*endptr == FS_PATH_SEPARATOR ){
        *endptr = '\0';
        endptr--;
    }

    while(endptr > path && *(endptr) != FS_PATH_SEPARATOR){
        *endptr = '\0';
        endptr--;
    };
    
    return (int)(endptr - path) + 1;
}

// We check if the filename is a default one (ie newfile1.c)
bool _isDefaultFileName(void){
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
