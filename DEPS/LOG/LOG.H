#ifndef ENGINE_LOG_LOG_H
#define ENGINE_LOG_LOG_H

#include <stdarg.h>
#include "STD.H"
#include "..\ENV\ENV.H"
#include <time.h>

extern FILE *logFp;

void log_init();
void log_shutdown();
void logger(const char *format, ...);

#endif
