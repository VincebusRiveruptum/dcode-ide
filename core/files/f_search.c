#include "files.h"

// ==== SEARCH BEHAVIOR ==================================

// * Each file arena has its own fileSearchMetadata, its not insidie the 
// File definition because this could cause memory usage issues when 
// there are many Word matches, so it separate for better memory 
// perfomance and control

// * The search metadata stores all matches in a pointer array. So,

// * Every time the file changes the searchMetadata of the textArea
// MUST be flush, so there are no dangling pointers nor references to a word
// address that changed.

// * The fileListSearchMetadata INDEX is parallel to fileList textArea
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
	TextArea *textArea = NULL;
	SearchMetadata *searchMetadata = NULL;

    if(
        !currentWindow ||
        !currentWindow->textArea ||
        !currentWindow->textArea->file ||
        !currentWindow->textArea->file->lines ||
        !currentWindow->textArea->file->lines->firstNode
    ){

        logger("[ed_findWord]: currentWindow first line node is NULL");
        return;
    }

	textArea = currentWindow->textArea;
	searchMetadata = textArea->searchMetadata;

    lineNode = textArea->file->lines->firstNode;
	
    if(!lineNode){
        logger("[ed_findWord]: lineNode is NULL");
        return;
    }
    
    if(!searchMetadata){
        logger("[ed_findWord]: searchMetadata is NULL");
        return;
    }

    if(!searchMetadata->arena || !searchMetadata->arena->base){
        sprintf(searchArenaName, "SRCH");
        searchMetadata->arena = (MemoryArena *)mem_arena_create(searchArenaName, MEM_ARENA_2K);
    } else {
        mem_arena_reset(searchMetadata->arena);
    }

    wordLen = strlen(searchMetadata->dialogInputBuffer);

    if (wordLen == 0) return;


    searchMetadata->wordCount = 0;
    searchMetadata->words = NULL;
    searchMetadata->currentWordNode = NULL;

    // If searchMetadata atributes are NULl this means that there is no previous search done
    // So we will begin the process.

    // If there is already a metadata

    // Depending on the orientation ( previous, next ) we will look forward or previous from the wordOffset and 
    // the line index.
    logger(
		"[ed_findWord]: Current word! : %s",
		searchMetadata->dialogInputBuffer
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
				searchMetadata->dialogInputBuffer
			);
        
        while(detectedWordOffset){        
            matchBuffer = 
				(WordMetadata *) 
				mem_arena_alloc(
					searchMetadata->arena, 
					sizeof(WordMetadata)
				);
     
            if(!matchBuffer){
                logger("[ed_findWord]: Line 1558, matchBuffer is NULL");
                return;
            }

            matchBuffer->lineNode = lineNode;
            matchBuffer->wordIndex = searchMetadata->wordCount;
    
            matchBuffer->cursorLine = lineIndex;
            logger("[ed_findWord]: matchBuffer->cursorLine = %d", lineIndex);

            matchBuffer->cursorCol = detectedWordOffset - wordIndexPtr;
            // Word position in line
            matchBuffer->wordPtr = detectedWordOffset;
            
            addGenericNode(
				&searchMetadata->words, 
				matchBuffer, 
				searchMetadata->arena
			);
            
            searchMetadata->wordCount++;
  
            detectedWordOffset = 
				strstr(
					detectedWordOffset + wordLen,
					searchMetadata->dialogInputBuffer
				);
        }
                
        lineNode = lineNode->next;
        lineIndex++;
    }

    // We set the first found word as current word
    searchMetadata->currentWordNode =
        searchMetadata->words &&
        searchMetadata->words->firstNode
        ? searchMetadata->words->firstNode 
        : NULL ;

}

void ed_drawSearchTool(){
    TextArea *textArea = currentWindow ? currentWindow->textArea : NULL;
    SearchMetadata *searchMetadata = textArea ? textArea->searchMetadata : NULL;
    int vis_offset = 0;
    int dialogStartY = 0;
    
    if (!searchMetadata) {
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
        searchMetadata->wordCount,
        (
            searchMetadata->currentWordNode && 
            ((WordMetadata *)searchMetadata->currentWordNode->data) 
                ? ((WordMetadata *)searchMetadata->currentWordNode->data)->wordIndex
                : 0
        )
    );

    ed_async_scanf(
		vis_offset + 1, 
		3, 
		(2 * vis_offset) - 1, 
		searchMetadata->dialogInputBuffer, 
		strlen(searchMetadata->dialogInputBuffer), 
		&(searchMetadata->dialogInputIndex)
	);
            
}

void ed_searchMoveCursor(){
    TextArea *textArea = currentWindow ? currentWindow->textArea : NULL;
    SearchMetadata *searchMetadata = textArea ? textArea->searchMetadata : NULL;

    if(
        !textArea ||
        !searchMetadata ||
        !searchMetadata->words ||
        !searchMetadata->currentWordNode ||
        !searchMetadata->currentWordNode->data
    ) return;
    
    if(
		hal_inp_isKeyDown(HAL_KEY_ENTER) && 
		!hal_inp_isKeyDown(HAL_KEY_LSHIFT)
	){
        // We go forward
        searchMetadata->currentWordNode = 
            searchMetadata->currentWordNode &&
            searchMetadata->currentWordNode->next
            ? searchMetadata->currentWordNode->next
            : searchMetadata->currentWordNode ;        

    }else if (
		hal_inp_isKeyDown(HAL_KEY_ENTER) && 
		hal_inp_isKeyDown(HAL_KEY_LSHIFT)
	){
        // We go back         
        searchMetadata->currentWordNode = 
            searchMetadata->currentWordNode &&
            searchMetadata->currentWordNode->prev
            ? searchMetadata->currentWordNode->prev
            : searchMetadata->currentWordNode ;        
    }

    // We update the cursor
    textArea->currentLineNode = 
        searchMetadata->currentWordNode &&
        searchMetadata->currentWordNode->data &&
        ((WordMetadata*) searchMetadata->currentWordNode->data)->lineNode
        ? ((WordMetadata *)searchMetadata->currentWordNode->data)->lineNode
        : NULL;

    textArea->cursorCol = 
        searchMetadata->currentWordNode &&
        searchMetadata->currentWordNode->data &&
        ((WordMetadata*) searchMetadata->currentWordNode->data)->cursorCol
        ? ((WordMetadata *)searchMetadata->currentWordNode->data)->cursorCol
        : 0;

    textArea->cursorLine =
        searchMetadata->currentWordNode &&
        searchMetadata->currentWordNode->data &&
        ((WordMetadata*) searchMetadata->currentWordNode->data)->cursorLine
        ? ((WordMetadata *)searchMetadata->currentWordNode->data)->cursorLine
        : 0;

    ed_updateScrollY();
    ed_updateScrollX();
    ed_updateCursor();
}