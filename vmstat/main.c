#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#include "list.h"

#define BUFFER_SIZE 4096  // Enough for /proc/vmstat

//  Parse a buffer containing vmstat lines, extract key-value pairs, track changes
void parse_vmstat_buffer(const char *buffer, int phase) {
    char key[100];
    unsigned int val;

    const char *line = buffer;

    while (*line != '\0') {
        // Find end of line
        const char *next_line = strchr(line, '\n');
        if (!next_line) break;

        char temp[256];
        int len = next_line - line;
        if (len >= sizeof(temp)) len = sizeof(temp) - 1;

        strncpy(temp, line, len);  // Copy the line
        temp[len] = '\0';          // Null-terminate

        // Extract key and value
        if (sscanf(temp, "%99s %u", key, &val) == 2) {
            struct diff d = list_update_or_add_vmstat(key, val);
            if (phase != INIT_SNAPSHOT && d.statsdiff != 0) {
                printf("VMSTAT %-20s Δ %8u\n", d.name, d.statsdiff);
            }
        }

        line = next_line + 1; // Move to next line
    }
}

//  Child reads /proc/vmstat and sends content through pipe
//  Parent receives buffer and parses it
void fetch_and_parse_vmstat(int phase) {
    int fd[2];
    if (pipe(fd) == -1) {
        perror("pipe failed");
        exit(1);
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork failed");
        exit(1);
    }

    if (pid == 0) {
        // Child process
        close(fd[0]); // Close read end

        FILE *f = fopen("/proc/vmstat", "r");
        if (!f) {
            perror("Failed to open /proc/vmstat");
            close(fd[1]);
            exit(1);
        }

        char buf[BUFFER_SIZE];
        size_t n = fread(buf, 1, sizeof(buf) - 1, f);
        buf[n] = '\0';  // Null-terminate

        write(fd[1], buf, n + 1);  // Send buffer to parent
        fclose(f);
        close(fd[1]);
        exit(0);
    } else {
        // Parent process
        close(fd[1]); // Close write end

        char buf[BUFFER_SIZE];
        ssize_t r = read(fd[0], buf, sizeof(buf));
        if (r > 0) {
            buf[r] = '\0'; // Ensure null-terminated
            parse_vmstat_buffer(buf, phase);
        } else {
            fprintf(stderr, "Failed to read from pipe\n");
        }

        close(fd[0]);
        wait(NULL); // Wait for child to finish
    }
}

int main() {
    INIT_LIST_HEAD(&vmstat_head);  // Initialize the stats list

    printf("Taking initial snapshot...\n");
    fetch_and_parse_vmstat(INIT_SNAPSHOT);  // First snapshot

    while (1) {
        sleep(INTERVAL);  // Wait before next snapshot
        fetch_and_parse_vmstat(CHECK_SNAPSHOT);
    }

    return 0;
}

