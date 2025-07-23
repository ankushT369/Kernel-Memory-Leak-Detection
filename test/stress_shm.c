// gcc -o stress_shm stress_shm.c -lrt
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    printf("[*] Creating large shared memory segments...\n");
    for (int i = 0; i < 100; i++) {
        int shmid = shmget(IPC_PRIVATE, 1024*1024, IPC_CREAT | 0600);
        if (shmid < 0) perror("shmget");
    }
    sleep(5);
    return 0;
}

