#ifndef ENV_H
#define ENV_H

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdarg.h>
#include <ctype.h>

#define ENV_FILENAME ".env"
#define CFG_FILENAME "default.cfg"

#define ENV_TYPE_STRING 0
#define ENV_TYPE_INT 1
#define ENV_TYPE_BOOL 2
#define ENV_TYPE_FLOAT 3
#define ENV_TYPE_PATH 4

#define STRING_MAX_LENGTH 255
#define true 1
#define false 0

typedef unsigned char bool;

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