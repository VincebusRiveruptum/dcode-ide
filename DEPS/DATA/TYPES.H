#ifndef DATA_TYPES_H
#define DATA_TYPES_H

#define true 1
#define false 0

typedef unsigned char bool;

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
