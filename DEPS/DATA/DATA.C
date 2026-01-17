#include "data.h"

Node *createNode(void *data){
    Node *newNode;
    newNode = (Node*)malloc(sizeof(Node));
    newNode->data = data;
    newNode->prev = NULL;
    newNode->next = NULL;
    return newNode;
}

void addToList(List **list, Node *newNode){
    Node *rec = NULL;

    if((*list) == NULL){
        (*list) = (List*)malloc(sizeof(List));
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

bool checkId(unsigned long id, unsigned long *array){
	return includes(id, array, 65536);
}
/*
unsigned long addId(unsigned long id, unsigned long *array){
	if(checkId(id, array)){
		return id;
	}
	array[index] = id;
	index++;
	return id;
}
*/

// GENERIC
void addGenericNode(List **list, void *data){
	Node *newNode = (Node *)malloc(sizeof(Node));
	newNode->data = data;
	newNode->next = NULL;
	newNode->prev = NULL;

	addToList(list, newNode);
}
