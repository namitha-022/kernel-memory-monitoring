// Kernel module skeleton
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/sched/signal.h>
#include <linux/mm.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/mutex.h>
#include <linux/device.h>

#include "ioctl_defs.h"

#define DEVICE_NAME "container_monitor"

static int major;
static struct class *cls;
static struct task_struct *monitor_thread;

struct proc_entry {
    pid_t pid;
    unsigned long soft_limit;
    unsigned long hard_limit;
    int soft_exceeded;

    struct task_struct *task;
    struct list_head list;
};

static LIST_HEAD(proc_list);
static DEFINE_MUTEX(proc_list_lock);

static int monitor_fn(void *data)
{
    struct proc_entry *entry, *tmp;

    while (!kthread_should_stop()) {
        mutex_lock(&proc_list_lock);

        list_for_each_entry_safe(entry, tmp, &proc_list, list) {

            struct task_struct *task = pid_task(find_vpid(entry->pid), PIDTYPE_PID);

            if (!task || !task->mm) {
                list_del(&entry->list);
                kfree(entry);
                continue;
            }

            unsigned long rss = get_mm_rss(task->mm) << (PAGE_SHIFT - 10);

            if (rss > entry->soft_limit && !entry->soft_exceeded) {
                printk(KERN_WARNING "[monitor] PID %d exceeded SOFT (%lu KB)\n",
                       entry->pid, rss);
                entry->soft_exceeded = 1;
            }

            if (rss > entry->hard_limit) {
                printk(KERN_ERR "[monitor] PID %d exceeded HARD (%lu KB) - killing\n",
                       entry->pid, rss);

                send_sig(SIGKILL, task, 0);

                list_del(&entry->list);
                kfree(entry);
            }
        }

        mutex_unlock(&proc_list_lock);
        msleep(1000);
    }
    return 0;
}

static long device_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    struct proc_info info;
    struct proc_entry *entry;

    if (cmd == REGISTER_PID) {

        if (copy_from_user(&info, (void __user *)arg, sizeof(info)))
            return -EFAULT;

        entry = kmalloc(sizeof(*entry), GFP_KERNEL);
        if (!entry)
            return -ENOMEM;

        entry->pid = info.pid;
        entry->soft_limit = info.soft_limit;
        entry->hard_limit = info.hard_limit;
        entry->soft_exceeded = 0;

        mutex_lock(&proc_list_lock);
        list_add(&entry->list, &proc_list);
        mutex_unlock(&proc_list_lock);

        printk(KERN_INFO "[monitor] Registered PID %d\n", info.pid);
    }

    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = device_ioctl,
};

static int __init monitor_init(void)
{
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ERR "container_monitor: register_chrdev failed\n");
        return major;
    }

    cls = class_create(DEVICE_NAME);
    if (IS_ERR(cls)) {
        unregister_chrdev(major, DEVICE_NAME);
        printk(KERN_ERR "container_monitor: class_create failed\n");
        return PTR_ERR(cls);
    }

    if (IS_ERR(device_create(cls, NULL, MKDEV(major, 0), NULL, DEVICE_NAME))) {
        class_destroy(cls);
        unregister_chrdev(major, DEVICE_NAME);
        printk(KERN_ERR "container_monitor: device_create failed\n");
        return -1;
    }

    monitor_thread = kthread_run(monitor_fn, NULL, "mem_monitor");
    if (IS_ERR(monitor_thread)) {
        device_destroy(cls, MKDEV(major, 0));
        class_destroy(cls);
        unregister_chrdev(major, DEVICE_NAME);
        printk(KERN_ERR "container_monitor: kthread_run failed\n");
        return PTR_ERR(monitor_thread);
    }

    printk(KERN_INFO "container_monitor loaded\n");
    return 0;
}

static void __exit monitor_exit(void)
{
    struct proc_entry *entry, *tmp;

    kthread_stop(monitor_thread);

    mutex_lock(&proc_list_lock);
    list_for_each_entry_safe(entry, tmp, &proc_list, list) {
        list_del(&entry->list);
        kfree(entry);
    }
    mutex_unlock(&proc_list_lock);

    device_destroy(cls, MKDEV(major, 0));
    class_destroy(cls);
    unregister_chrdev(major, DEVICE_NAME);

    printk(KERN_INFO "container_monitor unloaded\n");
}

module_init(monitor_init);
module_exit(monitor_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Container memory monitor with soft/hard limits");
MODULE_AUTHOR("Kernel Memory Monitoring Project");