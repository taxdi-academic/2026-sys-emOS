#include "scheduler.h"

// Volatile flag to handle interruption (setjmp/longjmp style)
volatile static bool interrupt_requested = false;
volatile static int interrupt_target_pid = -1;

Scheduler::Scheduler(ProcessManager* pm) : pm(pm), current_pid(-1), slice_start_time(0) {}

int Scheduler::getCurrentPid(void) {
    return current_pid;
}

void Scheduler::switchContext(void) {
    if (!pm) return;
    current_pid = pm->getNextRunnable();
}

bool Scheduler::tick(unsigned long timeslice_ms, void (*print_fn)(const char*)) {
    if (!pm || pm->countRunning() == 0) {
        return false; // no executable process
    }

    // Get the next process
    switchContext();
    if (current_pid == -1) return false;

    Process* p = pm->getProcess(current_pid);
    if (!p || p->func == nullptr) return false;

    slice_start_time = millis();

    // Execute the process for max timeslice_ms
    // In practice on Arduino/STM32, we don't have true preemption
    // So we launch the program and it must cooperate (yield/delay)
    p->func(print_fn);

    unsigned long elapsed = millis() - slice_start_time;
    p->total_runtime_ms += elapsed;
    p->last_scheduled_time = millis();

    // Clean up zombies
    pm->cleanupZombies();

    return true;
}

void Scheduler::run(void (*print_fn)(const char*)) {
    // Main scheduler loop
    while (pm->countRunning() > 0) {
        tick(100, print_fn); // timeslice de 100ms (approx)
    }
    print_fn("[Kernel] All processes are completed.\r\n");
}
