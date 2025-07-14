#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>

#include "list.h"

void parse_vmstat_file(int phase) {
  int fds[2];
  if(pipe(fds)==-1){
    printf("pipe creation failed");
    return 0;
  }

  pid_t pid = fork();

  static char key[100];
  static unsigned int val;
  //static char line[256]; cant belong here as i guess it will be the pipe buffer between them another missing point 

  if(pid==0){
    close(fds[READ_END]); //closing read end as its handled by the child process itself
  while(1){
      char line[256]; //acting as a buffer in piped
    FILE *f = fopen("/proc/vmstat", "r");
    if (!f) {
        perror("Failed to open /proc/vmstat");
        exit(1);
    }

    
    while(fgets(line,sizeof(line),f)){
      if(sscanf(line,"%99 %u",key,&val)){
        write(fds[1], line, strlen(line));
      }
    fclose(f);
    sleep(INTERVAL);
    }
  }
}
  if(pid==1){
    close(fds[WRITE_END]);//closing the write end as it will be worked here and recieving from it not parent's job
    /*while (fgets(line, sizeof(line), f)) {
        if (sscanf(line, "%99s %u", key, &val) != 2) continue;*/
    while(1){ //what kind of condition to be applied so that it cooperates with piped READ_END ok not here maybe inside
      char line[256];
      while(/*the condition*/){
        char line[256];
        struct diff d = list_update_or_add_vmstat(key, &val);
        if (phase != INIT_SNAPSHOT && d.statsdiff != 0) {
            printf("VMSTAT %-20s Δ %8u\n", d.name, d.statsdiff);
        }
      }
    }
  }
}


int main() {
    INIT_LIST_HEAD(&vmstat_head);

    printf("Taking initial snapshot...\n");
    parse_vmstat_file(INIT_SNAPSHOT);

    while (1) {
        sleep(5);
        printf("Checking for vmstat changes...\n");
        parse_vmstat_file(CHECK_SNAPSHOT);
    }

    return 0;
}
