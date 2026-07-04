#ifndef APP_MAIN_H
#define APP_MAIN_H

// DEPS (Generic/Portable)
#include "..\DEPS\DATA\DATA.H"
#include "..\DEPS\ENV\ENV.H"
#include "..\DEPS\LOG\LOG.H"
#include "..\DEPS\MEM\MEM.H"
#include "..\DEPS\SORT\SORT.H"
#include "..\DEPS\EXT\VSNPRNTF.H"

// HAL (Hardware Abstraction Layer)
#include "..\HAL\hal_vid.h"
#include "..\HAL\hal_inp.h"
#include "..\HAL\hal_fs.h"

// PLATFORM SPECIFIC (DOS DRIVERS)
#include "..\PLATFORM\DOS\INPUT\INPUT.H"
#include "..\PLATFORM\DOS\VIDEO\VIDEO.H"
#include "..\PLATFORM\DOS\FS\FS.H"

// CORE ENGINE
#include "..\CORE\STD.H"
#include "..\CORE\CONFIG\CONFIG.H"
#include "..\CORE\DRAW\DRAW.H"
#include "..\CORE\EDITOR\EDITOR.H"
#include "..\CORE\FILES\FILES.H"
#include "..\CORE\TEST\TEST.H"
#include "..\CORE\VISMEM\VISMEM.H"

#endif
