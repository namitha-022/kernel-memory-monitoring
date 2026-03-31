// shared structs
#ifndef COMMON_H
#define COMMON_H

/*
 * Shared definitions between kernel module and supervisor
 * Keep this file lightweight and compatible with both
 * kernel-space and user-space.
 */

/* ============================= */
/*        IOCTL STRUCTURE        */
/* ============================= */

#include "../kernel_module/ioctl_defs.h"

/* ============================= */
/*     EXIT REASON STRINGS       */
/* ============================= */

#define REASON_NORMAL_EXIT        "normal_exit"
#define REASON_STOPPED            "stopped"
#define REASON_HARD_KILLED        "hard_limit_killed"
#define REASON_KILLED_OTHER       "killed_other"

/* ============================= */
/*       DEFAULT LIMITS          */
/* ============================= */

#define DEFAULT_SOFT_LIMIT  50000   // 50 MB
#define DEFAULT_HARD_LIMIT  80000   // 80 MB

/* ============================= */
/*       DEVICE NAME             */
/* ============================= */

#define DEVICE_PATH "/dev/container_monitor"

#endif
