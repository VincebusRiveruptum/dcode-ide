#include "editor.h"
void ed_initConfig(int argc, char *argv[]){
    //f_defaultExtension

    // We will hardcode the default extension until i implement .ENV/CFG LOADING
    if(!cfg_loadConfig()){
        //logger("[ed_initConfig]: Could not load config file!");
        // App exits
        printf("\nCould not load config file!.");
        return ;
    }

    log_init();   
    logger("[ed_initConfig]: %d %s", argc, argv[1]);
    ed_handleArguments(argc, argv);

    v_currentMode = settings.DEFAULT_VIDEO_MODE;

    ed_updateCursor();
}

void ed_handleArguments(int argc, char *argv[]){
    int i;
    // File opening
    logger("[ed_handleArguments]: %d %s", argc, argv[1]);
    
    if(argc > 1 || (argv != NULL && argv[1] != NULL)){
        // Multiple file opening
        for(i=1;i<argc;i++){
            if(!f_openFile(argv[i])){
                logger(
					"[ed_handleArguments]: File %s not found. Falling back to new file.", 
					argv[i]
				);

                f_newFile(argv[i]);
            }
        }
    }else{
        f_newFile(NULL);
    }
}