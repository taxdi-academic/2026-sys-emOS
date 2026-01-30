# Zombie Generation

In a zombie_generator.c program --> we create zombies at regular intervals (2s)
There is a function that creates the zombies, a handler that allows creating the USR1 command to kill the zombies

## To compile the program:

```
gcc -Wall -O2 -o zombie_manager zombie_manager.c
```

## To display the processes

```
ps --ppid <PROGRAM_PID> -o pid,ppid,stat,cmd
```

## To kill zombie processes:

```
kill -USR1 <PROGRAM_PID>
```
