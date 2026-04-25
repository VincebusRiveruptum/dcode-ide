// ENV/CONFIG LAYER

#include "CONFIG.H"

Settings settings;

bool cfg_loadConfig(){
    loadEnv();

    settings.DEBUG                          = (bool) getEnv("DEBUG")
    settings.MAX_FILE_INSTANCE_SIZE         = (unsigned int) getEnv("MAX_FILE_INSTANCE_SIZE")
    settings.MAX_FILE_INSTANCES_OPENED      = (unsigned int) getEnv("MAX_FILE_INSTANCES_OPENED")
    settings.MAX_LINE_LENGTH                = (unsigned int) getEnv("MAX_LINE_LENGTH")
    settings.AUTOIDENTING                   = (bool) getEnv("AUTOIDENTING")
    settings.AUTOCLOSING                    = (bool) getEnv("AUTOCLOSING")
    settings.AUTOBRACES                     = (bool) getEnv("AUTOBRACES")
    settings.DEFAULT_EXTENSION              = (char*) getEnv("DEFAULT_EXTENSION")
    settings.AUTOSAVE                       = (bool) getEnv("AUTOSAVE")
    settings.VIDEO_MODE                     = (unsigned char) getEnv("VIDEO_MODE")
    settings.STATUSBAR_COLOR_BG             = (unsigned char) getEnv("STATUSBAR_COLOR_BG")
    settings.STATUSBAS_COLOR_TEXT           = (unsigned char) getEnv("STATUSBAS_COLOR_TEXT")

    freeconf();
}

