#include "data.h"

List *createList(MemoryArena *arena){
	List *newList = NULL;

	if(arena){
		newList = (List*)mem_arena_alloc(arena, sizeof(List));
	}else{
		newList = (List*)malloc(sizeof(List));
	}

	if(!newList) 
		return NULL;

	newList->firstNode = NULL;
	newList->lastNode = NULL;
	newList->length = 0;

	return newList;
}
// DO NOT USE IF YOU ARE USING ARENAS
void deleteNodeByIndex(List **list, int index){
    int i=0;
    Node *rec = (*list)->firstNode;

    if(rec != NULL && (index - 1 ) == 0){
        (*list)->firstNode = rec->next;
        if((*list)->firstNode != NULL){
            (*list)->firstNode->prev = NULL;
        }

        free((*list)->firstNode->data);
        free((*list)->firstNode);
        return;
    }else{
        while(rec != NULL){
            if(rec->prev != NULL && i == (index - 1)){
                rec->prev->next = rec->next;
            }

            if(rec->next != NULL && i == (index - 1)){
                rec->next->prev = rec->prev;
            }
            i++;
           
            free(rec->data);
            free(rec);
            return;
        }
    }
    return;
}


// This removes a node from a list
// NOTE: This does not free the node data,
// you have to free it after removing the node betwen.
void deleteNodeByPtr(List **list, void *ptr){
    void *nodeData = NULL;
    Node *rec = NULL;
    Node *next = NULL;
    Node *prev = NULL;

    if (!list || !*list) return;

    rec = (*list)->firstNode;
    while(rec != NULL){
        nodeData = rec->data;

        if(nodeData == ptr){
            next = rec->next;
            prev = rec->prev;

            if(prev != NULL){
                prev->next = next;
            } else {
                (*list)->firstNode = next;
            }

            if(next != NULL){
                next->prev = prev;
            } else {
                (*list)->lastNode = prev;
            }

            (*list)->length--;

            free(rec);
            return;
        }
        rec = rec->next;
    }
}

// USE THIS IF YOU ARE USING ARENAS
void softDeleteByIndex(List **list, int index){
    int i=0;
    Node *rec = (*list)->firstNode;

    if(rec != NULL && (index - 1 ) == 0){
        (*list)->firstNode = rec->next;
        if((*list)->firstNode != NULL){
            (*list)->firstNode->prev = NULL;
        }
        rec->isDeleted = true;
        return;
    }else{
        while(rec != NULL){
            if(rec->prev != NULL && i == (index - 1)){
                rec->prev->next = rec->next;
            }

            if(rec->next != NULL && i == (index - 1)){
                rec->next->prev = rec->prev;
            }
            i++;
            rec->isDeleted = true;
            return;
        }
    }
    return;
}

// TODO : TEST THIS FUNCTION
Node *insertByIndex(List **list, Node *newNode, unsigned int index){
    Node *temp = NULL;

    if((*list) != NULL && ((index - 1) < (unsigned int)(*list)->length)){        
        temp = getNodeByIndex(list, index);

        if(temp != NULL){
            newNode->next = temp;
            newNode->prev = temp->prev;
            temp->prev = newNode;
            (*list)->length++;
            return newNode;
        }else{

            return NULL;
        }
    }else{
        return NULL;
    }
}

// TODO : TEST THIS FUNCTION

Node *pop(List **list){
    Node *popped = NULL;

    if(
        !(*list) ||
        !(*list)->firstNode ||
        !(*list)->lastNode ||
        (!(*list)->length) > 0
    )
		return NULL;
	
	popped = (*list)->lastNode;

	(*list)->lastNode = 
		(*list)->lastNode->prev;
	
	if ((*list)->lastNode) {
		(*list)->lastNode->next = NULL;
	} else {
		(*list)->firstNode = NULL;
	}
	
	(*list)->length--;

	popped->next = NULL;
	popped->prev = NULL;
	return popped;

}

// TODO: TEST THIS FUNCTION

List *invertList(List **list){
    Node *rec, *tmp = NULL;
    unsigned int i=0;

    if((*list) != NULL){
        (*list)->firstNode = (*list)->lastNode;       
        rec = (*list)->lastNode;

        do{
            tmp = rec->next;
            rec->next = rec->prev;
            rec->prev = tmp;

            rec = rec->next;
            i++;
        } while(rec != NULL);

        (*list)->lastNode = rec;

        return (*list);
    }
    return NULL;
}


Node *getNodeByIndex(List **list, int index){
    Node *rec;
    int i=0;

    if (list == NULL || *list == NULL) return NULL;
    rec = (*list)->firstNode;

    if(index < (*list)->length){
        while(rec != NULL){
            if(i == index){
                return rec;
            }
            rec = rec->next;
            i++;
        }
    }
    return NULL;
}

// DONT USE THIS FUNCTION IF YOU ARE USING ARENAS
void freeList(List **list){
    Node *current = (*list)->firstNode;
    Node *temp;
    while(current != NULL){
        temp = current;
        current = current->next;
        if(temp->data) free(temp->data);
        free(temp);
    }
    free(*list);
    *list = NULL;
}

//
bool includes(float val, float *arr, size_t n) {
    size_t i = 0;
    for(i = 0; i < n; i++) {
        if(arr[i] == val)
            return true;
    }
    return false;
}

void _addToList(List **list, Node *newNode, MemoryArena *arena){
    Node *rec = NULL;
    if((*list) == NULL){
        if(arena == NULL){
            (*list) = (List*)malloc(sizeof(List));
        }else if (arena){
            (*list) = (List*)mem_arena_alloc(arena, sizeof(List));
        }else{
            return;
        }

        (*list)->firstNode = NULL;
        (*list)->lastNode = NULL;
        (*list)->length = 0;
    }

    if ((*list)->firstNode == NULL){
        (*list)->firstNode = newNode;
    }
    else{
        rec = (*list)->firstNode;
        while (rec->next != NULL){
            rec = rec->next;
        }
        rec->next = newNode;
        (newNode)->prev = rec;
    }
    (*list)->lastNode = newNode;
    (*list)->length++;
}

void addGenericNode(List **list, void *data, MemoryArena *arena){
	Node *newNode = NULL;

    if(arena != NULL){
        newNode = (Node*)mem_arena_alloc(arena, sizeof(Node));
    }else{
        newNode = (Node*)malloc(sizeof(Node));
    }

    if(!newNode) return;

	newNode->data = data;
	newNode->next = NULL;
	newNode->prev = NULL;
    newNode->isDeleted = false;

	_addToList(list, newNode, arena);
}

Node *insertGenericNode(List **list, void *data, MemoryArena *arena, unsigned int index){
    Node *temp = NULL;
    Node *newNode = NULL;

    if(!data) return NULL;
    if(!arena){
        logger("[data/insertGenericNode]: arena is NULL");
        return NULL;
    }
    if((*list) == NULL) {
        logger("[data/insertGenericNode]: list is NULL");
        return NULL;
    }

    /* Check bounds: can insert at any index from 0 to length inclusive (appending) */
    if(index > (unsigned int)(*list)->length){
        logger("[data/insertGenericNode]: index %d out of bounds (length %d)", index, (*list)->length);
        return NULL;
    }

    newNode = (Node*)mem_arena_alloc(arena, sizeof(Node));
    newNode->data = data;
    newNode->isDeleted = false;

    if(index == 0){
        /* Insert at the beginning */
        newNode->next = (*list)->firstNode;
        newNode->prev = NULL;
        if((*list)->firstNode != NULL){
            (*list)->firstNode->prev = newNode;
        }
        (*list)->firstNode = newNode;
        
        /* If list was empty, it's also the last node */
        if((*list)->lastNode == NULL){
            (*list)->lastNode = newNode;
        }
    } else if(index == (unsigned int)(*list)->length){
        /* Append at the end */
        newNode->next = NULL;
        newNode->prev = (*list)->lastNode;
        if((*list)->lastNode != NULL){
            (*list)->lastNode->next = newNode;
        }
        (*list)->lastNode = newNode;
    } else {
        /* Insert in the middle */
        temp = getNodeByIndex(list, index);
        if(temp != NULL){
            newNode->next = temp;
            newNode->prev = temp->prev;
            if(temp->prev != NULL){
                temp->prev->next = newNode;
            }
            temp->prev = newNode;
        } else {
            /* Should not happen due to bounds check, but safe fallback */
            return NULL;
        }
    }

    (*list)->length++;
    return newNode;
}
