// ENV/CONFIG LAYER

#include "CONFIG.H"

Settings settings;

bool cfg_loadConfig(){
    if(!loadEnv()){
        return false;
    } 

    settings.DEBUG                          = (bool) getEnv("DEBUG") || 0;
    settings.MAX_FILE_INSTANCE_SIZE         = (unsigned int) getEnv("MAX_FILE_INSTANCE_SIZE") || MAX_ARENAS;
    settings.MAX_FILE_INSTANCES_OPENED      = (unsigned int) getEnv("MAX_FILE_INSTANCES_OPENED") || MAX_ARENAS;
    settings.MAX_LINE_LENGTH                = (unsigned int) getEnv("MAX_LINE_LENGTH") || MAX_FILE_LINE_LENGTH;
    settings.AUTOIDENTING                   = (bool) getEnv("AUTOIDENTING") || 1;
    settings.AUTOCLOSING                    = (bool) getEnv("AUTOCLOSING") || 1;
    settings.AUTOBRACES                     = (bool) getEnv("AUTOBRACES") || 1;
    settings.DEFAULT_EXTENSION              = (char*) getEnv("DEFAULT_EXTENSION");
    settings.AUTOSAVE                       = (bool) getEnv("AUTOSAVE") || 0;
    settings.VIDEO_MODE                     = (unsigned char) getEnv("VIDEO_MODE"); || 0;
    settings.STATUSBAR_COLOR_BG             = (unsigned char) getEnv("STATUSBAR_COLOR_BG") || COLOR_LIGHT_GRAY;
    settings.STATUSBAS_COLOR_TEXT           = (unsigned char) getEnv("STATUSBAS_COLOR_TEXT") || COLOR_DARK_RED;

    freeconf();

    logger("[cfg_loadConfig]: default.cfg loaded successfully");

    return true;
}

