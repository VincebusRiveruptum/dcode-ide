#ifndef ENGINE_LOG_LOG_H
#define ENGINE_LOG_LOG_H

#include <stdarg.h>
#include <STDIO.H>
#include <STDLIB.H>
#include <CONIO.H>
#include <STRING.H>
#include <DOS.H>    
#include <MEM.H>
#include <i86.h>
#include <errno.h>
#include <time.h>

#include "..\ENV\ENV.H"
#include <time.h>

extern FILE *logFp;
extern unsigned char log_enable;

void log_init();
void log_shutdown();
void logger(const char *format, ...);

#endif
