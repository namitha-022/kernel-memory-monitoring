// ioctl client
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "../kernel_module/ioctl_defs.h"

int register_pid(int pid, unsigned long soft, unsigned long hard)
{
    int fd = open("/dev/container_monitor", O_RDWR);
    if (fd < 0) {
        perror("open");
        return -1;
    }

    struct proc_info info = {pid, soft, hard};

    if (ioctl(fd, REGISTER_PID, &info) < 0) {
        perror("ioctl");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}