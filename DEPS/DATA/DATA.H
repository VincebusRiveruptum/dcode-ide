// Generic Simple doubly linked list by vincebus riveruptum. 

#ifndef DATA_H
#define DATA_H

#include <stdlib.h>
#include <stddef.h>

#include "..\MEM\MEM.H"

#include "TYPES.H"

void addToList(List **list, Node *newNode, char *arenaName);
Node *createNode(void *data, char *arenaName);
Node *getNodeByIndex(List **list, int index);
void deleteNodeByIndex(List **list, int index);
void softDeleteByIndex(List **list, int index);

Node *insertByIndex(List **list, Node *newNode, unsigned int index);
Node *pop(List **list);
List *invertList(List **list);
void addGenericNode(List **list, void *data, char *arenaName);
void freeList(List **list);

#endif