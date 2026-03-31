// Supervisor main
// simulate manual stop after 2 seconds
sleep(2);

meta.stop_requested = 1;
kill(pid, SIGTERM);