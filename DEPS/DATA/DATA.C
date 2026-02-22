#include "data.h"

Node *createNode(void *data, char *arenaName){
    Node *newNode;

    if(arenaName == NULL){
        newNode = (Node*)malloc(sizeof(Node));
    }else{
        newNode = (Node*)mem_arena_alloc(NULL, arenaName, sizeof(Node));
    }
    
    newNode->data = data;
    newNode->isDeleted = false;
    newNode->prev = NULL;
    newNode->next = NULL;
    return newNode;
}

void addToList(List **list, Node *newNode, char *arenaName){
    Node *rec = NULL;
    if((*list) == NULL){
        if(arenaName == NULL){
            (*list) = (List*)malloc(sizeof(List));
        }else{
            (*list) = (List*)mem_arena_alloc(NULL, arenaName, sizeof(List));
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

    if((*list) != NULL && ((index - 1) < (*list)->length)){        
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

    if((*list) != NULL && (*list)->lastNode != NULL){
        popped = (*list)->lastNode;
        (*list)->lastNode = (*list)->lastNode->prev;
        (*list)->lastNode->next = NULL;
        (*list)->length--;

        popped->next = NULL;
        popped->prev = NULL;
        return popped;
    }
    return NULL;
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

// GENERIC
void addGenericNode(List **list, void *data, char *arenaName, MemoryArena *arena){
	Node *newNode = NULL;

    if(arena != NULL){
        newNode = (Node*)mem_arena_alloc(arena, NULL, sizeof(Node));
    }else if (arenaName != NULL) {
        newNode = (Node*)mem_arena_alloc(NULL, arenaName, sizeof(Node));
    }else{
        newNode = (Node*)malloc(sizeof(Node));
    }

	newNode->data = data;
	newNode->next = NULL;
	newNode->prev = NULL;
    newNode->isDeleted = false;

	addToList(list, newNode, arenaName);
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
    if(index > (*list)->length){
        logger("[data/insertGenericNode]: index %d out of bounds (length %d)", index, (*list)->length);
        return NULL;
    }

    newNode = (Node*)mem_arena_alloc(arena, NULL, sizeof(Node));
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
    } else if(index == (*list)->length){
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
