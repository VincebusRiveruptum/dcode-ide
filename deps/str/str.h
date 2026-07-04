#ifndef DEPS_STR_H
#define DEPS_STR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#ifdef STANDALONE
#include <conio.h>
#endif

 
#define TYPE_INT 0
#define TYPE_CHAR 1
#define TYPE_FLOAT 2
#define TYPE_DOUBLE 3
#define TYPE_STR 4

char **strsplit(char *str, const char *separator);
char *strinv(char *str);
char *strjoinint(int *arr, size_t arrlen, char *separator);
char *strjoinfloat(float *arr, size_t arrlen, char *separator);
char *strjoindouble(double *arr, size_t arrlen, char *separator);
char *strjoinstr(char **arr, size_t arrlen, char *separator);
char *strjoin(void *arr,unsigned char type, size_t arrlen,  char *separator);
char *strltrim(char *str);
char *strrtrim(char *str);
char *strtrim(char *str);
char *strstrip(char *str);
char *strnslice(char *str, size_t from, size_t step, size_t strlen);

#endif