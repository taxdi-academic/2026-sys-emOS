# STM32
## 1. Prerequisites

To complete this section, we will need the following tools:
  - Docker
  - Socat
    - Useful for connecting applications inside separate containers
  - ssh (to facilitate VM usage)
      - Allows easier copy-paste operations
  - GIT (which will allow us to use the resources for this lab)

We will start by cloning the git repository, which contains the resources for the lab, with the command:
```
git clone https://forgens.univ-ubs.fr/gitlab/charton/virtos.git
```
Finally, we will go to the **/virtos/Ressource_OS_STM32/docker** folder to build the stm32 (see Build.md).

---

## 2. Adding Features

### 2.1 Interruption
Our STM32 does not contain a software interrupt system equivalent to what is found in a classic operating system (like the `SIGINT` signal on Linux).
We will therefore create our own global interrupt mechanism, which will allow interrupting any running program using a keyboard combination, such as `Ctrl + C`

---

#### 2.1.1 Objectives

- Simulate behavior similar to `Ctrl+C` on a Linux terminal.
- Be able to interrupt **any program** (kernel or user).
- Centralize this management in a **single module**: `interrupt_handler`.
- Provide consistent and reproducible behavior throughout the system.

---

#### 2.1.2 Operating Principle

When you type `Ctrl + C` in the serial terminal (via `socat`), the ASCII character `3` is sent to the STM32.

A `checkInterrupt()` function is responsible for:
1. Reading the characters present on the serial port.
2. Detecting the code `3` (corresponding to `Ctrl+C`).
3. Triggering a **global flag** `interrupted = true`.

All system programs can then check this flag to stop cleanly.

---

#### 2.1.3 File Structure

include/
└── interrupt_handler.h

src/
└── interrupt_handler.cpp

#### 2.1.4 Logic Flowchart

         ┌──────────────┐
         │  Terminal    │
         │ (Ctrl + C)   │
         └──────┬───────┘
                │
        ASCII 3 via Serial
                │
     ┌──────────▼──────────┐
     │ checkInterrupt()    │
     │ detects ASCII = 3   │
     │ interrupted = true  │
     └──────────┬──────────┘
                │
     ┌──────────▼──────────┐
     │ Kernel/User Loops   │
     │ if(interrupted)     │
     │   clean stop        │
     └─────────────────────┘

#### 2.1.5 Advantages and Disadvantages

| **Category** | **Description** |
|----------------|-----------------|
| ✅ **Simplicity of integration** | The system relies only on serial reading (`Serial`), without hardware or OS dependency. |
| ✅ **Universality** | Works in the **kernel** and in all **user programs** without major modification.* |
| ✅ **SIGINT-like behavior** | Simulates the behavior of the `Ctrl+C` signal on Linux (clean program stop). |
| ✅ **Compatibility** | Works on both **QEMU** and a **physical STM32 board**. |
| ✅ **Modularity** | The handler is isolated in a module (`interrupt_handler`), easy to reuse and extend. |
| ⚠️ **Not a real hardware interrupt** | The system relies on **periodic checking** (`polling`) of the serial port, not on a real IRQ. |
| ⚠️ **Possible latency** | If a function blocks for a long time (e.g. `delay(5000)`), Ctrl+C detection is delayed. |
| ⚠️ **No asynchronous interrupt** | The signal does not immediately interrupt execution: `checkInterrupt()` must be called regularly. |
| ⚠️ **No priority** | Only one global interrupt is handled — no hierarchization or signal masking. |
| 💡 **Possible extension** | We can link `Ctrl+C` to a **scheduler**, a **physical button** or other signals (`Ctrl+Z`, etc.). |

* ⚠️ YOU MUST FIRST DEFINE A background_scheduler THAT WILL RUN THE INTERRUPT PROGRAM IN THE BACKGROUND !!

