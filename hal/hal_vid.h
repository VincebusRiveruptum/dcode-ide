#ifndef HAL_VID_H
#define HAL_VID_H

#include "std.h"

#define HAL_VIDEO_BUFFER_SIZE 8192

#define HAL_VID_80X25 1
#define HAL_VID_80X43 2
#define HAL_VID_80X50 3
#define HAL_VID_80X60 4
#define HAL_VID_132X25 5
#define HAL_VID_132X43 6
#define HAL_VID_132X50 7
#define HAL_VID_132X60 8

#define HAL_SHOW_MSG 0
#define HAL_NO_MSG 1

extern unsigned short *textmemptr;      /* Video buffer pointer */
extern unsigned short *editormemptr;    /* Editor buffer */
extern char tempBuffer[HAL_VIDEO_BUFFER_SIZE];

extern unsigned char VIDEO_COLS;
extern unsigned char VIDEO_ROWS;
extern unsigned char currentCursorX;
extern unsigned char currentCursorY;
extern int v_currentMode;

void hal_vid_init(void);
unsigned char hal_vid_setVideoMode(unsigned char mode, unsigned char show_msg);
void hal_vid_close(void);
void hal_vid_clearBuffer(unsigned short *buffer);
void hal_vid_cycleVideoModes(void);
void hal_vid_refresh(void);
void hal_vid_putCursor(unsigned char x, unsigned char y);

/* Low-level register/assembly operations */
void hal_vid_set25Lines(void);
void hal_vid_set43Lines(void);
void hal_vid_set50Lines(void);
void hal_vid_set80x60(void);
void hal_vid_set132x25(void);
void hal_vid_set132x43(void);
void hal_vid_set132x50(void);
void hal_vid_set132x60(void);
unsigned short hal_vid_getVideoBufferSize(void);

#endif
