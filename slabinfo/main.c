#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>


#include "slabinfolist.h"

// Helper to read one line at a time from a pipe(as fgets doesn't work so well in pipe)
int read_line_from_pipe(int fd, char *line) {
    char ch;
    int idx = 0;

    while (read(fd, &ch, 1) > 0) {
        if (ch == '\n') break;
        if (idx < LINE_BUFFER - 1)
            line[idx++] = ch;
    }

    line[idx] = '\0';
    return idx > 0;
}

void print_usage() {
    printf("Usage:\n");
    printf("  ./program -diff             Show differences only\n");
    printf("  ./program -all              Show all items\n");
    printf("  ./program -top <number>     Show top N items\n");
    printf("\nExamples:\n");
    printf("  ./program -top 5\n");
    printf("  ./program -diff\n");
    printf("  ./program -all\n");
}

int parse_arg(int argc, char* argv[]) {
    if (strcmp(argv[1], "-diff") == 0)
        return FLAG_DIFF;

    if (strcmp(argv[1], "-all") == 0)
        return FLAG_ALL;

    if (strcmp(argv[1], "-top") == 0) {
        if (argc < 3) {
            printf("Error: -top requires a number\n");
            exit(1);
        }

        int n;
        if (sscanf(argv[2], "%d", &n) != 1 || n <= 0) {
            printf("Error: -top must be followed by a positive integer\n");
            exit(1);
        }

        return FLAG_TOP;
    }

    return FLAG_INVALID;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        //wrong CLI args print
        print_usage();
        exit(1);
    }

    int flag = FLAG_INVALID;
    flag = parse_arg(argc, argv);

    if (flag == FLAG_INVALID) {
        printf("Error: Unknown argument: %s\n", argv[1]);
        print_usage();
        exit(1);
    }

    int fds[2];  // need 2 buffers for ipc

    if (pipe(fds) == -1) { // optimized line for pipe as well as condition execution
        perror("pipe creation failed");
        exit(1);
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(1);
    }

    if (pid == 0) {
        // Child Process: Slab Reader
        close(fds[0]); // close read end

        while (1) {
            FILE *file = fopen(FILE_SLABINFO, "r");
            if (!file) {
                perror("cannot open /proc/slabinfo");
                sleep(INTERVAL);
                continue;
            }

            char line[LINE_BUFFER];

            // skip first two lines
            fgets(line, sizeof(line), file);
            fgets(line, sizeof(line), file);

            // write each slab line to the pipe
            while (fgets(line, sizeof(line), file))
                write(fds[1], line, strlen(line));
            

            fclose(file);
            sleep(INTERVAL);
        }

    } else {
        // Parent Process: Analyzer
        close(fds[1]); // close write end

        char line[LINE_BUFFER];
        slabinfo s;
        int snapshot_phase = INIT_SNAPSHOT;

        printf("Monitoring slab growth...\n");

        while (1) {
            if (!read_line_from_pipe(fds[0], line))
                continue;

            int matched = sscanf(line, "%s %u %u %zu %u %u",
                                 s.name, &s.active_objs, &s.num_objs,
                                 &s.objsize, &s.objperslab, &s.pagesperslab);

            if (matched != 6)
                continue;

            if (flag == FLAG_ALL) {
                printf("Received slab: %s | Active: %u | Total: %u\n",
                    s.name, s.active_objs, s.num_objs);
            }

            if (snapshot_phase == INIT_SNAPSHOT) {
                list_add(s);
                snapshot_phase = CHECK_SNAPSHOT;
                printf("Initial snapshot completed. Now tracking growth.\n");
                continue;
            }

            if (!list_exist(s)) {
                list_add(s);
                continue;
            }

            diff d = list_match(s);

            if (flag == FLAG_DIFF) {
                if (d.active_objs_diff != 0 || d.num_objs_diff != 0)
                    printf("Growth detected - Slab: %-20s | Active Δ: %+6d | Total Δ: %+6d\n",
                        s.name, d.active_objs_diff, d.num_objs_diff);
            }
        }

        wait(NULL);
    }

    return 0;
}
