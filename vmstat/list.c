#include <stdio.h>

#include "vmstat/list.h"

void list_add_vmstat(struct vmstat *new_stat) {
    new_stat->list_head.next = vmstat_head.next;
    new_stat->list_head.prev = &vmstat_head;
    vmstat_head.next->prev = &new_stat->list_head;
    vmstat_head.next = &new_stat->list_head;
}

struct vmstat* list_find_vmstat(const char *name) {
    struct list_head *pos;
    for (pos = vmstat_head.next; pos != &vmstat_head; pos = pos->next) {
        struct vmstat *entry = list_entry(pos, struct vmstat, list_head);
        if (strcmp(entry->name, name) == 0) return entry;
    }
    return NULL;
}

struct diff list_update_or_add_vmstat(const char *name, unsigned int new_stats) {
    struct diff d;
    strcpy(d.name, name);
    d.statsdiff = 0;

    struct vmstat *entry = list_find_vmstat(name);
    if (entry) {
        d.statsdiff = new_stats - entry->stats;
        entry->stats = new_stats;
    } else {
        struct vmstat *new_entry = malloc(sizeof(struct vmstat));
        strcpy(new_entry->name, name);
        new_entry->stats = new_stats;
        list_add_vmstat(new_entry);
    }

    return d;
}

