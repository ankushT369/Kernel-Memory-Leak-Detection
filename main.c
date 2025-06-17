#include<stdio.h>
//initally taking only /proc/slabinfo
//then growth detection from it solely from there no involvment from /proc/vmstat
//short term memory analysis and then detecting the Top-N growth

//file to parse slab allocator info
#define FILE_SLABINFO "/proc/slabinfo"
#define MAX_NAME_LEN 64
//define 5 for now later it can be changed based our needs
#define INTERVAL 5

typedef struct {
    char name[MAX_NAME_LEN];            // slab-cache name
    unsigned int active_objs;           // number of active objects
    unsigned int num_objs;              // total number of objects
    size_t objsize;                     // size of each object in bytes
    unsigned int objperslab;            // objects per slab
    unsigned int pagesperslab;          // pages per slab
} slabinfo;

void read_slabinfo() {

}


int main(){
    printf("other printtf stmts in accordance to the format necessary from the proc/slabinfo");
    //read_slabinfo();

    while (1) {
        read_slabinfo();
        compute_growth();
        print_top_growth();

        sleep(INTERVAL);
    }

    return 0;
}

//not to be done initially :
//parsing /proc/vmstat
//adding an overall growth history
//threshold and alert logic
//gui requirement? or better tui maybe manageable
