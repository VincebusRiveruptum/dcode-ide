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

    char *name;         // full name and path
    unsigned char ext;  // EXTENSION ID

    size_t bufferLength;  /* also known as file size */
    
    /* New line-based fields */
    List *lines;
    List *deletedLines;

    bool isModified;
} File;

typedef struct TextArea {
    File *file;
    
	MemoryArena *arena;

    int fileIndex;
    // Meta helper, used by search functions, could be used for file opening/saving/closing too in the future.
    
    // Text-area metadata  
    Node *currentLineNode;
    
    Line *prevLine;
    Line *currentLine;
    Line *nextLine;

    // Current line posPition chars
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

	SearchMetadata *searchMetadata;
	
    bool isActive;
} TextArea;

typedef enum WndStatus {
	WndStatus_INITIALIZED,
	WndStatus_MAXIMIZED,
	WndStatus_MINIMIZED,
	WndStatus_FLOATING,	
}WndStatus;

typedef struct EditorWindow{
	// EditorWindow tabs
	List *textAreaList;
	TextArea *textArea;
	WndStatus status;

	unsigned int x;
	unsigned int y;

	unsigned int width;
	unsigned int height;

	unsigned int currentFileIndex;
	unsigned int index;
	bool active;
}EditorWindow;

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
	EditorWindow *currentWindow;
} Workspace;

/* Globals ==============================================================*/

extern Workspace *currentWorkspace;
extern EditorWindow *currentWindow;

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

void f_triggerClose(bool end_program);
void f_closeCurrentTextArea();
void f_setCurrentFileAsModified();

// f_nav
void f_prepareFileNavDialog();
void f_drawFileNavDialog();

// f_search
void ed_prepareSearchTool();
SearchMetadata *f_createSearchMetadata(char *filename);
void f_freeSearchMetadata(SearchMetadata *data);
SearchMetadata *f_resetSearchMetadata(SearchMetadata *data);
void ed_drawSearchTool();
void ed_searchMoveCursor();
void ed_findWord();

// f_qopen
void f_quickOpenFileDialog();

// f_wnd
EditorWindow *f_createWindow();
Workspace *f_createWorkspace();

void f_closeFile(File *file);
void f_closeTextArea(TextArea *textArea);
void f_closeCurrentTextArea();
void f_deleteTextAreaFromWindow(EditorWindow *window, TextArea *textArea);

void f_freeFileList(List *fileList);
void f_freeWindowList(List *windowList);
void f_freeWorkspace();
TextArea *f_addTextAreaToWindow(EditorWindow *window, TextArea *textArea);
EditorWindow *f_addWindowToWorkspace(Workspace *workspace, EditorWindow *window);

void f_deleteWindowFromWorkspace(Workspace *workspace, EditorWindow *window);
void f_splitWindow();
void f_cycleActiveWindow();
void f_refreshWindows(Workspace *workspace);

#endif

