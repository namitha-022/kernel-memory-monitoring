// ioctl definitions
#ifndef IOCTL_DEFS_H
#define IOCTL_DEFS_H

#include <linux/ioctl.h>

#define REGISTER_PID _IOW('a', 'a', struct proc_info)

struct proc_info {
    int pid;
    unsigned long soft_limit;
    unsigned long hard_limit;
};

#endif