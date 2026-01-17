// Generic Simple doubly linked list by vincebus riveruptum. 

#ifndef DATA_H
#define DATA_H

#include <stdlib.h>
#include <stddef.h>

#include "TYPES.H"

void addToList(List **list, Node *newNode);
Node *createNode(void *data);
Node *getNodeByIndex(List **list, int index);
void deleteNodeByIndex(List **list, int index);

Node *insertByIndex(List **list, Node *newNode, unsigned int index);
Node *pop(List **list);
List *invertList(List **list);
void addGenericNode(List **list, void *data);
void freeList(List **list);

#endif