#include <stdio.h>

#include "list.h"

void parse_vmstat_file(int phase) {
    FILE *f = fopen("/proc/vmstat", "r");
    if (!f) {
        perror("Failed to open /proc/vmstat");
        return;
    }

    char key[100];
    unsigned int val;
    char line[256];

    while (fgets(line, sizeof(line), f)) {
        if (sscanf(line, "%99s %u", key, &val) != 2) continue;

        struct diff d = list_update_or_add_vmstat(key, val);
        if (phase != INIT_SNAPSHOT && d.statsdiff != 0) {
            printf("VMSTAT %-20s Δ %8u\n", d.name, d.statsdiff);
        }
    }

    fclose(f);
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