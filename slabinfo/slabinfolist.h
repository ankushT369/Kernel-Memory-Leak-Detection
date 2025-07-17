#ifndef __LIST__
#define __LIST__

#include <stdbool.h>

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


typedef struct
{
    char name[MAX_NAME_LEN];   // slab-cache name
    unsigned int active_objs;  // number of active objects
    unsigned int num_objs;     // total number of objects
    size_t objsize;            // size of each object in bytes
    unsigned int objperslab;   // objects per slab
    unsigned int pagesperslab; // pages per slab

    double ema;
    unsigned int pre_active_objs;
    int monotonic_count;
} slabinfo;

typedef struct
{
    int active_objs_diff; // number of active object difference
    int num_objs_diff;    // total number of objects difference
} diff;

#endif // __LIST__