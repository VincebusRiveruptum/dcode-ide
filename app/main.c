/**
	DCODE Engine by Vincebus Riveruptum

	(C) 2026 Vincebus Riveruptum

	This library is for educational or entertainment purposes only.
	So, there are still some issues, unfortunatelly
	It is not intended for use in production environments.
*/   

#include "main.h"

int main(int argc, char *argv[]){
	char c;
	int ticks = 0;
    
	endProgram = false;
	
	ed_initConfig(argc, argv);
    
	hal_inp_initKeyboard();
	hal_vid_init();

	if (argc > 1) {
		logger("[main]: %d %s", argc, argv[1]);
	} else {
		logger("[main]: %d (no args)", argc);
	}

    dw_requestRenderEvent(DW_RENDER_ALL);
	
	while(endProgram == false){
		ed_prepareSelectionTool();
		
		// SEARCH TOOl
		ed_prepareSearchTool();
		        
		// Switch files
		f_prepareFileNavDialog();

		// Split window (Ctrl + \)
		if(hal_inp_keysPressed(HAL_INP_TRIGGER_EDGE, 2, HAL_KEY_LCTRL, HAL_KEY_BACKSLASH)) 
			f_splitWindow();

		// Cycle active window split (Ctrl + W)
		if(hal_inp_keysPressed(HAL_INP_TRIGGER_EDGE, 2, HAL_KEY_LCTRL, HAL_KEY_W))
			f_cycleActiveWindow();


		// ACTION KEYS HANDLING
		// This is uses ISR approach, not getch()
		if(hal_inp_isKeyPressed(HAL_KEY_F11))
			hal_vid_cycleVideoModes();		

		if(hal_inp_isKeyPressed(HAL_KEY_F12))
			mem_vis_mem();
		

		if(hal_inp_isKeyPressed(HAL_KEY_HOME)) 
			ed_putCursorStart();
		if(hal_inp_isKeyPressed(HAL_KEY_END)) 
			ed_putCursorEnd();
        
		if(hal_inp_isKeyPressed(HAL_KEY_PAGEUP)) 
			ed_putCursorFistLine();
		if(hal_inp_isKeyPressed(HAL_KEY_PAGEDOWN)) 
			ed_putCursorLastLine();

		// Horizontal Word jump
		if(hal_inp_keysPressed(HAL_INP_TRIGGER_EDGE, 2, HAL_KEY_LCTRL, HAL_KEY_RIGHT)) 
			ed_wordJump(ED_WORD_JUMP_NEXT);
		if(hal_inp_keysPressed(HAL_INP_TRIGGER_EDGE, 2, HAL_KEY_LCTRL, HAL_KEY_LEFT)) 
			ed_wordJump(ED_WORD_JUMP_PREV);

		// Line swapping
		if(hal_inp_keysPressed(HAL_INP_TRIGGER_EDGE, 2, HAL_KEY_LALT, HAL_KEY_UP)) 
			ed_swapLine(ED_LINE_JUMP_UP);
		if(hal_inp_keysPressed(HAL_INP_TRIGGER_EDGE, 2, HAL_KEY_LALT, HAL_KEY_DOWN)) 
			ed_swapLine(ED_LINE_JUMP_DOWN);

		// SPACE
		if(hal_inp_isKeyDown(HAL_KEY_SPACE)) dw_renderEvent = true;
        
		// DELETE (forward)
		if(hal_inp_isKeyPressed(HAL_KEY_DELETE)) ed_supr();

		/* FILE OPERATIONS */
		
		// CLOSE APP
		if(hal_inp_isKeyPressed(HAL_KEY_ESC)) 
			f_triggerClose(true);
		
		// NEW FILE
		if(hal_inp_keysPressed(HAL_INP_TRIGGER_EDGE, 2, HAL_KEY_LCTRL, HAL_KEY_N)){
			f_newFile(NULL);
			logger(
				"[main]: User created %s ,a new file.", 
				currentWindow->currentFile->name);
		}
		
		// OPEN FILE 
		if(hal_inp_keysPressed(HAL_INP_TRIGGER_EDGE, 2, HAL_KEY_LCTRL, HAL_KEY_O))
			f_quickOpenFileDialog();

		// CLOSE FILE (Alt+F4)
		if(hal_inp_keysPressed(HAL_INP_TRIGGER_EDGE, 2, HAL_KEY_LALT, HAL_KEY_F4)){
			logger("[main]: User closed file.");
			f_triggerClose(false);
		}

		// SAVE FILE
		if(hal_inp_keysPressed(
			HAL_INP_TRIGGER_EDGE, 
			3, 
			HAL_KEY_LCTRL, 
			HAL_KEY_LSHIFT, 
			HAL_KEY_S
		)) 
			f_saveFile();
 
		// SHELL SPAWN
		if(hal_inp_isKeyPressed(HAL_KEY_F9)) 
			ed_shellSpawn();
 
		ed_resetActity();
		
		// Getch approach, why? Because getch() reads and uses DOS routines for handling the keyboard
		// so it translates the input scancode to the correct codepage value.

		if(hal_inp_kbhit()){
			c = hal_inp_getch();
            
			if(c == 0 || (unsigned char)c == 0xE0){
				// CURSOR ARROW HANDLING
				c = hal_inp_getch(); /* Consume extended byte and arrows */

				/*
					If the cursor is currently inside the text area, we are in TEXT MODE
					if the cursor sits outside means we are in GUI MODE
                */
                
				if(c == HAL_KEY_UP) ed_moveCursor(0, -1);
				if(c == HAL_KEY_DOWN) ed_moveCursor(0, 1);
				if(c == HAL_KEY_LEFT) ed_moveCursor(-1, 0);
				if(c == HAL_KEY_RIGHT) ed_moveCursor(1, 0);
                
			} else {
				/* Ignore action keys based on ASCII values */
				if (c >= 32 || c == HAL_CHAR_TAB) {
					ed_typeChar(c);
					ticks++;
				} else if (c == HAL_CHAR_BACKSPACE) {
					ed_backspace();
				} else if (c == HAL_CHAR_ENTER) {
					ed_newLine();
				}
			}
		}

		if(currentWindow && currentWindow->currentFile){
			ed_handleSelection();
		}

		// Independent from render-event. as it always poll
		// globalAuxTimer if there is any timer activated.

		dw_renderEventDispatcher();
		
		hal_inp_updateKeyboard();
	}

	hal_inp_closeKeyboard();
	dw_cls(textmemptr);
	hal_vid_set25Lines();
	log_shutdown();
	printf("96 Tears...\n");
	return 0;
}