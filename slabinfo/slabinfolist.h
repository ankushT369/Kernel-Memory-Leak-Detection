#ifndef __LIST__
#define __LIST__

#include <stdbool.h>
#include <stddef.h>
#include <unistd.h>

// file to parse slab allocator info
#define FILE_SLABINFO "/proc/slabinfo"
#define MAX_NAME_LEN 64
// define 5 seconds for now later it can be changed based our needs
#define INTERVAL 5
#define MAX_SLABS 1024
#define LINE_BUFFER 256

#define INIT_SNAPSHOT 1
#define CHECK_SNAPSHOT 2

// command line arguments
#define FLAG_DIFF 0
#define FLAG_ALL 1
#define FLAG_TOP 2
#define FLAG_INVALID -1

typedef struct list list;

typedef struct
{
    char name[MAX_NAME_LEN];   // slab-cache name
    unsigned int active_objs;  // number of active objects
    unsigned int num_objs;     // total number of objects
    size_t objsize;            // size of each object in bytes
    unsigned int objperslab;   // objects per slab
    unsigned int pagesperslab; // pages per slab

    double ema;
    unsigned int prev_active_objs;
    int monotonic_count;
    float growth;
    unsigned int baseline_active_objs;  // Starting point for long-term analysis
} slabinfo;

typedef struct
{
    int active_objs_diff; // number of active object difference
    int num_objs_diff;    // total number of objects difference
} diff;

//struct list stores the slabinfo. Instread of
//fixed sized array we are taking a double linkedlist
typedef struct list{
    slabinfo* slab;
    list *prev;
    list *next; 
}list;

//linkedlist functions
list* list_add(slabinfo new_slab);
bool list_exist(slabinfo target);
void list_trav(void);
diff list_match(slabinfo target);
void list_remove(slabinfo target);
void list_del(void);
int list_cnt(void);
void init_slab_list();
void parse_slabinfo(void);

//exposing head pointer
list *get_slab_list_head(void);

#endif // __LIST__