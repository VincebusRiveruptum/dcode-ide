// ENV/CONFIG LAYER

#include "CONFIG.H"
#include <stdlib.h>

Settings settings;

static int getEnvInt(char *key, int defaultVal) {
    char *val = (char*)getEnv(key);
    return val ? atoi(val) : defaultVal;
}

bool cfg_loadConfig(){
    char *ext;

    if(!loadEnv()){
        return false;
    } 

    settings.DEBUG                          = (unsigned char)getEnvInt("DEBUG", 0);
    settings.MAX_FILE_INSTANCE_SIZE         = (unsigned int)getEnvInt("MAX_FILE_INSTANCE_SIZE", MAX_ARENAS);
    settings.MAX_FILE_INSTANCES_OPENED      = (unsigned int)getEnvInt("MAX_FILE_INSTANCES_OPENED", MAX_ARENAS);
    settings.MAX_LINE_LENGTH                = (unsigned int)getEnvInt("MAX_LINE_LENGTH", MAX_FILE_LINE_LENGTH);
    settings.AUTOIDENTING                   = (unsigned char)getEnvInt("AUTOIDENTING", 1);
    settings.AUTOCLOSING                    = (unsigned char)getEnvInt("AUTOCLOSING", 1);
    settings.AUTOBRACES                     = (unsigned char)getEnvInt("AUTOBRACES", 1);
    settings.TAB_INDICATOR                  = (unsigned char)getEnvInt("TAB_INDICATOR", 0);
    settings.TAB_SPACE_INDICATOR            = (unsigned char)getEnvInt("TAB_SPACE_INDICATOR", 0);
    
    memset(settings.DEFAULT_EXTENSION, '\0', 16);
    ext = (char*) getEnv("DEFAULT_EXTENSION");
    if (ext) strcpy(settings.DEFAULT_EXTENSION, ext);

    settings.AUTOSAVE                       = (unsigned char)getEnvInt("AUTOSAVE", 0);
    settings.VIDEO_MODE                     = (unsigned char)getEnvInt("VIDEO_MODE", 0);
    settings.STATUSBAR_COLOR_BG             = (unsigned char)getEnvInt("STATUSBAR_COLOR_BG", COLOR_LIGHT_GRAY);
    settings.STATUSBAR_COLOR_TEXT           = (unsigned char)getEnvInt("STATUSBAR_COLOR_TEXT", COLOR_RED);

    // Hardcoded for now
    settings.clang_colors[DW_RESWORD_NONE]         = (unsigned char)getEnvInt("DW_RESWORD_NONE", COLOR_LIGHT_GRAY); //COLOR_LIGHT_GRAY;
    settings.clang_colors[DW_RESWORD_PREPROCESSOR] = (unsigned char)getEnvInt("DW_RESWORD_PREPROCESSOR", COLOR_LIGHT_RED); //COLOR_LIGHT_RED;
    settings.clang_colors[DW_RESWORD_TYPES]        = (unsigned char)getEnvInt("DW_RESWORD_TYPES", COLOR_LIGHT_CYAN); //COLOR_LIGHT_CYAN;
    settings.clang_colors[DW_RESWORD_INT]          = (unsigned char)getEnvInt("DW_RESWORD_INT", COLOR_LIGHT_BLUE); //COLOR_LIGHT_BLUE;
    settings.clang_colors[DW_RESWORD_FLOAT]        = (unsigned char)getEnvInt("DW_RESWORD_FLOAT", COLOR_LIGHT_BLUE); //COLOR_LIGHT_BLUE;
    settings.clang_colors[DW_RESWORD_CONSTANT]     = (unsigned char)getEnvInt("DW_RESWORD_CONSTANT", COLOR_LIGHT_BLUE); //COLOR_LIGHT_BLUE;
    settings.clang_colors[DW_RESWORD_CHAR]         = (unsigned char)getEnvInt("DW_RESWORD_CHAR", COLOR_LIGHT_BLUE); //COLOR_LIGHT_BLUE;
    settings.clang_colors[DW_RESWORD_EXPRESSION]   = (unsigned char)getEnvInt("DW_RESWORD_EXPRESSION", COLOR_LIGHT_RED); //COLOR_LIGHT_RED;
    settings.clang_colors[DW_RESWORD_STRING]       = (unsigned char)getEnvInt("DW_RESWORD_STRING", COLOR_LIGHT_YELLOW); //COLOR_LIGHT_YELLOW;
    settings.clang_colors[DW_RESWORD_CONTROL_FLOW] = (unsigned char)getEnvInt("DW_RESWORD_CONTROL_FLOW", COLOR_LIGHT_RED); //COLOR_LIGHT_RED;
    settings.clang_colors[DW_RESWORD_DOS_FUNC]     = (unsigned char)getEnvInt("DW_RESWORD_DOS_FUNC", COLOR_LIGHT_CYAN); //COLOR_LIGHT_CYAN;
    settings.clang_colors[DW_RESWORD_STD_FUNC]     = (unsigned char)getEnvInt("DW_RESWORD_STD_FUNC", COLOR_LIGHT_CYAN); //COLOR_LIGHT_CYAN;
    settings.clang_colors[DW_RESWORD_COMMENT]      = (unsigned char)getEnvInt("DW_RESWORD_COMMENT", COLOR_DARK_GRAY); //COLOR_DARK_GRAY;
    settings.clang_colors[DW_RESWORD_FUNCTION]     = (unsigned char)getEnvInt("DW_RESWORD_FUNCTION",COLOR_LIGHT_GREEN); //COLOR_LIGHT_GREEN;
    settings.clang_colors[DW_RESWORD_WIFE]         = COLOR_LIGHT_YELLOW;

    logger("[cfg_loadConfig]: default.cfg loaded successfully");

    return true;
}

