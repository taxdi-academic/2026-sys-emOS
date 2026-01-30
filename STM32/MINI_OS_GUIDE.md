# Mini OS for STM32 - User Guide

## Overview

This mini OS implements the 3 required tasks:

### 1. **Interrupt Mechanism**
- Programs running indefinitely (e.g. `infiniteLoop`) can be stopped via the `kill <pid>` command
- An interrupted process moves to the `ZOMBIE` state and is automatically cleaned up

### 2. **Multi-process Management**
- Interactive shell with commands to create, list, suspend and terminate processes
- Each process has a unique PID and a state (RUNNING, SUSPENDED, ZOMBIE)
- Ability to switch between processes without "background task" (single-context mode)

### 3. **Round-Robin Scheduler**
- Implementation of a scheduler that executes processes alternately
- Each process gets a "time slice" (timeslice) before moving to the next one
- Suspended processes are ignored by the scheduler
- Display of CPU time consumed by each process

---

## Architecture

### Main Components

#### `ProcessManager` (process_manager.h/cpp)
- Manages the process table (max 8 processes simultaneously)
- Process states: EMPTY, RUNNING, SUSPENDED, ZOMBIE
- Methods:
  - `createProcess()`: create a new process
  - `getProcess()`: retrieve a process by PID
  - `killProcess()`: terminate a process
  # Mini OS — User Guide (revised version)

  **Purpose**

  Provide a pedagogical mini kernel for STM32 (QEMU) allowing:
  - to execute multiple user programs,
  - to stop blocking programs (Ctrl+C or `kill <pid>`),
  - to schedule processes (cooperative Round-Robin).

  This guide explains the usage and quick test scenarios.

  **Prerequisites**

  - Compile the project with PlatformIO from the `OS-FunctionPrograms` folder:
    ```bash
    pio run
    ```
  - Connect to the serial console (115200) to access the shell.

  **Main Shell Commands**

  - `list`: display the list of available programs (ID).
  - `ps`: display active processes (PID, name, state, CPU ms).
  - `launch <id>`: launch a program in blocking mode (single-task).
  - `start <id>`: create a process (added to the table and marked RUNNING).
  - `kill <pid>`: request cooperative stop of a process (marks ZOMBIE).
  - `suspend <pid>` / `resume <pid>`: pause / resume a process.
  - `run`: start the Round-Robin scheduler (RUNNING processes are alternated).
  - `help`: display help.
  - `quit`: stop the system.

  Note: Ctrl+C (ASCII 3) sent from the terminal requests a global cooperative stop (equivalent to general `kill`).

  **Quick Scenarios**

  - Launch two processes then execute the scheduler:
    1. `start 1` (prog2)
    2. `start 7` (infiniteLoop)
    3. `run`

    `prog2` will be executed and finish, `infiniteLoop` runs in the background. Press Ctrl+C in the terminal to request stop.

  - Stop a blocking process:
    1. `start 7` (creates infiniteLoop)
    2. `ps` → note the PID
    3. `kill <pid>` → the process will read the flag and exit cleanly

  **Scheduler Behavior**

  The scheduler is cooperative: programs must call `kernel_yield()` regularly (or use `delay`) to allow the kernel to read Ctrl+C and perform operations. `infiniteLoop` now includes `kernel_yield()`.

  **Limitations**

  - No true preemption: the kernel does not save the hardware context of a program. For complete preemption it would require: SysTick + context switch + separate stacks.
  - No memory isolation between processes.
