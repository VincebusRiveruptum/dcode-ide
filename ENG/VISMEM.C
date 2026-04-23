
// This funciton shows up a chart that shows memory usage.
const int SIZE_RATIO = 100;
const int RECT_WIDTH=70;
const int RECT_HEIGHT=20;
const int RECT_XSTART=4;
const int RECT_YSTART=1;

void mem_vis_mem(){
    bool endvis = false;
    int memArenas = 0;
    FileArena *farenaptr = NULL;
    
    int i=0,j=0,w=0;
    char color=0,curr_color=0;
    int numblocks=0;
    int numbgblocks=0;
    int currOffset=0;
    
    // We clear the screen
    dw_cls();
    dw_rectangle();
    // If we want to draw all the memory map into a 70x20 rectangle then
    // we have to scale down the memory size and offset by some ratio
    // lets divide by 100

    // Get arenas
    do{
        farenaptr = fileList[i];
        if(farenaptr)memArenas++;
    }while(farenaptr);

    // Main loop
    farenaptr = NULL;

    while(!endvis){
        // Render stuff
        for(i=0; i < memArenas; i++){
            color = (char)i;
            farenaptr = fileList[i];
            
            if(!farneaptr->arena){
                logger("[mem_vis_mem]: NULL memory arena!");
                continue;
            }

            numblocks = farenaptr->arena->offset / SIZE_RATIO;
            numbgblocks = farenaptr->arena->size / SIZE_RATIO;
            
            // WE DRAW THE MEMORY ARENA CHART
            for(j=currOffset;j<numbgblocks;j++){
                if(j % RECT_WIDTH == 0 ) w++;

                curr_color = j < numblocks ? color : COLOR_DARK_GRAY;
                
                dw_charXY(textmemptr,'X',j,w,curr_color);
            }
            
        }
        





        //Checks for end loop
        if(isKeyDown(KEY_ESC)) endvis = true;
    }
}