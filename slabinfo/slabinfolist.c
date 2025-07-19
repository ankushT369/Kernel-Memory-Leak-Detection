//list.c file contains all the linkedlist operations
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "slabinfolist.h"

//global head pointer and list size tracker
static list* head = NULL;
static int list_size = 0;

//function to compare two slabinfo structs (based on name)
bool slabinfo_equal(slabinfo a, slabinfo b) {
    return strcmp(a.name, b.name) == 0;
}

//add new node to the linkedlist
list* list_add(slabinfo new_slab) {
    list* new_node = (list*)malloc(sizeof(list));
    if (!new_node) {
        perror("Failed to allocate memory for new node");
        return NULL;
    }

    new_node->slab = (slabinfo*)malloc(sizeof(slabinfo));
    if (!new_node->slab) {
        perror("Failed to allocate memory for slabinfo");
        free(new_node);
        return NULL;
    }

    *(new_node->slab) = new_slab;
    new_node->next = head;
    new_node->prev = NULL;

    if (head)
        head->prev = new_node;

    head = new_node;
    list_size++;

    return new_node;
}

//check if theres any node is present in the linkedlist or not 
bool list_exist(slabinfo target) {
    list* temp = head;
    while (temp) {
        if (slabinfo_equal(*(temp->slab), target))
            return true;
        temp = temp->next;
    }
    return false;
}

//traverse the linkedlist
void list_trav() {
    list* temp = head;
    while (temp) {
        printf("Name: %-20s Active: %-6u Total: %-6u ObjSize: %-4zu Obj/Slab: %-4u Pages/Slab: %-2u\n",
               temp->slab->name,
               temp->slab->active_objs,
               temp->slab->num_objs,
               temp->slab->objsize,
               temp->slab->objperslab,
               temp->slab->pagesperslab);
        temp = temp->next;
    }
}

//match the linkedlist node
diff list_match(slabinfo target) {
    list* temp = head;
    diff result = {0};

    while (temp) {
        if (slabinfo_equal(*(temp->slab), target)) {
            //match found based on name
            result.active_objs_diff = (int)target.active_objs - (int)temp->slab->active_objs;
            result.num_objs_diff = (int)target.num_objs - (int)temp->slab->num_objs;

            return result;
        }
        temp = temp->next;
    }

    printf("No match found for '%s'.\n", target.name);
    return result;
}

//remove a node from linkedlist
void list_remove(slabinfo target) {
    list* temp = head;
    while (temp) {
        if (slabinfo_equal(*(temp->slab), target)) {
            if (temp->prev)
                temp->prev->next = temp->next;
            else
                head = temp->next;

            if (temp->next)
                temp->next->prev = temp->prev;

            free(temp->slab);
            free(temp);
            list_size--;

            printf("Removed slab: %s\n", target.name);
            return;
        }
        temp = temp->next;
    }
    printf("Slab not found for removal.\n");
}

//delete the whole linkedlist
void list_del() {
    list* temp = head;
    while (temp) {
        list* next = temp->next;
        free(temp->slab);
        free(temp);
        temp = next;
    }
    head = NULL;
    list_size = 0;
}

//returns the total number of nodes in the
//linkedlist
int list_cnt() {
    return list_size;
}

// Accessor for other modules
list *get_slab_list_head()
{
    return head;
}

void init_slab_list()
{
    // Initialize internal linked list (head is already global in your code)
    list_del(); // if needed to reset state
}

void parse_slabinfo()
{
    FILE *file = fopen(FILE_SLABINFO, "r");
    if (!file) {
        perror("cannot open /proc/slabinfo");
        return;
    }

    char line[LINE_BUFFER];
    slabinfo s;

    // skip first two lines (headers)
    fgets(line, sizeof(line), file);
    fgets(line, sizeof(line), file);

    // read each slab line
    while (fgets(line, sizeof(line), file)) {
        int matched = sscanf(line, "%s %u %u %zu %u %u",
                            s.name, &s.active_objs, &s.num_objs,
                            &s.objsize, &s.objperslab, &s.pagesperslab);
        
        if (matched != 6)
            continue;

        if (!list_exist(s)) {
            list_add(s);
        } else {
            // Update existing slab
            diff d = list_match(s);
        }
    }

    fclose(file);
}
