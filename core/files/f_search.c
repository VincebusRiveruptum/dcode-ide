#include "files.h"

// ==== SEARCH BEHAVIOR ==================================

// * Each file arena has its own fileSearchMetadata, its not insidie the 
// File definition because this could cause memory usage issues when 
// there are many Word matches, so it separate for better memory 
// perfomance and control

// * The search metadata stores all matches in a pointer array. So,

// * Every time the file changes the searchMetadata of the currentFile
// MUST be flush, so there are no dangling pointers nor references to a word
// address that changed.

// * The fileListSearchMetadata INDEX is parallel to fileList currentFile
// this makes sure there are no collisions when flushing or filling the 
// metadata of an already opened file.

// The access to the current search meta data index is easy with 

void ed_prepareSearchTool(){
    if(
		hal_inp_keysPressed(
			HAL_INP_TRIGGER_EDGE, 
			2, 
			HAL_KEY_LCTRL, 
			HAL_KEY_F)
		) ed_onSearchTool = true;
		
    if(ed_onSearchTool == true){
        if(hal_inp_isKeyPressed(HAL_KEY_ESC)){
            ed_onSearchTool = false;
            dw_requestRenderEvent(DW_RENDER_ALL);
        }else{
            ed_drawSearchTool();
			
            if(hal_inp_isKeyPressed(HAL_KEY_ESC)){
                ed_onSearchTool = false;
                dw_requestRenderEvent(DW_RENDER_ALL);
            }else if (!hal_inp_isKeyDown(HAL_KEY_ENTER)){
                ed_findWord();
                dw_requestRenderEvent(DW_RENDER_SEARCH);
            }else{                
                ed_searchMoveCursor();
                dw_requestRenderEvent(DW_RENDER_SEARCH);
            }
        }
    } 
}

SearchMetadata *f_createSearchMetadata(char *filename){
	char newName[255] = {'\0'};
	SearchMetadata *new = NULL;
	MemoryArena *arena = NULL;
	
	// we could improve by also adding a random number between them
	sprintf(newName, "SRCH-%s",fs_getFileName(filename));
	arena = (MemoryArena*)mem_arena_create(newName, MEM_ARENA_2K);

	if(!arena) return NULL;

	new = (SearchMetadata*)mem_arena_alloc(arena, sizeof(SearchMetadata));
	new->arena = arena;
	
    new->dialogInputIndex = 0;

    memset(
		new->dialogInputBuffer,
		'\0',
		255
	);

    new->wordCount = 0;
    new->words = NULL;
    new->currentWordNode = NULL;

	return new;
}

// Free a search meta data object
void f_freeSearchMetadata(SearchMetadata *searchMetaData){
    MemoryArena *arena = NULL;

    if(
        !searchMetaData ||
        !searchMetaData->arena
    ) return;

    arena = searchMetaData->arena;

    mem_arena_free(arena);
}

// Reset search metadata, by creating a new fresh arena with
SearchMetadata *f_resetSearchMetadata(SearchMetadata *searchMetadata){
    char *oldName = NULL;
    if(
        !searchMetadata ||
        !searchMetadata->arena
    ) return NULL;

    // Offeted to the end  of the SRCH- prefix
    oldName = strdup(searchMetadata->arena->name + 5);

    f_freeSearchMetadata(searchMetadata);

    searchMetadata = f_createSearchMetadata(oldName);

    return searchMetadata;
}
// This is a small program for testing. 
// The purpose is take a phrase and count the words.e

int ed_wordCountInStr(char *str){
	int wordCount = 0;
	char *wordIndexPtr;
	char *detectedStartOffset = NULL;
	char *detectedEndOffset = NULL;
	size_t detectedWordLen = 0;
	char detectedWord[255] = {'\0'};
	wordIndexPtr = str;
	
	// No words!Wing by spac
	if(*wordIndexPtr == '\0') return 0;

	while(wordIndexPtr && *wordIndexPtr != '\0'){
		// Buffer reset
		// Word start is when the previous char is empty space
		// Word end is when next char is space
		// We have to ignore escape chars
		// We need to calculate the len between both detectedWord offsets.
		if(!detectedStartOffset &&
			isalpha(*wordIndexPtr)
		){
			detectedStartOffset = wordIndexPtr;
		
		// If is an space, we know that a word ended
		}else if (*wordIndexPtr == ' ' || *(wordIndexPtr + 1) == '\0'){
			detectedEndOffset = wordIndexPtr;
			
			wordCount++;
			if(detectedEndOffset && detectedStartOffset){
				detectedWordLen = detectedEndOffset - detectedStartOffset;
				memcpy(detectedWord, detectedStartOffset, detectedWordLen);
				detectedWord[detectedWordLen] = '\0';
				//printf("\nDetected word: %s", detectedWord);
			}
            
			detectedStartOffset = NULL;
			detectedEndOffset = NULL;
			memset(detectedWord, '\0', 255);
			detectedWordLen = 0;
		}
		
		wordIndexPtr++; 
	}

	return wordCount;
}

// This will find word matches according to the currentSearchMetadata
// found word match
void ed_findWord(){
    int wordLen = 0;
    unsigned int lineIndex = 0;
    char *detectedWordOffset = NULL;
    char *wordIndexPtr = NULL;
    char searchArenaName[32];
    WordMetadata *matchBuffer = NULL;
    Node *lineNode = NULL;
	File *currentFile = NULL;
	SearchMetadata *currentFileSearch = NULL;

    if(
        !currentWindow ||
        !currentWindow->currentFile ||
        !currentWindow->currentFile->lines ||
        !currentWindow->currentFile->lines->firstNode
    ){

        logger("[ed_findWord]: currentWindow first line node is NULL");
        return;
    }

	currentFile = currentWindow->currentFile;
	currentFileSearch = currentFile->currentFileSearch;

    lineNode = currentFile->lines->firstNode;
	
    if(!lineNode){
        logger("[ed_findWord]: lineNode is NULL");
        return;
    }
    
    if(!currentFileSearch){
        logger("[ed_findWord]: currentFileSearch is NULL");
        return;
    }

    if(!currentFileSearch->arena || !currentFileSearch->arena->base){
        sprintf(searchArenaName, "SRCH");
        currentFileSearch->arena = (MemoryArena *)mem_arena_create(searchArenaName, MEM_ARENA_2K);
    } else {
        mem_arena_reset(currentFileSearch->arena);
    }

    wordLen = strlen(currentFileSearch->dialogInputBuffer);

    if (wordLen == 0) return;


    currentFileSearch->wordCount = 0;
    currentFileSearch->words = NULL;
    currentFileSearch->currentWordNode = NULL;

    // If searchMetadata atributes are NULl this means that there is no previous search done
    // So we will begin the process.

    // If there is already a metadata

    // Depending on the orientation ( previous, next ) we will look forward or previous from the wordOffset and 
    // the line index.
    logger(
		"[ed_findWord]: Current word! : %s",
		currentFileSearch->dialogInputBuffer
	);

    while(lineNode != NULL){
        
        if(
            !lineNode->data ||
            !((Line*)lineNode->data)->buffer
        ){
            logger(
				"[ed_findWord]: lineNode->data or lineNode->data->buffer is NULL"
			);
            return;
        }
        
        wordIndexPtr = ((Line*)(lineNode->data))->buffer;
        // No words!Wing by spac
        if(*wordIndexPtr == '\0'){
            lineNode = lineNode->next;
            lineIndex++;
            continue;
        } 

        detectedWordOffset = 
			strstr(
				wordIndexPtr, 
				currentFileSearch->dialogInputBuffer
			);
        
        while(detectedWordOffset){        
            matchBuffer = 
				(WordMetadata *) 
				mem_arena_alloc(
					currentFileSearch->arena, 
					sizeof(WordMetadata)
				);
     
            if(!matchBuffer){
                logger("[ed_findWord]: Line 1558, matchBuffer is NULL");
                return;
            }

            matchBuffer->lineNode = lineNode;
            matchBuffer->wordIndex = currentFileSearch->wordCount;
    
            matchBuffer->cursorLine = lineIndex;
            logger("[ed_findWord]: matchBuffer->cursorLine = %d", lineIndex);

            matchBuffer->cursorCol = detectedWordOffset - wordIndexPtr;
            // Word position in line
            matchBuffer->wordPtr = detectedWordOffset;
            
            addGenericNode(
				&currentFileSearch->words, 
				matchBuffer, 
				currentFileSearch->arena
			);
            
            currentFileSearch->wordCount++;
  
            detectedWordOffset = 
				strstr(
					detectedWordOffset + wordLen,
					currentFileSearch->dialogInputBuffer
				);
        }
                
        lineNode = lineNode->next;
        lineIndex++;
    }

    // We set the first found word as current word
    currentFileSearch->currentWordNode =
        currentFileSearch->words &&
        currentFileSearch->words->firstNode
        ? currentFileSearch->words->firstNode 
        : NULL ;

}

void ed_drawSearchTool(){
    File *currentFile = currentWindow ? currentWindow->currentFile : NULL;
    SearchMetadata *currentFileSearch = currentFile ? currentFile->currentFileSearch : NULL;
    int vis_offset = 0;
    int dialogStartY = 0;
    
    if (!currentFileSearch) {
		ed_statusBarMessage("No search object instance!.");
		return;
	}
    
    vis_offset = (VIDEO_COLS / 4);
    dialogStartY = 2;

    dw_rectangle(
		textmemptr, 
		vis_offset, 
		dialogStartY, 
		VIDEO_COLS - vis_offset, 
		6, 
		COLOR_BLUE, 
		COLOR_WHITE, 
		' ', 
		COLOR_WHITE, 
		COLOR_BLUE, 
		false, 
		DRAW_BORDER_SIMPLE, 
		"Search..."
	);

    dw_writeBuffer(
        textmemptr, 
        "Found matches: %d, Currently on result: %d", 
        vis_offset + 1, 
        dialogStartY + 2, 
        vis_offset + 48, 
        dialogStartY + 2, 
        COLOR_WHITE, 
        COLOR_BLUE, 
        currentFileSearch->wordCount,
        (
            currentFileSearch->currentWordNode && 
            ((WordMetadata *)currentFileSearch->currentWordNode->data) 
                ? ((WordMetadata *)currentFileSearch->currentWordNode->data)->wordIndex
                : 0
        )
    );

    ed_async_scanf(
		vis_offset + 1, 
		3, 
		(2 * vis_offset) - 1, 
		currentFileSearch->dialogInputBuffer, 
		strlen(currentFileSearch->dialogInputBuffer), 
		&(currentFileSearch->dialogInputIndex)
	);
            
}

void ed_searchMoveCursor(){
    File *currentFile = currentWindow ? currentWindow->currentFile : NULL;
    SearchMetadata *currentFileSearch = currentFile ? currentFile->currentFileSearch : NULL;

    if(
        !currentFile ||
        !currentFileSearch ||
        !currentFileSearch->words ||
        !currentFileSearch->currentWordNode ||
        !currentFileSearch->currentWordNode->data
    ) return;
    
    if(
		hal_inp_isKeyDown(HAL_KEY_ENTER) && 
		!hal_inp_isKeyDown(HAL_KEY_LSHIFT)
	){
        // We go forward
        currentFileSearch->currentWordNode = 
            currentFileSearch->currentWordNode &&
            currentFileSearch->currentWordNode->next
            ? currentFileSearch->currentWordNode->next
            : currentFileSearch->currentWordNode ;        

    }else if (
		hal_inp_isKeyDown(HAL_KEY_ENTER) && 
		hal_inp_isKeyDown(HAL_KEY_LSHIFT)
	){
        // We go back         
        currentFileSearch->currentWordNode = 
            currentFileSearch->currentWordNode &&
            currentFileSearch->currentWordNode->prev
            ? currentFileSearch->currentWordNode->prev
            : currentFileSearch->currentWordNode ;        
    }

    // We update the cursor
    currentFile->currentLineNode = 
        currentFileSearch->currentWordNode &&
        currentFileSearch->currentWordNode->data &&
        ((WordMetadata*) currentFileSearch->currentWordNode->data)->lineNode
        ? ((WordMetadata *)currentFileSearch->currentWordNode->data)->lineNode
        : NULL;

    currentFile->cursorCol = 
        currentFileSearch->currentWordNode &&
        currentFileSearch->currentWordNode->data &&
        ((WordMetadata*) currentFileSearch->currentWordNode->data)->cursorCol
        ? ((WordMetadata *)currentFileSearch->currentWordNode->data)->cursorCol
        : 0;

    currentFile->cursorLine =
        currentFileSearch->currentWordNode &&
        currentFileSearch->currentWordNode->data &&
        ((WordMetadata*) currentFileSearch->currentWordNode->data)->cursorLine
        ? ((WordMetadata *)currentFileSearch->currentWordNode->data)->cursorLine
        : 0;

    ed_updateScrollY();
    ed_updateScrollX();
    ed_updateCursor();
}