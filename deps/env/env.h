#ifndef ENV_H
#define ENV_H

#pragma once

#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE
#endif
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#if defined(__WATCOMC__)
char *strdup(const char *s);
#endif
#include <stdarg.h>
#include <ctype.h>

#define ENV_FILENAME ".env"
#define S_FILENAME "default.cfg"

#define ENV_TYPE_STRING 0
#define ENV_TYPE_INT 1
#define ENV_TYPE_BOOL 2
#define ENV_TYPE_FLOAT 3
#define ENV_TYPE_PATH 4

#define STRING_MAX_LENGTH 255
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

typedef struct ConfigEntry{
  char *key;  
  unsigned char type;
  char *value;
} ConfigEntry;  

typedef struct Config {
  ConfigEntry *entries;  
  int length;
} Config;  

Config *loadEnv();
void displayConf(Config *conf);
void freeConf(Config *conf);
char *getEnv(char *key, const char *defaultValueFormat, ...);

extern Config *config;

#endif
