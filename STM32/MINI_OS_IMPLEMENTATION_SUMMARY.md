
# Mini OS — Implementation Summary

**Objective**

This document summarizes the architecture and implementation choices of the mini OS (STM32/QEMU).

**Main Components**

- `ProcessManager` (`include/process_manager.h`, `src/process_manager.cpp`)
  - Process table (max 8). Each entry contains: PID, name, state, pointer to user function, timestamps, CPU counter.
  - States: `PS_EMPTY`, `PS_RUNNING`, `PS_SUSPENDED`, `PS_ZOMBIE`.
  - API: `createProcess()`, `getProcess()`, `killProcess()`, `suspendProcess()`, `resumeProcess()`, `getNextRunnable()`, `listProcesses()`, `cleanupZombies()`.

- `Scheduler` (`include/scheduler.h`, `src/scheduler.cpp`)
  - Cooperative Round-Robin scheduling.
  - `kernel_yield()` exposed to programs to allow them to yield to the kernel (detects Ctrl+C and sets `kernel_stop_requested`).
  - `run()` starts the scheduling loop: poll serial, tick, cleanup.

- `main.cpp` (shell)
  - Simple interactive shell to manipulate the process table and start the scheduler.

**Key Behavior**

- User programs follow a simple interface: `void prog(void (*print_fn)(const char*))`.
- The scheduler is cooperative: programs must call `kernel_yield()` or use `delay()` to allow the kernel to check inputs (Ctrl+C) and perform actions.

**How Round-Robin is Implemented**

1. `ProcessManager::getNextRunnable()` traverses the table (from `last_scheduled_index + 1`) and returns the PID of the next process in `PS_RUNNING`.
2. The scheduler calls the associated user function; the program executes and makes periodic yields.
3. On exit, the scheduler updates CPU time counters and cleans up zombies.

**Cooperative Interruption**

- `kernel_stop_requested` (defined in `scheduler.cpp`) is a flag read by programs.
- `kill(pid)` in `ProcessManager` marks the process as `PS_ZOMBIE` and triggers the cooperative stop flag if necessary.
- `kernel_yield()` allows detection of Ctrl+C (ASCII 3) sent on the console and sets `kernel_stop_requested = true`.

**Limitations and Improvement Points**

- No hardware preemption: for true preemption we need SysTick + context switch + register and stack saving per process.
- Each process shares the same memory address (no isolation) — risk of interference.
- Programs must be cooperative (call `kernel_yield()`).

Future improvements:

- Hardware preemption (SysTick) + `setjmp()`/`longjmp()` to save context.
- Allocate a dedicated stack per process and implement context switching.
- Add simple IPC (queues, signals) and more robust error handling.

**Directory Structure (relative path: `Ressource_OS_STM32/OS-FunctionPrograms/`)**

```
include/
  ├── `process_manager.h`    (NEW)     - Process management (structure, API)
  ├── `scheduler.h`          (NEW)     - Round-Robin scheduler (interface)
  └── `kernel_api.h`         (NEW)     - API exposed to programs (`kernel_yield`, stop flag)

src/
  ├── `main.cpp`             (MODIFIED) - Interactive shell + ProcessManager/Scheduler integration
  ├── `process_manager.cpp`  (NEW)     - Process management implementation
  ├── `scheduler.cpp`        (NEW)     - Scheduler and `kernel_yield` implementation
  └── user_programs/
      ├── `infiniteLoop.cpp` (MODIFIED) - Infinite example, became cooperative (`kernel_yield`)
      └── (other existing programs — e.g. `prog1`, `prog2`, `morpionv3`, etc.)

lib/                         (EXISTING) - Linked libraries (unchanged)

platformio.ini               (EXISTING) - PlatformIO configuration (board, pre/post scripts)

scripts/                     (EXISTING) - `pre-build-programs.py`, `post-qemu.py`
```

**Reminder**: this system is pedagogical — it favors simplicity and understanding of scheduling and cooperative interruption concepts.
