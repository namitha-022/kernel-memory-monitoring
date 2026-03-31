// header
#ifndef METADATA_H
#define METADATA_H

struct container_meta {
    int pid;
    int stop_requested;
    char reason[32];
};

void classify_exit(struct container_meta *meta, int status);

#endif