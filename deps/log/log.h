#ifndef ENGINE_LOG_LOG_H
#define ENGINE_LOG_LOG_H

#include <stdarg.h>
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
#endif

#include "../env/env.h"

extern FILE *logFp;
extern unsigned char log_enable;

void log_init();
void log_shutdown();
void logger(const char *format, ...);

#endif
