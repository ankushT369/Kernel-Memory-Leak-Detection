#include<stdio.h>
#include<string.h>

#include "kmld.h"
#include "list.h"
//initally taking only /proc/slabinfo
//then growth detection from it solely from there no involvment from /proc/vmstat
//short term memory analysis and then detecting the Top-N growth

void read_slabinfo(int ops) {
    printf("opening file %s\n", FILE_SLABINFO);
    FILE* file = fopen(FILE_SLABINFO, "r");
    if (!file) {
        //perror here for error handling
        return ;
    }
    printf("%s opened successfully \n", FILE_SLABINFO);

    /*
     * line 1: slabinfo - version: 2.1
     * line 2: # name            <active_objs> <num_objs> <objsize> <objperslab> <pagesperslab> : tunables <limit> <batchcount> <sharedfactor> : slabdata <active_slabs> <num_slabs> <sharedavail>
     * line 3: ovl_inode             88     88    720   22    4 : tunables    0    0    0 : slabdata      4      4      0
     * line 4: nf_conntrack_expect      0      0    208   39    2 : tunables    0    0    0 : slabdata      0      0      0
     * so on..
     */

    //we took line buffer to store the above lines
    char line[LINE_BUFFER];

    //skip the above 2 lines trivial for our needs
    fgets(line, sizeof(line), file); //"slabinfo - version" (line 1)
    fgets(line, sizeof(line), file); //header line with field names (line 2)

    //this while-loop parses everytime a new line from the /proc/slabinfo file
    //and it does untill it encounters end of file(end of /proc/slabinfo)
    while (fgets(line, sizeof(line), file)) {
        slabinfo s;

        //parse fields using sscanf
        int matched = sscanf(line, "%s %u %u %zu %u %u",
                             s.name,
                             &s.active_objs,
                             &s.num_objs,
                             &s.objsize,
                             &s.objperslab,
                             &s.pagesperslab);

        //the reason matched == 6 because we are parsing 6 fields
        //as you can see in the upper sscanf()
        if (matched == 6) {
            if (ops == INIT_SNAPSHOT)
                list_add(s); 
            else {
                if (!list_exist(s))
                    list_add(s);
                else {
                    diff d = list_match(s);
                    //print debug info for the change in active_objs and num objs
                    if (d.active_objs_diff != 0 || d.num_objs_diff != 0) {
                        printf("Slab: %-20s | Active Δ: %+6d | Total Δ: %+6d\n",
                            s.name, d.active_objs_diff, d.num_objs_diff);
                    }
                }
            }
        }
    }

    //list_trav();

    //close the file as we have read the content
    fclose(file);
    return ;
}

int main(){
    printf("other printtf stmts in accordance to the format necessary from the proc/slabinfo\n");
    read_slabinfo(INIT_SNAPSHOT);
    while(1) {
        printf("changes since last snapshot:\n");
        read_slabinfo(CHECK_SNAPSHOT);
        sleep(INTERVAL);
    }
    return 0;
}

//not to be done initially :
//parsing /proc/vmstat
//adding an overall growth history
//threshold and alert logic
//gui requirement? or better tui maybe manageable
