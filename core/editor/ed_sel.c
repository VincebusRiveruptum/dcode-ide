#include "editor.h"

void ed_prepareSelectionTool(){
    static TextArea *textArea = NULL;

    if(
        !currentWindow ||
        !currentWindow->textArea ||
        !currentWindow->textArea->file
    ) return;
	
	textArea = currentWindow->textArea;

	textArea->oldLineNode = 
		(struct Node *)textArea->currentLineNode;

	textArea->oldLine = textArea->cursorLine;
	textArea->oldCol = textArea->cursorCol;
}

void ed_clearSelection(){
    TextArea *textArea = NULL;

    if(!currentWindow || !currentWindow->textArea) 
		return;

	textArea = currentWindow->textArea;

    textArea->selectedStartNode = NULL;
    textArea->selectedEndNode = NULL;
    textArea->selectedStartX = 0;
    textArea->selectedEndX = 0;
    textArea->selectedStartLine = 0;
    textArea->selectedEndLine = 0;
    textArea->oldLineNode = (struct Node *)textArea->currentLineNode;
    textArea->oldLine = textArea->cursorLine;
    textArea->oldCol = textArea->cursorCol;
    on_selection_tool = false;
}
 
void ed_handleSelection() {
    static TextArea *textArea;
    bool isNav;

    if(
        !currentWindow ||
        !currentWindow->textArea ||
        !currentWindow->textArea->file
    ) return;
    
    textArea = currentWindow->textArea;

    /* Check if cursor actually moved */
    if (
		textArea->currentLineNode == textArea->oldLineNode && 
		textArea->cursorCol == textArea->oldCol
	) return;
    
    isNav = 
		hal_inp_isKeyDown(HAL_KEY_UP) || 
		hal_inp_isKeyDown(HAL_KEY_DOWN) ||
        hal_inp_isKeyDown(HAL_KEY_LEFT) || 
		hal_inp_isKeyDown(HAL_KEY_RIGHT) ||
        hal_inp_isKeyDown(HAL_KEY_HOME) || 
		hal_inp_isKeyDown(HAL_KEY_END) ||
        hal_inp_isKeyDown(HAL_KEY_PAGEUP) || 
		hal_inp_isKeyDown(HAL_KEY_PAGEDOWN);

    if (isNav) {
        if (
			hal_inp_isKeyDown(HAL_KEY_LSHIFT) || 
			hal_inp_isKeyDown(HAL_KEY_RSHIFT)) 
		{
            /* If selection is not active, anchor it at the old position */
            if (textArea->selectedStartNode == NULL) {
                textArea->selectedStartNode = textArea->oldLineNode;
                textArea->selectedStartX = textArea->oldCol;
                textArea->selectedStartLine = textArea->oldLine;
            }
            /* Always update selection end to the new position */
            textArea->selectedEndNode = textArea->currentLineNode;
            textArea->selectedEndX = textArea->cursorCol;
            textArea->selectedEndLine = textArea->cursorLine;
            on_selection_tool = true;
            dw_requestRenderEvent(DW_RENDER_SELECTION);
        } else {
            /* Clear selection since we moved cursor without Shift */
            ed_clearSelection();
        }
    } else {
        /* Any other cursor movement (e.g. typing, backspace, new line) */
		/* clears selection */
        ed_clearSelection();
    }
    
}

/* This is agnostic to draw.c line rendering methods as this */
/* copy the screen line portion on the screen and */
/* applies the selection mask */

/* This is WIP */
/* currently just renders the last state of the selected line */
/* if we are selecting more that one line, the lines between will */
/* not be rendered yet. */
/* */
void ed_renderLineSelection(){
    /* Copy crrent line video memory area */
    unsigned short *lineBuffer = NULL;
    /* Selectection metadata */
    unsigned short selectedStartX = 0;
    unsigned short selectedEndX = 0;
    int step = 0;
    int i=0;

    if(
        !currentWorkspace || 
        !currentWorkspace->currentWindow ||
        !currentWorkspace->currentWindow->textArea
    )   return ;

    lineBuffer = 
        _getCurrentLinePtrInBuffer(
            textmemptr, 
            currentWorkspace->currentWindow
        );

    if(!lineBuffer)
        return;

    /* This retruns the start and position of the selection */
    /* highlingting depending on the selection direction */
    _calculateSelectedLineStartEnd(
        currentWorkspace->currentWindow,
        &selectedStartX, 
        &selectedEndX,
        &step 
    );
     /* Selection Highlighting */
    for(i=selectedStartX; i != selectedEndX ; i += step){
        lineBuffer[i] = lineBuffer[i] & 0x00FF;
        lineBuffer[i] = lineBuffer[i] | ((COLOR_LIGHT_GRAY << 4 | COLOR_BLACK) << 8);
    }

    ed_updateCursor();

    return;
}

/* Returns number of deleted chars in a selected block in a line. */
int _deleteInSingleLine(void){
    unsigned short start = 0;
    unsigned short end = 0;
    unsigned short lenBetween = 0;
    Line *line = NULL;
    TextArea *textArea = NULL;

    if(
        !currentWorkspace ||
        !currentWorkspace->currentWindow ||
        !currentWorkspace->currentWindow->textArea ||
        !currentWorkspace->currentWindow->textArea->currentLine
    ){
        return -1;
    }

    textArea = currentWorkspace->currentWindow->textArea;
    line = textArea->currentLine;

    if(!line->buffer || line->length == 0){
        return -1;
    }

    /* Start must be a minor index than the end index. */
    if(textArea->selectedStartX < textArea->selectedEndX){
        start = textArea->selectedStartX;
        end = textArea->selectedEndX;
    }else{
        start = textArea->selectedEndX;
        end = textArea->selectedStartX;
    }

    if(start > line->length){
        start = line->length;
    }
    if(end > line->length){
        end = line->length;
    }

    lenBetween = line->length - end;

    if(lenBetween > 0){
        memcpy(line->buffer + start, line->buffer + end, lenBetween);
    }

    line->length = start + lenBetween;
    line->buffer[line->length] = '\0';

    textArea->cursorCol = start;
    
    return (int)(end - start);
}

void _glueLines(
    Node *start, 
    Node *end, 
    unsigned short startLineIndex,
    unsigned short startX,
    unsigned short endX
){
    unsigned short lenOnwards = 0;
    Line *startLine = NULL;
    Line *endLine = NULL;
    TextArea *textArea = NULL;

    /* Validation */
    if(
        !currentWorkspace ||
        !currentWorkspace->currentWindow ||
        !currentWorkspace->currentWindow->textArea ||
        !start ||
        !start->data ||
        !((Line*)(start->data))->buffer ||
        !end ||
        !end->data ||
        !((Line*)(end->data))->buffer
    ){
        return;
    }

    textArea = currentWorkspace->currentWindow->textArea;
    startLine = (Line*)start->data;
    endLine = (Line*)end->data;

    if(startX > startLine->length){
        startX = startLine->length;
    }
    if(endX > endLine->length){
        endX = endLine->length;
    }

    lenOnwards = (endLine->length > endX) ? (endLine->length - endX) : 0;
    if(startX + lenOnwards > MAX_FILE_LINE_LENGTH){
        lenOnwards = MAX_FILE_LINE_LENGTH - startX;
    }

    if(lenOnwards > 0){
        memcpy(
            startLine->buffer + startX, 
            endLine->buffer + endX, 
            lenOnwards
        );
    }
    
    startLine->length = startX + lenOnwards; 
    startLine->buffer[startLine->length] = '\0'; 

    /* Link start and end directly before recycling end */
    end->prev = start;
    start->next = end;

    /* Recycle end node */
    ed_softDeleteLine(textArea, end);

    /* Update editor metadata */
    textArea->currentLineNode = start;
    textArea->currentLine = startLine;
    textArea->prevLine = start->prev ? (Line*)start->prev->data : NULL;
    textArea->nextLine = start->next ? (Line*)start->next->data : NULL;

    textArea->cursorCol = startX;
    textArea->cursorLine = startLineIndex;
}

int _deleteSelectedLines(void){
    int i = 0;
    unsigned short startLineIndex = 0;
    unsigned short startX = 0;
    unsigned short endX = 0;
    Node *start = NULL;
    Node *end = NULL;
    Node *rec = NULL;
    Node *tmp = NULL;
    Node *check = NULL;
    TextArea *textArea = NULL;

    if(
        !currentWorkspace || 
        !currentWorkspace->currentWindow ||
        !currentWorkspace->currentWindow->textArea ||
        !currentWorkspace->currentWindow->textArea->file ||
        !currentWorkspace->currentWindow->textArea->file->lines
    ){
        return -1;
    }

    textArea = currentWorkspace->currentWindow->textArea;

    /* Ordering so always start is at a position previous to the end */
    if(textArea->selectedStartLine < textArea->selectedEndLine){
        start = textArea->selectedStartNode;
        startLineIndex = textArea->selectedStartLine;
        startX = textArea->selectedStartX;
        end = textArea->selectedEndNode;
        endX = textArea->selectedEndX;
    }else if(textArea->selectedStartLine > textArea->selectedEndLine){
        start = textArea->selectedEndNode;
        startLineIndex = textArea->selectedEndLine;
        startX = textArea->selectedEndX;
        end = textArea->selectedStartNode;
        endX = textArea->selectedStartX;
    }else{
        check = textArea->selectedStartNode ? 
            textArea->selectedStartNode->next : NULL;
        while(check != NULL && check != textArea->selectedEndNode){
            check = check->next;
        }
        if(check == textArea->selectedEndNode){
            start = textArea->selectedStartNode;
            startLineIndex = textArea->selectedStartLine;
            startX = textArea->selectedStartX;
            end = textArea->selectedEndNode;
            endX = textArea->selectedEndX;
        }else{
            start = textArea->selectedEndNode;
            startLineIndex = textArea->selectedEndLine;
            startX = textArea->selectedEndX;
            end = textArea->selectedStartNode;
            endX = textArea->selectedStartX;
        }
    }
    
    /* Bounds and null check */
    if(!start || !end || start == end){
        return -1;
    }
    
    rec = start->next;

    /* Delete all nodes in between start and end */
    while(rec != NULL && rec != end){
        tmp = rec->next;
        ed_softDeleteLine(textArea, rec);
        logger("DELETING %d lines", i);
        rec = tmp;
        i++;
    }

    if(rec != end){
        logger("[_deleteSelectedLines] Error: end node was not reached");
        return -1;
    }
       
    /* Glue the lines on each side of the deleted lines gap */
    _glueLines(
        start, 
        end, 
        startLineIndex, 
        startX, 
        endX
    );

    ed_clearSelection();
    return 1;
}

void ed_deleteSelection(void){
    EditorWindow *currentWindow = NULL;
    TextArea *textArea = NULL;
    
    if( 
        !currentWorkspace ||
        !currentWorkspace->currentWindow ||
        !currentWorkspace->currentWindow->textArea ||
        !currentWorkspace->currentWindow->textArea->file
    ){
        logger("[ed_deleteSelection] Error: Invalid file universe.");
        exit(1);
    }

    currentWindow = currentWorkspace->currentWindow;
    textArea = currentWindow->textArea;

    /* Check selected line nodes */
    if(
        !textArea->selectedStartNode ||
        !textArea->selectedEndNode
    ){
        return;
    }
    
    /* Simple deletion, in the same line */
    if(
        (
            textArea->selectedStartNode ==
            textArea->selectedEndNode
        ) &&
        (
            textArea->selectedStartLine ==
            textArea->selectedEndLine
        )
    ){
        if(_deleteInSingleLine() < 0){
            logger(
                "[ed_deleteSelection]: Error while" 
                " deleting selected text"
            );
        }
        ed_clearSelection();
    }else{
        /* Deletion if more lines involved ( >1) */
        _deleteSelectedLines();
    }

    ed_updateScrollY();
    f_setCurrentFileAsModified();
}

/*
    This functions tells if there is 
    a selected text block right now
*/

bool ed_isSelectedText(Workspace *workspace){
    TextArea *textArea = NULL;
    if(
        !workspace ||
        !workspace->currentWindow ||
        !workspace->currentWindow->textArea
    ){
        logger("[ed_isSelectedText] Error: Invalid workspace/window/textArea universe.");
        exit(1);
    }

    textArea = workspace->currentWindow->textArea;

    if(
        (textArea->selectedStartX >= 0) &&
        (textArea->selectedEndX >= 0 ) &&
        (textArea->selectedStartLine >= 0) &&
        (textArea->selectedEndLine >= 0) &&
        textArea->selectedStartNode &&
        textArea->selectedEndNode
    ){
        return true;
    }

    return false;

}