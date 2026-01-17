#ifndef DATA_TYPES_H
#define DATA_TYPES_H

typedef struct Node{
    void *data;
    struct Node *prev;
    struct Node *next;
} Node;

typedef struct List{
    struct Node *firstNode;
    struct Node *lastNode;
    int length; 
} List;

typedef unsigned char bool;

#define true 1
#define false 0

#endif
