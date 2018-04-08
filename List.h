#ifndef LIST_H
#define LIST_H
#include <stdbool.h>
#include "typedefs.h"

struct List_s;
struct ListIterator_s;


List * list_create(void);
void list_addValue(List *list, void *value);
void list_destroy(List *list);

ListIterator * listIterator_create(List *list);
void * listIterator_next(ListIterator *iterator);
//listIterator_destroy always returns false so it can be used inline in for loops
bool listIterator_destroy(ListIterator *iterator);
//Use in for loops
#define each_in_list(list, value) ListIterator *it = listIterator_create(list); (value = (decltype(value))listIterator_next(it)) != NULL || listIterator_destroy(it); 
#endif
