#ifndef ENGINE_LOG_LOG_H
#define ENGINE_LOG_LOG_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#if defined(__MSDOS__) || defined(__WATCOMC__)
#include <CONIO.H>
#include <DOS.H>    
#include <MEM.H>
#include <i86.h>
#endif

#include "../ENV/ENV.H"

extern FILE *logFp;
extern unsigned char log_enable;

void log_init();
void log_shutdown();
void logger(const char *format, ...);

#endif
