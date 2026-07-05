
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

// FLUSH METADATA

void f_flushSearchMetadata(){
    if(
		!currentWindow || 
		!currentWindow->currentFile ||
		!currentWindow->currentFile->currentFileSearch) 
	return;

    mem_arena_free(
		currentWindow->currentFile->currentFileSearch->arena
	);
	
    currentWindow->currentFile->currentFileSearch->arena = NULL; // Ensure pointer is cleared

    //currentFileSearch->dialogInputIndex = 0;
    //memset(currentFileSearch->dialogInputBuffer, '\0', 255);

    currentWindow->currentFile->currentFileSearch->wordCount = 0;
    currentWindow->currentFile->currentFileSearch->words = NULL;
    currentWindow->currentFile->currentFileSearch->currentWordNode = NULL;
}

void f_allocSearchMetadata(){
	if(
		!currentWindow || 
		!currentWindow->currentFile ||
		!currentWindow->currentFile->currentFileSearch
	) {
		logger("[f_allocSearchMetadat]: No valid currentFile data");
		return;
	}
	
    mem_arena_init(
		currentWindow->currentFile->currentFileSearch->arena,
		currentWindow->currentFile->name,
		MEM_ARENA_2K
	);
    
    currentWindow->currentFile->currentFileSearch->dialogInputIndex = 0;

    memset(
		currentWindow->currentFile->currentFileSearch->dialogInputBuffer,
		'\0',
		255
	);

    currentWindow->currentFile->currentFileSearch->wordCount = 0;
    currentWindow->currentFile->currentFileSearch->words = NULL;
    currentWindow->currentFile->currentFileSearch->currentWordNode = NULL;
}
