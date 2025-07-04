#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "kmld.h"
#include "list.h"

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
    //arg: -diff, -all, -top 3,  
    int flag = -1;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-diff") == 0) {
            flag = FLAG_DIFF;
        } else if (strcmp(argv[i], "-all") == 0) {
            flag = FLAG_ALL;
        } else if (strcmp(argv[i], "-top") == 0) {
            if (i + 1 < argc) {
                int n;
                if (sscanf(argv[i + 1], "%d", &n) == 1 && n > 0) {
                    flag = FLAG_TOP;
                    i++;
                } else {
                    fprintf(stderr, "Error: -top must be followed by a positive integer\n");
                    exit(1);
                }
            } else {
                fprintf(stderr, "Error: -top requires a number\n");
                exit(1);
            }
        }

    }

    return flag;
}


int main(int argc, char* argv[]) {
    if (argc < 2) {
        //handle wrong CLI args
        print_usage();
        exit(1);
    }

    int flag = FLAG_INVALID;
    flag = parse_arg(argc, argv);

    if (flag == FLAG_INVALID) {
        printf("Invalid arguments\n");
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
            while (fgets(line, sizeof(line), file)) {
                write(fds[1], line, strlen(line));
            }

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
            if (!read_line_from_pipe(fds[0], line)) continue;
            printf("HEY\n");

            int matched = sscanf(line, "%s %u %u %zu %u %u",
                                 s.name, &s.active_objs, &s.num_objs,
                                 &s.objsize, &s.objperslab, &s.pagesperslab);

            if (matched == 6) {
                if (flag == FLAG_ALL) {
                    printf("Received slab: %s | Active: %u | Total: %u\n",
                        s.name, s.active_objs, s.num_objs);
                }

                if (snapshot_phase == INIT_SNAPSHOT) {
                    list_add(s);
                    snapshot_phase = CHECK_SNAPSHOT;
                    printf("Initial snapshot completed. Now tracking growth.\n");
                } else {
                    if (!list_exist(s)) {
                        list_add(s);
                    } else {
                        diff d = list_match(s);
                        if (d.active_objs_diff != 0 || d.num_objs_diff != 0) {
                            if (flag == FLAG_DIFF) {
                                printf("Growth detected - Slab: %-20s | Active Δ: %+6d | Total Δ: %+6d\n",
                                    s.name, d.active_objs_diff, d.num_objs_diff);
                            }
                            //top k slabs list logic here
                        }
                    }
                }
            }
        }

        wait(NULL);
    }

    return 0;
}
