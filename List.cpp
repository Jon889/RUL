#include "List.h"
#include <stdlib.h>
#include <assert.h>

typedef struct ListItem_s {
    void *value;
    struct ListItem_s *next;
} ListItem;

struct List_s {
    ListItem *firstItem;
    ListItem **lastItem;
};

List *list_create(void) {
    List *list = new List();
    list->lastItem = &list->firstItem;
    return list;
}

void list_addValue(List *list, void *value) {
    ListItem *newItem = new ListItem();
    newItem->value = value;
    *list->lastItem = newItem;
    list->lastItem = &newItem->next;
}

void list_destroy(List *list) {
    ListItem *current = list->firstItem;
    while (current != NULL) {
        ListItem *next = current->next;
        free(current);
        current = next;
    }
    free(list);
}

struct ListIterator_s {
    List *list;
    ListItem *currentItem;
};

ListIterator * listIterator_create(List *list) {
    ListIterator *it = new ListIterator();
    it->list = list;
    it->currentItem = list->firstItem;
    return it;
}
void * listIterator_next(ListIterator *iterator) {
    assert(iterator != NULL);
    if (iterator->currentItem == NULL) {
        return NULL;
    }
    void *returnValue = iterator->currentItem->value;
    iterator->currentItem = iterator->currentItem->next;
    return returnValue;
}

bool listIterator_destroy(ListIterator *iterator) {
    free(iterator);
    return false;//so it can be used inline in for loops
}
