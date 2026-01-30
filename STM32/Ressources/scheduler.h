#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "process_manager.h"

// Scheduler class (simple Round-Robin)
class Scheduler {
public:
    Scheduler(ProcessManager* pm);

    // Exécuter un timeslice pour le processus courant
    // Retourne true si un processus a eu du CPU, false sinon
    bool tick(unsigned long timeslice_ms, void (*print_fn)(const char*));

    // Passer au processus suivant dans la queue
    void switchContext(void);

    // Obtenir le PID du processus courant en exécution
    int getCurrentPid(void);

    // Démarrer la boucle d'ordonnancement
    void run(void (*print_fn)(const char*));

private:
    ProcessManager* pm;
    int current_pid;
    unsigned long slice_start_time;
};

#endif
