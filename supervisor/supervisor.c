#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include "metadata.h"
#include "monitor_client.h"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <pid>\n", argv[0]);
        return 1;
    }

    pid_t pid = atoi(argv[1]);
    struct container_meta meta = {0};

    unsigned long soft_limit = 50 * 1024;  // 50 MB
    unsigned long hard_limit = 80 * 1024;  // 80 MB

    if (register_pid(pid, soft_limit, hard_limit) < 0) {
        fprintf(stderr, "Failed to register PID with kernel module\n");
        return 1;
    }

    printf("Monitoring PID %d (soft=%lu KB, hard=%lu KB)\n", pid, soft_limit, hard_limit);

    while (1) {
        int status;
        int result = waitpid(pid, &status, WNOHANG);
        if (result > 0) {
            classify_exit(&meta, status);
            printf("PID: %d -> %s\n", pid, meta.reason);
            break;
        }
        if (result < 0) {
            break;
        }
        usleep(100000);
    }

    return 0;
}