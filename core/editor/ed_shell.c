#include "editor.h"

// Shell spawn.. 
void ed_shellSpawn(){
    char cmd[255];
    char currPath[255];
    char *comspec = NULL;
    File *currentFile = NULL;

    memset(cmd, '\0', 255);
    memset(currPath, '\0', 255);

#if defined(__MSDOS__) || defined(__WATCOMC__)
    comspec = getenv("COMSPEC");
#else
    comspec = getenv("SHELL");
#endif

    currentFile = currentWindow ? currentWindow->currentFile : NULL;

    if(!currentFile) return;
    
    hal_inp_closeKeyboard();

    hal_vid_set25Lines();
    dw_cls(textmemptr);

    // TODO: SAVE FILE

#if defined(__MSDOS__) || defined(__WATCOMC__)
    if (!comspec) comspec = "COMMAND.COM";
#else
    if (!comspec) comspec = "/bin/bash";
#endif

    strncpy(
		currPath, 
		currentFile->name, 
		hal_fs_getFilePath(currentFile->name)
	);

#if defined(__MSDOS__) || defined(__WATCOMC__)
    sprintf(cmd, "cd %s", currPath);

    logger("[ed_shellSpawn]: %s", currPath);

    spawnl(P_WAIT, comspec, comspec, "/K", cmd, NULL);
#else
    sprintf(cmd, "cd %s && %s", currPath, comspec);

    logger("[ed_shellSpawn]: %s", currPath);

    system(cmd);
#endif
    
    hal_inp_initKeyboard();
    hal_inp_clearKeyboardBuffer();
    
    hal_vid_setVideoMode(v_currentMode, false);

    dw_requestRenderEvent(DW_RENDER_ALL);
}
