#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#include <Arduino.h>

// Pointer to user function (standard signature)
typedef void (*user_program_fn)(void (*print_fn)(const char*));

// Possible process states
enum ProcessState {
    PS_EMPTY,       // empty slot
    PS_RUNNING,     // running
    PS_SUSPENDED,   // suspended (pause)
    PS_ZOMBIE       // terminated, waiting for cleanup
};

// Structure to manage a process
struct Process {
    int pid;                           // process ID
    const char* name;                  // program name
    user_program_fn func;              // pointer to the program function
    ProcessState state;                // current state
    unsigned long created_time;        // creation timestamp
    unsigned long last_scheduled_time; // last time it got CPU time
    unsigned long total_runtime_ms;    // total execution time (ms)
};

// Process management class
class ProcessManager {
public:
    static const int MAX_PROCESSES = 8;

    ProcessManager();

    // Create and launch a new process
    int createProcess(const char* name, user_program_fn func);

    // Retrieve a process by PID
    Process* getProcess(int pid);

    // List all active processes
    void listProcesses(void (*print_fn)(const char*));

    // Suspend a process (pause)
    bool suspendProcess(int pid);

    // Resume a paused process
    bool resumeProcess(int pid);

    // Terminate a process
    bool killProcess(int pid);

    // Get the next process to execute (Round-Robin)
    int getNextRunnable(void);

    // Number of active processes (RUNNING or SUSPENDED)
    int countActive(void);

    // Number of RUNNING processes
    int countRunning(void);

    // Clean up zombie processes
    void cleanupZombies(void);

private:
    Process processes[MAX_PROCESSES];
    int current_pid_counter;
    int last_scheduled_index; // for Round-Robin

    int findFreeSlot(void);
};

#endif
