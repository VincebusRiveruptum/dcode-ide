#ifndef APP_MAIN_H
#define APP_MAIN_H

// DEPS (Generic/Portable)
#include "../deps/data/data.h"
#include "../deps/env/env.h"
#include "../deps/log/log.h"
#include "../deps/mem/mem.h"
#include "../deps/sort/sort.h"
#include "../deps/ext/vsnprntf.h"

// HAL (Hardware Abstraction Layer)
#include "../hal/hal_vid.h"
#include "../hal/hal_inp.h"
#include "../hal/hal_fs.h"

// PLATFORM SPECIFIC (DOS DRIVERS)
#if defined(__MSDOS__) || defined(__WATCOMC__)
#include "../platform/dos/input/input.h"
#include "../platform/dos/video/video.h"
#include "../platform/dos/fs/fs.h"
#endif

// CORE ENGINE
#include "../core/std.h"
#include "../core/config/config.h"
#include "../core/draw/draw.h"
#include "../core/editor/editor.h"
#include "../core/files/files.h"
#include "../core/test/test.h"
#include "../core/vismem/vismem.h"

#endif
