#include "files.h"

/*
    This is basically VSCODE's quick open feature. The behavior is the following:
    Prompt that has pre-filled the absolute path of the current position, filalble 
	with the left and right arrows Select file of the current selected path with 
	the up and down arrows. The file list will be reactive depending on hte detected 
	path from the input prompt.
*/

void f_quickOpenFileDialog(){
    int stepIndex = 0;
    int vis_offset = 0;
    int dialogStartY = 0;
    int dialogEndY = 0;
    int filesLength = 0;
    int selectedEntry = 0;
    int nodeIndex = 0;
    int clearMarkPoint = 0;
    int MAX_LIST_HEIGHT = 11;
    int entryScrollY = 0;
    int selectedEntryScrollY = 0;
    char scrollChar = '\0';
    char selectedEntryFullPath[512];
    char currentPath[255];
    bool isSelected = false;

    Directory *currPathDirectory = NULL;
    Node *node = NULL;
    FileEntry *fileEntry = NULL;
    FileEntry *selectedFileEntry = NULL;

    memset(selectedEntryFullPath, '\0', 512);
    memset(currentPath, '\0', 255);

    hal_fs_getAbsoluteCurrentPath(currentPath, 255);
    
    if(currentPath[0] == '\0'){
        logger("[ed_quickOpenFileDialog]: Failed to retrieve currentPath");
    }

    if (strlen(currentPath) < sizeof(currentPath) - 1) {
        strcat(currentPath, FS_PATH_SEPARATOR);
    }

    vis_offset = (VIDEO_COLS / 4);

    dialogStartY = 2;
    dialogEndY = 18; 

    dw_rectangle(
		textmemptr, 
		vis_offset,
		dialogStartY,
		VIDEO_COLS - vis_offset,
		dialogEndY,
		COLOR_BLUE,
		COLOR_WHITE,
		' ',
		COLOR_WHITE,
		COLOR_BLUE,
		false,
		DRAW_BORDER_SIMPLE,
		"OPEN FILE"
	);

    stepIndex = strlen(currentPath);
    
    hal_inp_waitForRelease();

    do{
        // Key selection
        if(hal_inp_isKeyPressed(HAL_KEY_UP)){
            selectedEntry = 
            selectedEntry > 1
                ? selectedEntry - 1
                : 1; 

            if(selectedEntry - entryScrollY <= 0){
                entryScrollY--;
                logger("[f_quickOpenFileDialog] : entryScrollY : %d ", entryScrollY);
            }

        }else if(hal_inp_isKeyPressed(HAL_KEY_DOWN)){
            selectedEntry = 
            selectedEntry < filesLength
                ? selectedEntry + 1
                : filesLength; 

            // We calculate the scrolling
            if(selectedEntry - entryScrollY - 2 > MAX_LIST_HEIGHT ){
                entryScrollY++;
                logger("[f_quickOpenFileDialog] : entryScrollY : %d ", entryScrollY);
            }
            
                
        }else if(hal_inp_isKeyPressed(HAL_KEY_ENTER)){
            // If we press enter, we have to detect if the entry is either a directory or a file
            // 
            if(selectedFileEntry && strcmp(selectedFileEntry->name, "..") == 0){         // .. path
                stepIndex = _goBackPath(currentPath);       
                selectedEntry = 0;
                selectedFileEntry = NULL;
            }else if(selectedFileEntry && selectedFileEntry->isDirectory){
                //// Nothing happens
                strcat(currentPath, selectedFileEntry->name);
                strcat(currentPath, FS_PATH_SEPARATOR);
                stepIndex = strlen(currentPath);
                selectedEntry = 0;
                selectedFileEntry = NULL;
            }else if(selectedFileEntry){
                // We open the file
                // TODO: SANITIZE buffer by removing the chars until the last directory
                //snprintf(selectedEntryFullPath, sizeof(selectedEntryFullPath), "%s%s", currentPath, selectedFileEntry->name);
                sprintf(selectedEntryFullPath, "%s%s", currentPath, selectedFileEntry->name);
                f_openFile(selectedEntryFullPath);
                ed_updateCursor();
                dw_requestRenderEvent(DW_RENDER_ALL);
                if (currPathDirectory) hal_fs_freeDirectory(currPathDirectory);
                return;
            }
			
        }else{

            // Freeing up list of files each time there is a change 
            // in the prompt.

            // Ok, for the file selection we have to clear mark a flag 
            // for the selected file
            // By
            if(currPathDirectory) hal_fs_freeDirectory(currPathDirectory);

            selectedFileEntry = NULL;

            currPathDirectory = hal_fs_getDirectoryFileList(currentPath);

            if(!currPathDirectory){
                logger(
                    "[ed_quickOpenFileDialog]: Could not get currPathDirectory or FileEntry list for selection!");
                return;
            }

            // Draw list of files
            filesLength = currPathDirectory->fileEntries->length;
            node = currPathDirectory->fileEntries->firstNode;
            nodeIndex = 0;
            selectedEntryScrollY = 0;

            while(node != NULL && 
                (selectedEntryScrollY) - 1 <= MAX_LIST_HEIGHT
            ){
                fileEntry = (FileEntry*)node->data;
                nodeIndex++;
                // We write the filename under the prompt
                selectedEntryScrollY = (nodeIndex - entryScrollY);
                        
                if(selectedEntryScrollY <= 0) {
                    node = node->next;
                    continue;
                }
                
                isSelected = 
                    (nodeIndex == selectedEntry) ? true : false;
                    // We mar the selected item or not
                if(isSelected == true){

                    selectedFileEntry = fileEntry;
                    dw_writeBuffer(
                        textmemptr,
                        "%s", 
                        vis_offset + 1, 
                        3 + selectedEntryScrollY + 1, 
                        VIDEO_COLS - vis_offset - 1, 
                        3 + selectedEntryScrollY + 1, 
                        COLOR_BLUE, 
                        COLOR_WHITE, 
                        fileEntry->name
                    );
                }else{
                    dw_writeBuffer(
                        textmemptr,
                        "%s", 
                        vis_offset + 1, 
                        3 + selectedEntryScrollY + 1, 
                        VIDEO_COLS - vis_offset - 1, 
                        3 + selectedEntryScrollY + 1, 
                        COLOR_WHITE, 
                        COLOR_BLUE, 
                        fileEntry->name
                    );
                }

                // Draw scrollbar if there are more items than the 
                // the height of the list
                if(filesLength - 2 > MAX_LIST_HEIGHT){
                    // UP ARROW
                    if((selectedEntryScrollY - 1) == 0){
                        scrollChar = 0x1E;
                    // DOWN ARROW
                    }else if(selectedEntryScrollY - 2 == MAX_LIST_HEIGHT){
                        scrollChar = 0x1F;
                    }else{
                        scrollChar = 0xB1;
                    }

                    dw_writeBuffer(
                        textmemptr,
                        "%c", 
                        VIDEO_COLS - vis_offset - 1, 
                        3 + selectedEntryScrollY + 1, 
                        VIDEO_COLS - vis_offset - 1, 
                        3 + selectedEntryScrollY + 1, 
                        COLOR_BLUE, 
                        COLOR_LIGHT_GRAY, 
                        scrollChar
                    );
                }

                node = node->next;
            }

            // Clear list container until touches bottom
            clearMarkPoint = selectedEntryScrollY;

            if(clearMarkPoint < dialogEndY - 5){
                while(clearMarkPoint < dialogEndY - 5){     
                    clearMarkPoint++;
                    dw_writeBuffer(
                        textmemptr,
                        "%s", 
                        vis_offset + 1, 
                        clearMarkPoint + 4, 
                        VIDEO_COLS - vis_offset - 1, 
                        clearMarkPoint + 4, 
                        COLOR_WHITE, 
                        COLOR_BLUE, 
                        "            "
                    );
                }
            }
            
            // Draw rect in the middle, 1/4 will be the start and the
            // end, so i it will always be in the center
            hal_vid_refresh();
            
            ed_async_scanf(
                vis_offset + 1, 
                3, (2 * vis_offset) - 1, 
                currentPath, 
                strlen(currentPath), 
                &stepIndex
            );
            
        }
        
        hal_inp_updateKeyboard();
    }while(!hal_inp_isKeyPressed(HAL_KEY_ESC));

    if(currPathDirectory) 
        hal_fs_freeDirectory(currPathDirectory);

    if (
		currentWindow &&
		currentWindow->currentFile
	){
        ed_updateCursor();
    }

    dw_requestRenderEvent(DW_RENDER_ALL);
}
