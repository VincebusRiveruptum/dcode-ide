#include "SORT.H" 

static int compareNodes(void *d1, void *d2, size_t offset, SortType type) {
    void *v1 = (char*)d1 + offset;
    void *v2 = (char*)d2 + offset;

    switch (type) {
        case SORT_TYPE_INT:
            return (*(int*)v1 - *(int*)v2);
        case SORT_TYPE_UINT:
            return (*(unsigned int*)v1 > *(unsigned int*)v2) ? 1 : (*(unsigned int*)v1 < *(unsigned int*)v2 ? -1 : 0);
        case SORT_TYPE_FLOAT:
            return (*(float*)v1 > *(float*)v2) ? 1 : (*(float*)v1 < *(float*)v2 ? -1 : 0);
        case SORT_TYPE_LONG:
            return (*(long*)v1 > *(long*)v2) ? 1 : (*(long*)v1 < *(long*)v2 ? -1 : 0);
        case SORT_TYPE_ULONG:
            return (*(unsigned long*)v1 > *(unsigned long*)v2) ? 1 : (*(unsigned long*)v1 < *(unsigned long*)v2 ? -1 : 0);
        case SORT_TYPE_STRING:
            return strcmp(*(char**)v1, *(char**)v2);
        case SORT_TYPE_CHAR:
            return (*(char*)v1 - *(char*)v2);
        default:
            return 0;
    }
}

void s_sortList(List *list, size_t offset, SortType type, bool ascending) {
    Node *i, *j;
    void *tempData;
    bool shouldSwap;
    int cmp;

    if (!list || list->length < 2) {
        return;
    }

    for (i = list->firstNode->next; i != NULL; i = i->next) {
        tempData = i->data;
        j = i->prev;

        while (j != NULL) {
            cmp = compareNodes(j->data, tempData, offset, type);
            shouldSwap = ascending ? (cmp > 0) : (cmp < 0);

            if (shouldSwap) {
                j->next->data = j->data;
                j->data = tempData;
                j = j->prev;
            } else {
                break;
            }
        }
    }
}

#ifdef SORT_STANDALONE

typedef struct {
    int id;
    float score;
    char *name;
} TestItem;

void printList(List *list) {
    Node *curr = list->firstNode;
    printf("List (length %d):\n", list->length);
    while (curr) {
        TestItem *item = (TestItem*)curr->data;
        printf("  [ID: %d, Score: %.2f, Name: %s]\n", item->id, item->score, item->name);
        curr = curr->next;
    }
    printf("\n");
}

TestItem* createItem(int id, float score, char *name) {
    TestItem *item = malloc(sizeof(TestItem));
    item->id = id;
    item->score = score;
    item->name = strdup(name);
    return item;
}

int main() {
    List *list = NULL;
    
    addGenericNode(&list, createItem(3, 85.5f, "Charlie"));
    addGenericNode(&list, createItem(1, 92.0f, "Gamma"));
    addGenericNode(&list, createItem(4, 78.2f, "Delta"));
    addGenericNode(&list, createItem(2, 88.8f, "Bravo"));

    printf("Original List:\n");
    printList(list);

    printf("Sorting by ID (int, ascending):\n");
    S_SORT_LIST(list, TestItem, id, SORT_TYPE_INT, true);
    printList(list);
        
    printf("Sorting by Score (float, descending):\n");
    S_SORT_LIST(list, TestItem, score, SORT_TYPE_FLOAT, false);
    printList(list);
        
    printf("Sorting by Name (string, ascending):\n");
    S_SORT_LIST(list, TestItem, name, SORT_TYPE_STRING, true);
    printList(list);
    

    // Clean up would be good but for a quick test it's fine
    return 0;
}

#endif