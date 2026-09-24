#ifndef ENG_SETTINGS_H
#define ENG_SETTINGS_H

#include "../../app/main.h"

typedef struct Settings{
    // EDITOR SETTING
    unsigned char DEBUG;
    unsigned long MAX_FILE_INSTANCE_SIZE;
    unsigned long MAX_FILE_INSTANCES_OPENED;

    unsigned int FILE_RESIZE;
    unsigned int FILE_HEADROOM;
    unsigned int MAX_LINE_LENGTH;

    unsigned char AUTOIDENTING;
    unsigned char AUTOCLOSING;
    unsigned char AUTOBRACES;
    unsigned char TAB_INDICATOR;
    unsigned char TAB_SPACE_INDICATOR;

    char *DEFAULT_EXTENSION;
    unsigned char AUTOSAVE;
    unsigned char MOUSE;

    unsigned char DEFAULT_VIDEO_MODE;
    // CUSTOMIZATION
    unsigned char STATUSBAR_COLOR_BG;
    unsigned char STATUSBAR_COLOR_TEXT;

    unsigned char clang_colors[32];
}Settings;

extern Settings settings;

bool cfg_loadSettings();

#endif