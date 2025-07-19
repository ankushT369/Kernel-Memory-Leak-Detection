#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vmstatlist.h"
struct list_head vmstat_head;

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

struct diffvm list_update_or_add_vmstat(const char *name, unsigned int new_stats) {
    struct diffvm d;
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


void init_vmstat_list()
{
    INIT_LIST_HEAD(&vmstat_head);
}

void parse_vmstat()
{
    FILE *fp = fopen("/proc/vmstat", "r");
    if (!fp)
    {
        perror("fopen /proc/vmstat");
        return;
    }
    char name[128];
    unsigned long long val;
    while (fscanf(fp, "%127s %llu", name, &val) == 2)
    {
        list_update_or_add_vmstat(name, (unsigned int)val);
    }
    fclose(fp);
}

unsigned int get_vmstat(const char *name)
{
    struct vmstat *entry = list_find_vmstat(name);
    return entry ? entry->stats : 0;
}

void show_vmstat_summary()
{
    unsigned int memfree = get_vmstat("nr_free_pages");
    unsigned int reclaim = get_vmstat("nr_slab_reclaimable");
    unsigned int unreclaim = get_vmstat("nr_slab_unreclaimable");

    printf("[VMSTAT] free_pages=%u reclaimable=%u unreclaimable=%u\n",
           memfree, reclaim, unreclaim);
}
