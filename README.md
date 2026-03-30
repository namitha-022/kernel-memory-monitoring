# kernel-memory-monitoring

A Linux kernel module and user-space supervisor system that monitors process memory usage and enforces soft and hard memory limits.

---

## Important Requirement

This project requires Secure Boot to be DISABLED.
Reason:
Linux blocks unsigned kernel modules when Secure Boot is enabled.

---

## Setup Instructions 

### 1. Install Dependencies

sudo dnf update
sudo dnf install gcc make kernel-devel kernel-headers
sudo dnf install kernel-devel-$(uname -r)

---

### 2. Verify Kernel Headers

uname -r
ls /usr/src/kernels/

Ensure your kernel version exists in the directory.

---

## Build Instructions

### 3. Build Kernel Module

cd kernel_module
make

---

### 4. Build Test Programs

cd ../tests
gcc memory_hog.c -o memory_hog
gcc normal_process.c -o normal_process

---

### 5. Build Supervisor

cd ../supervisor
make

---

## Run Instructions

### 6. Load Kernel Module

cd ../kernel_module
sudo insmod container_monitor.ko

---

### 7. Check Kernel Logs

sudo dmesg | tail

---

### 8. Get Major Number

sudo dmesg | grep container_monitor

Example output:
container_monitor loaded (major=240)

---

### 9. Create Device File

sudo mknod /dev/container_monitor c <major_number> 0
sudo chmod 666 /dev/container_monitor

Example:
sudo mknod /dev/container_monitor c 240 0

---

### 10. Run Supervisor

cd ../supervisor
./supervisor

---

## Expected Output

Terminal:
PID: 1234 -> hard_limit_killed

Kernel logs:
sudo dmesg | grep monitor

Example:
[monitor] Registered PID 1234
[monitor] PID 1234 exceeded SOFT (52000 KB)
[monitor] PID 1234 exceeded HARD (81000 KB) - killing

---

## Exit Classification

Normal exit → normal_exit  
Manual stop → stopped  
Hard limit exceeded → hard_limit_killed  

---

## Test Programs

memory_hog → continuously allocates memory  
normal_process → exits normally  

---

## Cleanup

sudo rmmod container_monitor
sudo rm /dev/container_monitor

---

## Troubleshooting

Error: Key was rejected by service  
→ Disable Secure Boot

Error: No such device  
→ Create device using mknod

Error: Permission denied  
→ sudo chmod 666 /dev/container_monitor

Cannot read logs  
→ sudo dmesg | tail

---

## Summary

- Kernel module monitors memory usage
- Soft limit → warning
- Hard limit → process killed
- Supervisor tracks exit reason
