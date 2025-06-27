#ifndef __LIST__
#define __LIST__

#include <stdbool.h>
#include "kmld.h"

typedef struct list list;

//struct list stores the slabinfo. Instread of
//fixed sized array we are taking a double linkedlist
struct list{
    slabinfo* slab;
    list* prev;
    list* next; 
};

//linkedlist functions
list* list_add(slabinfo);
bool list_exist(slabinfo);
void list_trav();
diff list_match(slabinfo);
void list_remove(slabinfo);
void list_del();
int list_cnt();

#endif // __LIST__
