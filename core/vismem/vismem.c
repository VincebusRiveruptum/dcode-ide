#include "vismem.h"

// This funciton shows up a chart that shows memory usage.
const int SIZE_RATIO = 200;
const int RECT_WIDTH=76;
const int RECT_HEIGHT=18;
const int RECT_XSTART=2;
const int RECT_YSTART=4;

void mem_vis_mem(){
    bool endvis = false;
    int memArenas = 0;
    
    int i=0,j=0,w=0;
    short curr_color=0;
    int numblocks=0;
    int numbgblocks=0;
    float perc_usage=0;
    unsigned char currchar;

	File *currentFile = NULL;

	if(!currentWindow || !currentWindow->currentFile)
		return;

	currentFile = currentWindow->currentFile;

    // We clear the screen
    dw_cls(textmemptr);
    dw_rectangle(
		textmemptr, 
		RECT_XSTART, 
		RECT_YSTART, 
		RECT_XSTART + RECT_WIDTH, 
		RECT_YSTART + RECT_HEIGHT, 
		COLOR_BLACK, 
		COLOR_WHITE, 
		' ', 
		COLOR_LIGHT_GRAY, 
		COLOR_BLACK, 
		false, 
		DRAW_BORDER_SIMPLE, 
		NULL
	);
    // If we want to draw all the memory map into a 70x20 rectangle then
    // we have to scale down the memory size and offset by some ratio
    // lets divide by 100

    perc_usage = 
		((float)currentFile->arena->offset * 100.0f) / 
		(float)currentFile->arena->size;

    dw_writeBuffer(
		textmemptr,
		"Memory usage visualizer", 
		1,
		1,
		23,
		1, 
		COLOR_LIGHT_RED, 
		COLOR_BLACK
	);

    dw_writeBuffer(
		textmemptr,
		"Opened files: %d", 
		1,
		2,
		20,
		2, 
		COLOR_LIGHT_GRAY, 
		COLOR_BLACK, 
		memArenas
	);

    dw_writeBuffer(
		textmemptr,
		"Current file: %s", 
		21,
		2,
		53,
		2, 
		COLOR_LIGHT_GRAY, 
		COLOR_BLACK, 
		currentFile->name
	);

    dw_writeBuffer(
		textmemptr,
		"File arena max size: 512KB", 
		54,
		2,
		78,
		2, 
		COLOR_LIGHT_GRAY, COLOR_BLACK
	);

    dw_writeBuffer(
		textmemptr,
		"Used memory: %d bytes (%.2f\% used)", 
		1,
		3,
		42,
		3, 
		COLOR_LIGHT_GRAY, 
		COLOR_BLACK, 
		currentFile->arena->offset, 
		(float)perc_usage
	);

    while(endvis == false){
        // Render stuff
        
        //logger("[mem_vis_mem]: Rendering %d arena", i);
        // w does not get reset

        if(!currentFile->arena){
            logger("[mem_vis_mem]: NULL memory arena!");
            continue;
        }

        numblocks = (int)(currentFile->arena->offset / SIZE_RATIO);
        numbgblocks = (int)(currentFile->arena->size / SIZE_RATIO);
        
        // WE DRAW THE MEMORY ARENA CHART
        for(j=0;j<numbgblocks;j++){
            // iF the line to be drawn is overflowing the container
            // rectangle, then we do not draw the block...
            if(w >= (RECT_HEIGHT - RECT_YSTART)) continue;

            // We do a Y step if the block number on the line
            // is more than the width
            if(j % (RECT_WIDTH - RECT_XSTART) == 0 ) w++;

            curr_color = 
				j < numblocks ? COLOR_LIGHT_YELLOW : COLOR_DARK_GRAY;
            currchar =
				j < numblocks ? 0xDB : 0xB1;
            
			dw_charXY_color(
				textmemptr,
				currchar,
				(j % (RECT_WIDTH - RECT_XSTART)) + RECT_XSTART + 1, 
				w + RECT_YSTART,curr_color
			);
        }
    
        //Checks for end loop
        if(hal_inp_isKeyDown(HAL_KEY_ESC)) endvis = true;
        if(!hal_inp_kbhit()){
            hal_inp_getch();
        }
    }
}