#ifndef DATA_TYPES_H
#define DATA_TYPES_H

#if defined(__MSDOS__) || defined(__WATCOMC__)
  #ifndef __cplusplus
    typedef unsigned char bool;
    #define true 1
    #define false 0
  #endif
#else
  #include <stdbool.h>
#endif

typedef struct Node{
    void *data;
    unsigned char isDeleted; //bool
    struct Node *prev;
    struct Node *next;
} Node;

typedef struct List{
    struct Node *firstNode;
    struct Node *lastNode;
    int length; 
} List;

#endif