#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>

#include "list.h"

#define READ_END 0
#define WRITE_END 1
#define INTERVAL 5  // seconds

void parse_vmstat_file(int phase) {
    int fds[2];
    if (pipe(fds) == -1) {
        perror("pipe creation failed");
        return;
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        return;
    }

    if (pid == 0) {
        // Child process: reads /proc/vmstat and writes to pipe
        close(fds[READ_END]);
        while (1) {
            FILE *f = fopen("/proc/vmstat", "r");
            if (!f) {
                perror("Failed to open /proc/vmstat");
                exit(1);
            }

            char line[256];
            while (fgets(line, sizeof(line), f)) {
                write(fds[WRITE_END], line, strlen(line));
            }

            fclose(f);
            sleep(INTERVAL);
        }
        close(fds[WRITE_END]);
        exit(0);
    } else {
        // Parent process: reads from pipe, updates list
        close(fds[WRITE_END]);

        char line[256];
        char key[100];
        unsigned int val;

        while (1) {
            ssize_t n = read(fds[READ_END], line, sizeof(line) - 1);
            if (n <= 0) break;

            line[n] = '\0';  // null terminate the line

            if (sscanf(line, "%99s %u", key, &val) == 2) {
                struct diff d = list_update_or_add_vmstat(key, &val);
                if (phase != INIT_SNAPSHOT && d.statsdiff != 0) {
                    printf("VMSTAT %-20s Δ %8u\n", d.name, d.statsdiff);
                }
            }
        }

        close(fds[READ_END]);
    }
}
