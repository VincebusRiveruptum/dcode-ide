
#ifndef DATA_TYPES_H
#define DATA_TYPES_H

typedef enum ListDirection {
  DATA_TRAVEL_UNSET,
  DATA_TRAVEL_BACKWARDS,
  DATA_TRAVEL_ONWARDS
} ListDirection;

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