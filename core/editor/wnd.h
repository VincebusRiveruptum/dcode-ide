// Window implementation

#ifndef ED_WND_H
#define ED_WND_H

#include "../std.h"

typedef enum wndstatus = {
	WND_MAXIMIZED,
	WND_MINIMIZED,
	WND_FLOATING,	
}ed_wndStatus;

typedef struct ed_wnd{
	// Window tabs
	List *fileArenaPtrLst;
	ed_wndStatus wndstatus;
	unsigned int width;
	unsigned int height;
	unsigned int wndIndex;
	bool active;
}ed_wnd;


#endif