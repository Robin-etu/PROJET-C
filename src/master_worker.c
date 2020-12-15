#if defined HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>

#include "myassert.h"

#include "master_worker.h"

// structure d'un worker
struct WorkerP
{
    int p; // le nombre premier dont il a la charge (P)
    int fd_prev; // tube pour recevoir des données du worker vourant
    int fd_master; // tube pour communiquer avec le master
};

// fonctions éventuelles proposées dans le .h
void my_pipe(int pipefd[2])
{
    int ret = pipe(pipefd);
    myassert(ret != -1, "Error my_pipe : failed");
}

Worker create_worker(Worker w)
{
    Worker new_worker = malloc(sizeof(struct WorkerP));
    int fds[2];
    my_pipe(fds);

    if (w == NULL){
        new_worker->fd_master = fds[1];
        new_worker->fd_prev = fds[0];
        new_worker->p = 2;
    }
    else {
        new_worker->fd_master = w->fd_master;
        new_worker->fd_prev = fds[0];
    }

    char *arg1, *arg2, *arg3;
    sprintf(arg1, "%d", new_worker->fd_master);
    sprintf(arg2, "%d", new_worker->fd_prev);
    sprintf(arg3, "%d", new_worker->p);

    if (fork() == 0)
        execl("worker", "worker", arg1, arg2, arg3, NULL);
    
    return new_worker;
}