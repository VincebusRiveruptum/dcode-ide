// ENV/CONFIG LAYER
#include "config.h"
#include <stdlib.h>

Settings settings;

bool cfg_loadConfig(){
    log_enable = false;

    loadEnv();

    settings.DEBUG                          
        = (unsigned char)atoi(getEnv("DEBUG", "0"));
    
    settings.MAX_FILE_INSTANCE_SIZE         
        = (unsigned int)atoi(getEnv("MAX_FILE_INSTANCE_SIZE","%ld", (MEM_ARENA_512K / 1024))) 
		* 1024 ;
    settings.MAX_FILE_INSTANCES_OPENED      
        = (unsigned int)atoi(getEnv("MAX_FILE_INSTANCES_OPENED", "%d", 32));
    settings.MAX_LINE_LENGTH                
        = (unsigned int)atoi(getEnv("MAX_LINE_LENGTH", "%d", MAX_FILE_LINE_LENGTH));
    settings.AUTOIDENTING                   
        = (unsigned char)atoi(getEnv("AUTOIDENTING", "1"));
    settings.AUTOCLOSING                    
        = (unsigned char)atoi(getEnv("AUTOCLOSING", "1"));
    settings.AUTOBRACES                     
        = (unsigned char)atoi(getEnv("AUTOBRACES", "1"));
    settings.TAB_INDICATOR                  
        = (unsigned char)atoi(getEnv("TAB_INDICATOR", "0"));
    settings.TAB_SPACE_INDICATOR            
        = (unsigned char)atoi(getEnv("TAB_SPACE_INDICATOR", "0"));
    
    settings.DEFAULT_EXTENSION 
        = (char*) getEnv("DEFAULT_EXTENSION" , ".c");

    settings.AUTOSAVE                       
        = (unsigned char)atoi(getEnv("AUTOSAVE", "0"));
    settings.DEFAULT_VIDEO_MODE             
        = (unsigned char)atoi(getEnv("DEFAULT_VIDEO_MODE", "1"));
    
    settings.CLIPBOARD_SIZE             
        = (unsigned char)atoi(getEnv("CLIPBOARD_SIZE", "%ld", (MEM_ARENA_8K / 1024)))
        * 1024;
    // Validate that DEFUALT_VIDEO_MODE is never 0 or ther will be an infite loop when trying to switch video modes.
    if (
        settings.DEFAULT_VIDEO_MODE <= 0 || 
        settings.DEFAULT_VIDEO_MODE > 8)
    settings.DEFAULT_VIDEO_MODE = 1;

    settings.STATUSBAR_COLOR_BG             
        = (unsigned char)atoi(getEnv("STATUSBAR_COLOR_BG", "%d", COLOR_LIGHT_GRAY));
    settings.STATUSBAR_COLOR_TEXT           
        = (unsigned char)atoi(getEnv("STATUSBAR_COLOR_TEXT", "%d", COLOR_RED));

    // Hardcoded for now
    settings.clang_colors[DW_RESWORD_NONE]         
        = (unsigned char)atoi(getEnv("DW_RESWORD_NONE", "%d", COLOR_LIGHT_GRAY));      
    settings.clang_colors[DW_RESWORD_PREPROCESSOR] 
        = (unsigned char)atoi(getEnv("DW_RESWORD_PREPROCESSOR", "%d", COLOR_LIGHT_RED));
    settings.clang_colors[DW_RESWORD_TYPES]        
        = (unsigned char)atoi(getEnv("DW_RESWORD_TYPES", "%d", COLOR_LIGHT_CYAN)); 
    settings.clang_colors[DW_RESWORD_INT]          
        = (unsigned char)atoi(getEnv("DW_RESWORD_INT", "%d", COLOR_LIGHT_BLUE)); 
    settings.clang_colors[DW_RESWORD_FLOAT]        
        = (unsigned char)atoi(getEnv("DW_RESWORD_FLOAT", "%d", COLOR_LIGHT_BLUE));
    settings.clang_colors[DW_RESWORD_CONSTANT]     
        = (unsigned char)atoi(getEnv("DW_RESWORD_CONSTANT", "%d", COLOR_LIGHT_BLUE));
    settings.clang_colors[DW_RESWORD_CHAR]         
        = (unsigned char)atoi(getEnv("DW_RESWORD_CHAR", "%d", COLOR_LIGHT_BLUE)); 
    settings.clang_colors[DW_RESWORD_EXPRESSION]   
        = (unsigned char)atoi(getEnv("DW_RESWORD_EXPRESSION", "%d", COLOR_LIGHT_RED)); 
    settings.clang_colors[DW_RESWORD_STRING]       
        = (unsigned char)atoi(getEnv("DW_RESWORD_STRING", "%d", COLOR_LIGHT_YELLOW)); 
    settings.clang_colors[DW_RESWORD_CONTROL_FLOW] 
        = (unsigned char)atoi(getEnv("DW_RESWORD_CONTROL_FLOW", "%d", COLOR_LIGHT_RED));
    settings.clang_colors[DW_RESWORD_DOS_FUNC]     
        = (unsigned char)atoi(getEnv("DW_RESWORD_DOS_FUNC", "%d", COLOR_LIGHT_CYAN)); 
    settings.clang_colors[DW_RESWORD_STD_FUNC]     
        = (unsigned char)atoi(getEnv("DW_RESWORD_STD_FUNC", "%d", COLOR_LIGHT_CYAN));
    settings.clang_colors[DW_RESWORD_COMMENT]      
        = (unsigned char)atoi(getEnv("DW_RESWORD_COMMENT", "%d", COLOR_DARK_GRAY)); 
    settings.clang_colors[DW_RESWORD_FUNCTION]     
    = (unsigned char)atoi(getEnv("DW_RESWORD_FUNCTION", "%d",COLOR_LIGHT_GREEN)); 
    
    settings.clang_colors[DW_RESWORD_WIFE]         = COLOR_LIGHT_YELLOW;
    
    log_enable = settings.DEBUG ;
    
    logger("[cfg_loadConfig]: default.cfg loaded successfully");

    return true;
}

