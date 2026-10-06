#ifndef TUI_STD_H
#define TUI_STD_H

#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE
#endif
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(__WATCOMC__)
char *strdup(const char *s);
#endif
#include <errno.h>
#include <time.h>

#if defined(__MSDOS__) || defined(__WATCOMC__)
#include <conio.h>
#include <dos.h>
#include <i86.h>
#undef _NO_EXT_KEYS
#include <process.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#endif

#ifndef BOOL_DEFINED
#define BOOL_DEFINED
#if defined(__MSDOS__) || defined(__WATCOMC__)
  #ifndef __cplusplus
    typedef unsigned char bool;
    #define true 1
    #define false 0
  #endif
#else
  #include <stdbool.h>
#endif
#endif

#endif
