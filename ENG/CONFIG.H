#ifndef ENG_CONFIG_H
#define ENG_CONFIG_H

#include "..\APP\MAIN.H"

typedef struct Settings{
    // EDITOR SETTING
    unsigned char DEBUG;
    unsigned int MAX_FILE_INSTANCE_SIZE;
    unsigned int MAX_FILE_INSTANCES_OPENED;

    unsigned int MAX_LINE_LENGTH;

    unsigned char AUTOIDENTING;
    unsigned char AUTOCLOSING;
    unsigned char AUTOBRACES;

    char DEFAULT_EXTENSION[16];
    unsigned char AUTOSAVE;
    unsigned char MOUSE;
    // VIDEO SETTINGS
    // 1 = default,   2= 43 lines, 3=50 lines
    unsigned char VIDEO_MODE;
    // CUSTOMIZATION
    unsigned char STATUSBAR_COLOR_BG;
    unsigned char STATUSBAR_COLOR_TEXT;
}Settings;

extern Settings settings;

bool cfg_loadConfig();

#endif