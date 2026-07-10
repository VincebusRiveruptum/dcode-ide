#ifndef APP_FILES_H
#define APP_FILES_H

#include "../std.h"

#include "../../app/main.h"
#include "../../deps/data/data.h"

/* Const. ================================================================*/

#define MAX_FILE_LINE_LENGTH 384
#define MAX_FILE_NAME 256

#define FILE_EXTENSION_TXT 0
#define FILE_EXTENSION_C 1
#define FILE_EXTENSION_PYTHON 2
#define FILE_EXTENSION_JS 3

/* Types =================================================================*/

typedef struct WordMetadata{
    struct Node *lineNode;
    unsigned int wordIndex;
    unsigned int cursorLine;
    unsigned int cursorCol;
    char *wordPtr;
}WordMetadata;

typedef struct SearchMetadata{
	// File search arena.
    MemoryArena *arena;

    // Input buffer index for the search dialog
    int dialogInputIndex;
    
    // Word to search (input from search dialog)
    char dialogInputBuffer[255];
    
    // Count of matches
    unsigned int wordCount;

    // List of WordMetadata.
    List *words;
    Node *currentWordNode;

} SearchMetadata;

typedef struct Line {
    char *buffer;
    size_t length;
} Line;

typedef struct File {
	// File own arena
	MemoryArena *arena;

    // Meta helper, used by search functions, could be used for file opening/saving/closing too in the future.
    int fileIndex;

    char *name;         // full name and path
    unsigned char ext;  // EXTENSION ID

    size_t bufferLength;  /* also known as file size */
    
    /* New line-based fields */
    List *lines;
    List *deletedLines;

    // Could be useful in the future
    Node *currentLineNode;
    
    Line *prevLine;
    Line *currentLine;
    Line *nextLine;

    // Current line position chars
    char prevChar;
    char currentChar;
    char nextChar;

    // This will be used for the editor to know if the previous line to the first visible was a comment
    // So the comment formmating will still function when scrolling down
    
    unsigned short scrollY;
    unsigned short scrollX;

    // Indicates the current cursor line index
    unsigned short cursorLine;

    // Indicates the current cursor column index
    unsigned short cursorCol;

    // Selectection metadata
    unsigned short selectedStartX;
    unsigned short selectedEndX;
    unsigned short selectedStartLine;
    unsigned short selectedEndLine;

    struct Node *oldLineNode;
    unsigned short oldCol;
    unsigned short oldLine;
    
    Node *selectedStartNode;
    Node *selectedEndNode;

	SearchMetadata *currentFileSearch;
	
    bool isModified;
    bool isActive;

} File;

typedef enum WndStatus {
	WndStatus_INITIALIZED,
	WndStatus_MAXIMIZED,
	WndStatus_MINIMIZED,
	WndStatus_FLOATING,	
}WndStatus;

typedef struct Window{
	// Window tabs
	List *fileList;
	File *currentFile;
	WndStatus status;

	unsigned int x;
	unsigned int y;

	unsigned int width;
	unsigned int height;

	unsigned int currentFileIndex;
	unsigned int index;
	bool active;
}Window;

/*
	A workspace is basically a dump that records the state/session
	of the editor at any moment.
	- The main idea is that if the user wants to keep the previous
	session then restore it we could by defining Workspaces.

	For now, is just a single opened workspace.
	It could be a List if we want to have multiple workspaces
	opened in a session.
*/
typedef struct Workspace{
	char *fullPath;
	List *windowList;
	Window *currentWindow;
} Workspace;

/* Globals ==============================================================*/

extern Workspace *currentWorkspace;
extern Window *currentWindow;

extern bool f_onFileNavigation;
extern bool endProgram;

/* Protypes =============================================================*/

#include "private.h"

// f_base
void f_dumpToFile(char *filename);
void f_dumpBufferTofile(char *buffer, size_t bufferLength, char *filename);
void f_newFile(char *filename);
bool f_openFile(char *filename);
void f_saveFile();
void f_closeFile(File *file);
void f_triggerClose(bool end_program);
void f_closeCurrentFile();

// f_nav
void f_prepareFileNavDialog();
void f_drawFileNavDialog();

// f_search
void ed_prepareSearchTool();
SearchMetadata *f_createSearchMetadata(char *filename);
void f_freeSearchMetadata(SearchMetadata *data);
void f_flushSearchMetadata();
void ed_drawSearchTool();
void ed_searchMoveCursor();
void ed_findWord();

// f_qopen
void f_quickOpenFileDialog();

// f_wnd
Window *f_createWindow();
Workspace *f_createWorkspace();
void f_freeFileList(List *fileList);
void f_freeWindowList(List *windowList);
void f_freeWorkspace();
File *f_addFileToWindow(Window *window, File *file);
Window *f_addWindowToWorkspace(Workspace *workspace, Window *window);
void f_deleteFileFromWindow(Window *window, File *file);
void f_deleteWindowFromWorkspace(Workspace *workspace, Window *window);
void f_splitWindow();
void f_cycleActiveWindow();
void f_refreshWindows(Workspace *workspace);

#endif
