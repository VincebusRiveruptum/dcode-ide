#ifndef ENG_CONFIG_H
#define ENG_CONFIG_H

#include "..\APP\MAIN.H"

typedef struct Settings{
    // EDITOR SETTING
    bool DEBUG;
    unsigned int MAX_FILE_INSTANCE_SIZE;
    unsigned int MAX_FILE_INSTANCES_OPENED;

    unsigned int MAX_LINE_LENGTH;

    bool AUTOIDENTING;
    bool AUTOCLOSING;
    bool AUTOBRACES;

    char DEFAULT_EXTENSION[16];
    bool AUTOSAVE;
    bool MOUSE;
    // VIDEO SETTINGS
    // 1 = default,   2= 43 lines, 3=50 lines
    unsigned char VIDEO_MODE;
    // CUSTOMIZATION
    unsigned char STATUSBAR_COLOR_BG;
    unsigned char STATUSBAS_COLOR_TEXT;
}Settings;

extern Settings settings;

bool cfg_loadConfig();

#endif