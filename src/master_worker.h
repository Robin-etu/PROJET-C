#ifndef MASTER_WORKER_H
#define MASTER_WORKER_H

// On peut mettre ici des éléments propres au couple master/worker :
//    - des constantes pour rendre plus lisible les comunications
//    - des fonctions communes (écriture dans un tube, ...)

// structure d'un worker
struct WorkerP;
typedef struct WorkerP *Worker;


// Fonctions communes
void my_pipe(int pipefd[2]);
Worker create_worker(Worker w);

#endif
