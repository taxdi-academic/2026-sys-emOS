#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "process_manager.h"

// Scheduler class (simple Round-Robin)
class Scheduler {
public:
    Scheduler(ProcessManager* pm);

    // Execute a timeslice for the current process
    // Returns true if a process got CPU time, false otherwise
    bool tick(unsigned long timeslice_ms, void (*print_fn)(const char*));

    // Switch to the next process in the queue
    void switchContext(void);

    // Get the PID of the current running process
    int getCurrentPid(void);

    // Start the scheduling loop
    void run(void (*print_fn)(const char*));

private:
    ProcessManager* pm;
    int current_pid;
    unsigned long slice_start_time;
};

#endif
