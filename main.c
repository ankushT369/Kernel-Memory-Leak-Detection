#include<stdio.h>
//initally taking only /proc/slabinfo
//then growth detection from it solely from there no involvment from /proc/vmstat
//short term memory analysis and then detecting the Top-N growth

int main(){
  printf("other printtf stmts in accordance to the format necessary from the proc/slabinfo");
    read_slabinfo();

    while (1) {
        sleep(INTERVAL/*#define INTERVAL 5*/);
        read_slabinfo();
        print_top_growth();
    }

    return 0;
}

//not to be done initially :
//parsing /proc/vmstat
//adding an overall growth history
//threshold and alert logic
//gui requirement? or better tui maybe manageable
