/*
    DCODE Engine by Vincebus Riveruptum

    (C) 2026 Vincebus Riveruptum

    This library is for educational or entertainment purposes only.
    So, there are still some issues, unfortunatelly
    It is not intended for use in production environments.
    */
    
#include "MAIN.H"

int main(int argc, char *argv[]){
    char c;
    int ticks = 0;
    
    endProgram = false;
    // So, the steps are:
    // 1. Set VGA mode 2
    log_init();
    mem_init();
    v_init_video();
    inp_initKeyboard();
    
    logger("[main]: %d %s", argc, argv[1]);
    ed_initConfig(argc, argv);
    //t_initTests();
    
    ed_renderEvent = true;
    
    while(endProgram == false){

        // ACTION KEYS HANDLING
        // This is uses ISR approach, not getch()
		if(inp_isKeyPressed(KEY_ESC)) f_triggerClose();
		if(inp_isKeyPressed(KEY_F11)) {v_cycleVideoModes(); ed_renderEvent = true;}
		if(inp_isKeyDown(KEY_F12)){mem_vis_mem();};

		if(inp_isKeyDown(KEY_HOME)) ed_putCursorStart();
		if(inp_isKeyDown(KEY_END)) ed_putCursorEnd();
        
		if(inp_isKeyDown(KEY_PAGEUP)) ed_putCursorFistLine();
		if(inp_isKeyDown(KEY_PAGEDOWN)) ed_putCursorLastLine();
        
		if(inp_keysPressed(INP_TRIGGER_EDGE, 3, KEY_LCTRL, KEY_LSHIFT, KEY_S)) f_saveFile();

        // Horizontal Word jump
        if(inp_keysPressed(INP_TRIGGER_EDGE, 2, KEY_LCTRL, KEY_RIGHT)) ed_wordJump(ED_WORD_JUMP_PREV);
        if(inp_keysPressed(INP_TRIGGER_EDGE, 2, KEY_LCTRL, KEY_LEFT)) ed_wordJump(ED_WORD_JUMP_NEXT);

        // Line Jumping
        if(inp_keysPressed(INP_TRIGGER_EDGE, 2, KEY_LALT, KEY_UP)) ed_swapLine(ED_LINE_JUMP_UP);
        if(inp_keysPressed(INP_TRIGGER_EDGE, 2, KEY_LALT, KEY_DOWN)) ed_swapLine(ED_LINE_JUMP_DOWN);


		if(inp_isKeyDown(KEY_SPACE)) ed_renderEvent = true;
        
		if(inp_isKeyPressed(KEY_DELETE)) ed_supr();
        
        
		// NEW FILE
		if(inp_keysPressed(INP_TRIGGER_EDGE, 2, KEY_LCTRL, KEY_N)){
			f_newFile(NULL);
			logger("[main]: User created %s ,a new file.", currentFileArena->file->name);
		}
        

        // Hice mi propio editor, ke wea!!
        // CLOSE FILE (Alt+F4)
        if(inp_keysPressed(INP_TRIGGER_EDGE, 2, KEY_LALT, KEY_F4)){
            logger("[main]: User closed file.");
            f_triggerClose();
        }
        
        // Getch approach, why? Because getch() reads and uses DOS routines for handling the keyboard
        // so it translates the input scancode to the correct codepage value.
        if(kbhit()){
            c = getch();
            
            if(c == 0 || (unsigned char)c == 0xE0){
                // CURSOR ARROW HANDLING
                c = getch(); /* Consume extended byte and arrows */

                /*
                    If the cursor is currently inside the text area, we are in TEXT MODE
                    if the cursor sits outside means we are in GUI MODE
                */
                
                if(c == KEY_UP) ed_moveCursor(0, -1);
                if(c == KEY_DOWN) ed_moveCursor(0, 1);
                if(c == KEY_LEFT) ed_moveCursor(-1, 0);
                if(c == KEY_RIGHT) ed_moveCursor(1, 0);
                
            } else {
                /* Ignore action keys based on ASCII values */
                if(c == CHAR_BACKSPACE){
                    ed_backspace();
                }

                if(c == CHAR_ENTER){                    
                    ed_newLine();
                }

				if(!(c == CHAR_ESCAPE ||
					c == CHAR_BACKSPACE ||
					c == CHAR_ENTER ||
					c == CHAR_DELETE)){
						ed_typeChar(c);
						ticks++;
					}
				}
			}

		// Draw statusbar
		ed_statusBar();
        
		//t_drawDebugger();
		ed_updateCursor();

		if(ed_renderEvent == true){
			ed_renderElements();
			ed_renderEvent = false;
		}

		inp_updateKeyboard();
	}

	inp_closeKeyboard();
	dw_cls(textmemptr);
	v_set25Lines();
	mem_shutdown();
	log_shutdown();
	printf("96 Tears...\n");
	return 0;
}








































