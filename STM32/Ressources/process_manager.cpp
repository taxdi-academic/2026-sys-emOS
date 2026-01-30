#include "process_manager.h"

ProcessManager::ProcessManager() : current_pid_counter(1), last_scheduled_index(-1) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        processes[i].pid = -1;
        processes[i].state = PS_EMPTY;
        processes[i].name = nullptr;
        processes[i].func = nullptr;
        processes[i].total_runtime_ms = 0;
    }
}

int ProcessManager::findFreeSlot(void) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].state == PS_EMPTY) return i;
    }
    return -1;
}

int ProcessManager::createProcess(const char* name, user_program_fn func) {
    int slot = findFreeSlot();
    if (slot == -1) return -1; // no free slot

    processes[slot].pid = current_pid_counter++;
    processes[slot].name = name;
    processes[slot].func = func;
    processes[slot].state = PS_RUNNING;
    processes[slot].created_time = millis();
    processes[slot].last_scheduled_time = millis();
    processes[slot].total_runtime_ms = 0;

    return processes[slot].pid;
}

Process* ProcessManager::getProcess(int pid) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].pid == pid && processes[i].state != PS_EMPTY) {
            return &processes[i];
        }
    }
    return nullptr;
}

void ProcessManager::listProcesses(void (*print_fn)(const char*)) {
    print_fn("[Kernel] Active processes:\r\n");
    print_fn("PID | Name               | State      | CPU (ms)\r\n");
    print_fn("--- | --- | --- | ---\r\n");

    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].state == PS_EMPTY) continue;

        char buf[64];
        const char* state_str = "";
        if (processes[i].state == PS_RUNNING) state_str = "RUNNING";
        else if (processes[i].state == PS_SUSPENDED) state_str = "PAUSED";
        else if (processes[i].state == PS_ZOMBIE) state_str = "ZOMBIE";

        snprintf(buf, sizeof(buf), " %d  | %-15s | %-9s | %lu\r\n",
                 processes[i].pid, processes[i].name, state_str, processes[i].total_runtime_ms);
        print_fn(buf);
    }
}

bool ProcessManager::suspendProcess(int pid) {
    Process* p = getProcess(pid);
    if (!p || p->state != PS_RUNNING) return false;
    p->state = PS_SUSPENDED;
    return true;
}

bool ProcessManager::resumeProcess(int pid) {
    Process* p = getProcess(pid);
    if (!p || p->state != PS_SUSPENDED) return false;
    p->state = PS_RUNNING;
    return true;
}

bool ProcessManager::killProcess(int pid) {
    Process* p = getProcess(pid);
    if (!p) return false;
    p->state = PS_ZOMBIE;
    return true;
}

int ProcessManager::getNextRunnable(void) {
    // Round-Robin: search for the next RUNNING process after last_scheduled_index
    int start = (last_scheduled_index + 1) % MAX_PROCESSES;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        int idx = (start + i) % MAX_PROCESSES;
        if (processes[idx].state == PS_RUNNING) {
            last_scheduled_index = idx;
            return processes[idx].pid;
        }
    }
    return -1; // no executable process
}

int ProcessManager::countActive(void) {
    int count = 0;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].state == PS_RUNNING || processes[i].state == PS_SUSPENDED) {
            count++;
        }
    }
    return count;
}

int ProcessManager::countRunning(void) {
    int count = 0;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].state == PS_RUNNING) {
            count++;
        }
    }
    return count;
}

void ProcessManager::cleanupZombies(void) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].state == PS_ZOMBIE) {
            processes[i].pid = -1;
            processes[i].state = PS_EMPTY;
            processes[i].name = nullptr;
            processes[i].func = nullptr;
        }
    }
}
