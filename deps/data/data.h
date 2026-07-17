// Generic Simple doubly linked list by vincebus riveruptum. 

#ifndef DATA_H
#define DATA_H

#include <stdlib.h>
#include <stddef.h>

#include "../mem/mem.h"

#include "types.h"

void _addToList(List **list, Node *newNode, MemoryArena *arena);
Node *createNode(void *data, char *arenaName);
Node *getNodeByIndex(List **list, int index);
void deleteNodeByIndex(List **list, int index);
void softDeleteByIndex(List **list, int index);

Node *insertByIndex(List **list, Node *newNode, unsigned int index);
Node *pop(List **list);
List *invertList(List **list);
List *createList(MemoryArena *arena);
void addGenericNode(List **list, void *data, MemoryArena *arena);
Node *insertGenericNode(List **list, void *data, MemoryArena *arena, unsigned int index);
void deleteNodeByPtr(List **list, void *ptr);

void freeList(List **list);

#endif