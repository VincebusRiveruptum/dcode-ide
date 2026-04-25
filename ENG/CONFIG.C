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
    
    memset(settings.DEFAULT_EXTENSION, '\0', 16);
    ext = (char*) getEnv("DEFAULT_EXTENSION");
    if (ext) strcpy(settings.DEFAULT_EXTENSION, ext);

    settings.AUTOSAVE                       = (unsigned char)getEnvInt("AUTOSAVE", 0);
    settings.VIDEO_MODE                     = (unsigned char)getEnvInt("VIDEO_MODE", 0);
    settings.STATUSBAR_COLOR_BG             = (unsigned char)getEnvInt("STATUSBAR_COLOR_BG", COLOR_LIGHT_GRAY);
    settings.STATUSBAR_COLOR_TEXT           = (unsigned char)getEnvInt("STATUSBAR_COLOR_TEXT", COLOR_RED);

    logger("[cfg_loadConfig]: default.cfg loaded successfully");

    return true;
}

