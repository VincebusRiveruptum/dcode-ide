#ifndef TUI_STD_H
#define TUI_STD_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#if defined(__MSDOS__) || defined(__WATCOMC__)
#include <conio.h>
#include <dos.h>
#include <mem.h>
#include <i86.h>
#include <process.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#endif

#endif
