#ifndef SORT_H
#define SORT_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>

#include "../data/data.h"

typedef enum {
    SORT_TYPE_INT,
    SORT_TYPE_FLOAT,
    SORT_TYPE_UINT,
    SORT_TYPE_LONG,
    SORT_TYPE_ULONG,
    SORT_TYPE_STRING,
    SORT_TYPE_CHAR
} SortType;

#if defined(__MSDOS__) || defined(__WATCOMC__)
  #ifndef __cplusplus
    typedef unsigned char bool;
    #define true 1
    #define false 0
  #endif
#else
  #include <stdbool.h>
#endif

/* 
 * Macro to simplify sorting. 
 * Usage: SORT_LIST(myList, MyStructType, myAttribute, SORT_TYPE_INT, true);
 */
#define S_SORT_LIST(list, structType, attribute, type, ascending) \
    s_sortList(list, offsetof(structType, attribute), type, ascending)

void s_sortList(List *list, size_t offset, SortType type, bool ascending);

#endif