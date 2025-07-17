#include <stdio.h>

#include "vmstatlist.h"
#include "slabinfolist.h"

typedef struct info{
    unsigned int loopcount;
    diff diffofslabinfo; //has active_objs_diff and num_objs_diff which we require the access of
} infoofslabinfo;

typedef struct info1{
    diffvm diffofvmstat;
    char name[256]; // targetting to store only Memcached and nr_free, i guess i should make it doubly linkedelist based approach since only 2 info will be there
    unsigned int Memcached;
    unsigned int nr_free;
} infoofvmstat;

int main(){
    //1st taking all the diffs and structs from respective dirs vmstat and slabinfo
    //2nd keeping a simple logic to track the changes that indicate kernel memory leak and not detecting some usage spike
    //using idea of persistance and then applying the simple logic mentioned at 2nd
    //points to be noted: i guess i have to drop the 3 latest values added into slabinfo struct
    return 0;
}

/*
   targetting the following perspective:
   loop count*active_objs_diff*num_objs_diff also if memcached is high and nr_free is low.
*/
