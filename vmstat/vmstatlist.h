#include <stdio.h>

#define INIT_SNAPSHOT_vm 1
#define CHECK_SNAPSHOT_vm 2

#define READ_END 0
#define WRITE_END 1
#define INTERVAL 5

#define INIT_LIST_HEAD(ptr) do { \
    (ptr)->next = (ptr); (ptr)->prev = (ptr); \
} while (0)

#define list_entry(ptr, type, member) \
    ((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

//declaring this head globally
extern struct list_head vmstat_head;

typedef struct list_head{
    struct list_head *prev, *next;
}list_head;
typedef struct vmstat{
    struct list_head list_head;
    char name[100];
    unsigned int stats;
}vmstat;
/*struct zone{
    struct vmstat vmstat[100];
};*/
typedef struct diffvm{
    char name[100];
    unsigned int statsdiff;
}diffvm;

struct diffvm list_update_or_add_vmstat(const char *name, unsigned int new_stats);
struct vmstat* list_find_vmstat(const char *name);
void list_add_vmstat(struct vmstat *new_stat);
