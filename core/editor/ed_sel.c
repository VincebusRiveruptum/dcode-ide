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
	static TextArea *textArea = NULL;

    if(!currentWindow || !currentWindow->textArea) 
		return;

	textArea = currentWindow->textArea;

    textArea->selectedStartNode = NULL;
    textArea->selectedEndNode = NULL;
    textArea->selectedStartX = 0;
    textArea->selectedEndX = 0;
    textArea->selectedStartLine = 0;
    textArea->selectedEndLine = 0;
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

    // Check if cursor actually moved
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
            // If selection is not active, anchor it at the old position
            if (textArea->selectedStartNode == NULL) {
                textArea->selectedStartNode = textArea->oldLineNode;
                textArea->selectedStartX = textArea->oldCol;
                textArea->selectedStartLine = textArea->oldLine;
            }
            // Always update selection end to the new position
            textArea->selectedEndNode = textArea->currentLineNode;
            textArea->selectedEndX = textArea->cursorCol;
            textArea->selectedEndLine = textArea->cursorLine;
            on_selection_tool = true;
            dw_requestRenderEvent(DW_RENDER_SELECTION);
        } else {
            // Clear selection since we moved cursor without Shift
            ed_clearSelection();
        }
    } else {
        // Any other cursor movement (e.g. typing, backspace, new line) 
		// clears selection
        ed_clearSelection();
    }
    
}

// This is agnostic to draw.c line rendering methods as this
// copy the screen line portion on the screen and 
// applies the selection mask

// This is WIP
// currently just renders the last state of the selected line
// if we are selecting more that one line, the lines between will 
// not be rendered yet.
// 
void ed_renderLineSelection(){
    // Copy crrent line video memory area
    unsigned short *lineBuffer = NULL;
    static TextArea *textArea = NULL;
    // Selectection metadata
    unsigned short selectedStartX = 0;
    unsigned short selectedEndX = 0;
    unsigned short tmp = 0;
    int step = 0;
    int i=0;

    if(
        !currentWorkspace || 
        !currentWorkspace->currentWindow ||
        !currentWorkspace->currentWindow->textArea
    )   return ;

    textArea = currentWorkspace->currentWindow->textArea;

    lineBuffer = 
        _getCurrentLinePtrInBuffer(
            textmemptr, 
            currentWorkspace->currentWindow
        );

    if(!lineBuffer)
        return;

    // This retruns the start and position of the selection
    // highlingting depending on the selection direction
    _calculateSelectedLineStartEnd(
        currentWorkspace->currentWindow,
        &selectedStartX, 
        &selectedEndX,
        &step 
    );
     // Selection Highlighting
    for(i=selectedStartX; i != selectedEndX ; i += step){
        lineBuffer[i] = lineBuffer[i] & 0x00FF;
        lineBuffer[i] = lineBuffer[i] | ((COLOR_LIGHT_GRAY << 4 | COLOR_BLACK) << 8);
    }

    ed_updateCursor();

    return;
}

// SAFE, no validation!!
// Returns number of deleted chars in a selected
// block in a line.
int _deleteInSingleLine(){
    TextArea *textArea = currentWorkspace->currentWindow->textArea;
    Line *line = textArea->currentLine;
    unsigned short start, end, lenBetween;

    // Start must be a minor index 
    // than the end index.
    if(
        textArea->selectedStartX <
        textArea->selectedEndX
     ){
        start =  textArea->selectedStartX;
        end =  textArea->selectedEndX;
    }else{
        start = textArea->selectedEndX;
        end = textArea->selectedStartX;
    }

    lenBetween = line->length - end;

    if(!line->buffer || !line->length) 
        return -1;
    
    memcpy(line->buffer + start, line->buffer + end, lenBetween);

    // Null termination, so remaining garbage is ignored.
    line->buffer[start + lenBetween + 1] = '\0';
    line->length = start + lenBetween;

    textArea->cursorCol = start;
    
    return (end - start);
}

void _glueLines(Node *start, Node *end, unsigned short startLineIndex){
    Line *startLine, *endLine;
    TextArea *textArea = currentWorkspace->currentWindow->textArea;
    unsigned short startX, endX, lenOnwards;
    // Validation
    if(
        !start ||
        !start->data ||
        !((Line*)(start->data))->buffer ||
        !end ||
        !end->data ||
        !((Line*)(end->data))->buffer
    )   return;

    startLine = (Line*)start->data;
    endLine = (Line*)end->data;

    // Column gluing
    // Start must be a minor index 
    // than the end index.
    if(
        textArea->selectedStartX <
        textArea->selectedEndX
     ){
        startX =  textArea->selectedStartX;
        endX =  textArea->selectedEndX;
    }else{
        startX = textArea->selectedEndX;
        endX = textArea->selectedStartX;
    }

    lenOnwards = 
        (int)(endLine->length - endX) > 0
        ? (endLine->length - endX)
        : 0;

    memcpy(
        startLine->buffer + startX, 
        endLine->buffer + endX, 
        lenOnwards
    );
    
    startLine->buffer[startX + lenOnwards + 1] = '\0'; 
    startLine->length = startX + lenOnwards; 
    
    //;
    
    // We glue the lines
    start->next = 
        end &&
        end->next
        ? end->next
        : NULL;
         
    if(
        end &&
        end->next &&
        end->next->prev
    ){
        end->next->prev = start;
    }

    // Recycle end node
    ed_softDeleteLine(textArea, end);

    // Update editor metadata
    textArea->currentLineNode = start;
    textArea->currentLine = startLine;
    textArea->prevLine = start->prev ? (Line*)start->prev->data : NULL;
    textArea->nextLine = start->next ? (Line*)start->next->data : NULL;

    textArea->cursorCol = startX;
    textArea->cursorLine = startLineIndex;
}

int _deleteSelectedLines(){
    TextArea *textArea = currentWorkspace->currentWindow->textArea;
    Node *start = NULL;
    Node *startNext = NULL;
    Node *end = NULL;
    Node *endPrev = NULL;
    Node *rec = NULL, *tmp=NULL;
    unsigned short startLineIndex;
    // Ordering so always start is a a position
    // previous to the end

    if(
        textArea->selectedStartLine <
        textArea->selectedEndLine 
    ){
        start = textArea->selectedStartNode;
        startLineIndex = textArea->selectedStartLine;
        end = textArea->selectedEndNode;
        
    }else{
        start = textArea->selectedEndNode;
        startLineIndex = textArea->selectedEndLine;
        end = textArea->selectedStartNode;   
    }
    
    // Then we delete the nodes in between
    // Only if there are more than three
    // lines selected.
    if(
        (!start || !end) ||     // null check
        (start == end)       // bound check
    ) return -1;
    
    rec = start->next;

    while(
        (rec != NULL ) && 
        (rec != end->prev) &&          
        (rec != end)      
    ){
        tmp = rec->next;
        ed_softDeleteLine(textArea, rec);
        rec = tmp;
    }
       
    // Then we glue the lines on each side of the
    // deleted lines gap.
    _glueLines(start, end, startLineIndex);

    ed_clearSelection();
    return 1;
}

void ed_deleteSelection(){
    EditorWindow *currentWindow = NULL;
    TextArea *textArea = NULL;
    Line *deletedLine = NULL;
    
    if( 
        !currentWorkspace ||
        !currentWindow ||
        !currentWindow->textArea ||
        !currentWindow->textArea->file
    ) return;

    currentWindow = currentWorkspace->currentWindow;
    textArea = currentWindow->textArea;
    // Check selected line nodes
    if(
        !textArea->selectedStartNode ||
        !textArea->selectedEndNode
    )
    return;
    
    //  Simple deletion,, in the same line
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
            
    }else{
        // deletion if more lines involved ( >1)
        _deleteSelectedLines();
        
    }

    f_setCurrentFileAsModified();
}