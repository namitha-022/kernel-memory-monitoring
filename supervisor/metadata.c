// metadata logic
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include "metadata.h"

void classify_exit(struct container_meta *meta, int status)
{
    if (WIFEXITED(status)) {
        strcpy(meta->reason, "normal_exit");
    } else if (WIFSIGNALED(status)) {
        int sig = WTERMSIG(status);

        if (sig == SIGKILL && !meta->stop_requested) {
            strcpy(meta->reason, "hard_limit_killed");
        } else if (meta->stop_requested) {
            strcpy(meta->reason, "stopped");
        } else {
            strcpy(meta->reason, "killed_other");
        }
    }
}