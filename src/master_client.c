#if defined HAVE_CONFIG_H
#include "config.h"
#endif

#define _XOPEN_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/sem.h>


#include "myassert.h"

#include "master_client.h"

// fonctions éventuelles proposées dans le .h

typedef struct MasterP
{
    int nbPrime;
    int highestPrime;
};

int my_semcreate(int nbSemaphores, int *initVal)
{
    key_t key;
    int semId;
    int ret;

    key = ftok(MYFILE, PROJ_ID);
    myassert(key != -1, "Error my_semcreate : key creation failed");

    semId = semget(key, nbSemaphores, IPC_CREAT | IPC_EXCL | 0641);
    myassert(semId != -1, "Error my_semcreate : creation failed");

    for(int i = 0; i < nbSemaphores; i++){
        ret = semctl(semId, 0, SETALL, &(initVal[i]));
        myassert(ret != -1, "Error my_semget : initialization failed");
    }

    return semId;
}

int my_semget()
{
    key_t key;
    int semId;

    key = ftok(MYFILE, PROJ_ID);
    myassert(key != -1, "Error my_semcreate : key creation failed");

    semId = semget(key, 0, 0);
    myassert(semId != -1, "Error my_semget : semaphore creation failed");

    return semId;
}

void sem_take(int semId, int num_sem)
{
    struct sembuf op = {num_sem, -1, 0};
    int ret;

    ret = semop(semId, &op, 1);
	myassert(ret != -1, "Error sem_take : failed");
}

void sem_sell(int semId, int num_sem)
{
    struct sembuf op = {num_sem, 1, 0};
    int ret;

    ret = semop(semId, &op, 1);
	myassert(ret != -1, "Error sem_sell : failed");
}

void my_sem_destroy(int semId)
{
    int ret;

    ret = semctl(semId, 0, IPC_RMID);
    myassert(ret != -1, "Error my_sem_destroy : destruction failed");
}

void my_mkfifo(const char *pathname)
{
    int ret;
    ret = mkfifo(pathname, 0600);
    myassert(ret != -1, "Error my_mkfifo : creation pipe failed");
}

void my_unlink(const char *pathname)
{
    int ret = unlink(pathname);
    myassert(ret != -1, "Error my_unlink : failed");
}

int my_open(const char *pathname, int flags)
{
    int fd = open(pathname, flags);
    myassert(fd != -1, "Error my_open : failed");

    return fd;
}

void my_close(int fd)
{
    int ret = close(fd);
    myassert(ret != -1, "Error my_close : failed");
}

void my_read(int fd, void *buf, size_t count)
{
    int ret = read(fd, buf, count);
    myassert(ret != -1, "Error my_read : failed");
}

void my_write(int fd, const void *buf, size_t count)
{
    int ret = write(fd, buf, count);
    myassert(ret != -1, "Error my_write : failed");
}