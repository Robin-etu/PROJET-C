#if defined HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include "myassert.h"

#include "master_client.h"
#include "master_worker.h"

/************************************************************************
 * Données persistantes d'un master
 ************************************************************************/

// on peut ici définir une structure stockant tout ce dont le master
// a besoin
typedef struct MasterP
{
    int nbPrime;
    int highestPrime;
} *Master;


/************************************************************************
 * Usage et analyse des arguments passés en ligne de commande
 ************************************************************************/

static void usage(const char *exeName, const char *message)
{
    fprintf(stderr, "usage : %s\n", exeName);
    if (message != NULL)
        fprintf(stderr, "message : %s\n", message);
    exit(EXIT_FAILURE);
}


/************************************************************************
 * Fonctions auxiliaires
 ************************************************************************/

static Master create_master()
{
    Master self = malloc(sizeof(struct MasterP));
    self->highestPrime = 0;
    self->nbPrime = 0;

    return self;
}

static int my_semcreate(int nbSemaphores, int *initVal)
{
    key_t key;
    int semId;
    int ret;

    key = ftok(MYFILE, PROJ_ID);
    assert(key != -1);

    semId = semget(key, nbSemaphores, IPC_CREAT | IPC_EXCL | 0641);
    assert(semId != -1);

    for(int i = 0; i < nbSemaphores; i++){
        ret = semctl(semId, 0, SETALL, initVal[i]);
        myassert(ret != -1, "Error my_semget : initialization failed");
    }

    return semId;
}

static int my_semget()
{
    key_t key;
    int semId;

    key = ftok(MYFILE, PROJ_ID);
    assert(key != -1);

    semId = semget(key, 0, 0);
    assert(semId != -1);

    return semId;
}

static void sem_take(int semId, int num_sem)
{
    struct sembuf op = {num_sem, -1, 0};
    int ret;

    ret = semop(semId, &op, 1);
	myassert(ret != -1, "Error sem_take : failed");
}

static void sem_sell(int semId, int num_sem)
{
    struct sembuf op = {num_sem, 1, 0};
    int ret;

    ret = semop(semId, &op, 1);
	myassert(ret != -1, "Error sem_sell : failed");
}

static void my_sem_destroy(int semId)
{
    int ret;

    ret = semctl(semId, 0, IPC_RMID);
    myassert(ret != -1, "Error my_sem_destroy : destruction failed");
}

static void my_mkfifo(const char *pathname)
{
    int ret;
    ret = mkfifo(pathname, 0600);
    myassert(ret != -1, "Error my_mkfifo : creation pipe failed");
}

static void my_unlink(const char *pathname)
{
    int ret = unlink(pathname);
    myassert(ret != -1, "Error my_unlink : failed");
}

static int my_open(const char *pathname, int flags)
{
    int fd = open(pathname, flags);
    myassert(fd != -1, "Error my_open : failed");

    return fd;
}

static void my_close(int fd)
{
    int ret = close(fd);
    myassert(ret != -1, "Error my_close : failed");
}

static void my_read(int fd, void *buf, size_t count)
{
    int ret = read(fd, buf, count);
    myassert(ret != -1, "Error my_read : failed");
}

static void my_write(int fd, const void *buf, size_t count)
{
    int ret = write(fd, buf, count);
    myassert(ret != -1, "Error my_write : failed");
}

static void create_worker()
{
    int fds[2];
    pipe(fds);

    if (fork() == 0)
        execl("worker",fds[0], fds[1], NULL);
}


/************************************************************************
 * boucle principale de communication avec le client
 ************************************************************************/
void loop(Master self, int semId)
{
    // boucle infinie :
    // - ouverture des tubes (cf. rq client.c)
    // - attente d'un ordre du client (via le tube nommé)
    // - si ORDER_STOP
    //       . envoyer ordre de fin au premier worker et attendre sa fin
    //       . envoyer un accusé de réception au client
    // - si ORDER_COMPUTE_PRIME
    //       . récupérer le nombre N à tester provenant du client
    //       . construire le pipeline jusqu'au nombre N-1 (si non encore fait) :
    //             il faut connaître le plus nombre (M) déjà enovoyé aux workers
    //             on leur envoie tous les nombres entre M+1 et N-1
    //             note : chaque envoie déclenche une réponse des workers
    //       . envoyer N dans le pipeline
    //       . récupérer la réponse
    //       . la transmettre au client
    // - si ORDER_HOW_MANY_PRIME
    //       . transmettre la réponse au client
    // - si ORDER_HIGHEST_PRIME
    //       . transmettre la réponse au client
    // - fermer les tubes nommés
    // - attendre ordre du client avant de continuer (sémaphore : précédence)
    // - revenir en début de boucle
    //
    // il est important d'ouvrir et fermer les tubes nommés à chaque itération
    // voyez-vous pourquoi ?

    int input;
    int order;

    while(1)
    {
        // - ouverture des tubes (cf. rq client.c)
        int fd1 = my_open(PIPE_CLIENT_MASTER, O_RDONLY);
        int fd2 = my_open(PIPE_MASTER_CLIENT, O_WRONLY);

        // - attente d'un ordre du client (via le tube nommé)
        my_read(fd1, &input, sizeof(int));

        switch(input)
        {
            case ORDER_STOP : {

            }

            case ORDER_COMPUTE_PRIME : {

            }

            case ORDER_HOW_MANY_PRIME : {
                // on, écrit la réponse sur le PIPE_MASTER_CLIENT
                my_write(fd2, &(self->nbPrime), sizeof(int));
            }

            case ORDER_HIGHEST_PRIME : {
                // on écrit la réponse sur le PIPE_MASTER_CLIENT
                my_write(fd2, &(self->highestPrime), sizeof(int));
            }
        }

        // - fermeture des tubes nommés
        my_close(fd1);
        my_close(fd2);

        // - attendre ordre du client avant de continuer (2ème sémaphore)
        sem_take(semId, 1);
    }

}


/************************************************************************
 * Fonction principale
 ************************************************************************/

int main(int argc, char * argv[])
{
    if (argc != 1)
        usage(argv[0], NULL);

    // - création et initialisation de la structure stockant tout ce dont a besoin le master
    Master self = create_master();

    // - création des sémaphores
    int initVal[2] = {1, 0};
    int semId = my_semget(2, initVal);

    // - création des tubes nommés
    my_mkfifo(PIPE_CLIENT_MASTER);
    my_mkfifo(PIPE_MASTER_CLIENT);

    // - création du premier worker
    if (fork() == 0)
        execl("worker",NULL);

    // boucle infinie
    loop(self, semId);

    // destruction des tubes nommés, des sémaphores, ...
    my_unlink(PIPE_CLIENT_MASTER);
    my_unlink(PIPE_MASTER_CLIENT);
    my_sem_destroy(semId);

    return EXIT_SUCCESS;
}

// N'hésitez pas à faire des fonctions annexes ; si les fonctions main
// et loop pouvaient être "courtes", ce serait bien
