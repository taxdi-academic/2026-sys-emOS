# Mini OS — Usage Examples (revised version)

This file contains step-by-step scenarios to use the mini OS shell and scheduler.

Example 1 — Launch multiple processes and use the scheduler
-------------------------------------------------------------

1) Create two processes:

```
shell> start 1         # prog2 (counter 0..4)
shell> start 7         # infiniteLoop
```

2) Check the processes:

```
shell> ps
```

3) Start the scheduler:

```
shell> run
```

Observations:
- `prog2` will execute its iterations then terminate (ZOMBIE).
- `infiniteLoop` will continue until we request its stop.
- Press `Ctrl+C` in the terminal to request cooperative stop (the program must call `kernel_yield()` to be responsive).

Example 2 — Start, suspend and resume
--------------------------------------------

```
shell> start 4         # morpionv3
shell> start 5         # morpionv4
shell> ps
shell> suspend <pid>   # suspend a PID obtained via ps
shell> resume <pid>
```

Example 3 — Stop a blocking process
-----------------------------------------

1) Start the blocking task: `start 7` (infiniteLoop)
2) Note the PID via `ps`
3) `kill <pid>` to request cooperative stop

The program will display a stop message and the scheduler will clean up the table.

Example 4 — Advanced scenario (manual automated tests)
-----------------------------------------------------

- Chain `start 1`, `start 7`, `run` then in another terminal execute `kill <pid>` to observe the asynchronous reaction.

Useful Rules
------------

- `kernel_yield()`: cooperative programs must call it regularly to be preemptible (already added in `infiniteLoop`).
- `kill` marks a process as `ZOMBIE` and triggers the cooperative stop flag.
- The scheduler periodically cleans up zombies.

Quick Troubleshooting
----------------

- No process listed after `start` → Check that `createProcess()` returned a valid PID.
- `kill` has no effect → check that the program calls `kernel_yield()` or `delay()`.
