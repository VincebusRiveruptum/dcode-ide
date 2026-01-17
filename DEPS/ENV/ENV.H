#ifndef ENV_H
#define ENV_H

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <ctype.h>

#include "TYPES.H"

#define ENV_FILENAME ".env"
#define CFG_FILENAME "config.cfg"

Config *loadEnv();
void displayConf(Config *conf);
void freeConf(Config *conf);
void *getEnv(char *key);

extern Config *config;

#endif