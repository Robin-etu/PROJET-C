#if defined HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
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
typedef struct MasterP {
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

static int create_sem(int nbSem)
{
    key_t key;
    int semId;

    key = ftok(MYFILE, PROJ_ID);
    assert(key != -1);

    semId = semget(key, nbSem, IPC_CREAT | IPC_EXCL | 0641);
    assert(semId != -1);

    return semId;
}

static void create_namedPipe(const char *name)
{
    int ret;
    ret = mkfifo(name, 0600);
    assert(ret != -1);
}


/************************************************************************
 * boucle principale de communication avec le client
 ************************************************************************/
void loop(Master self, int semID)
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
        int fd1 = open(PIPE_CLIENT_MASTER, O_RDONLY);
        assert(fd1 != -1);
        int fd2 = open(PIPE_MASTER_CLIENT, O_WRONLY);
        assert(fd2 != -1);

        
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
    int semId = create_sem(2);

    // - création des tubes nommés
    create_namedPipe(PIPE_CLIENT_MASTER);
    create_namedPipe(PIPE_MASTER_CLIENT);

    // - création du premier worker

    // boucle infinie
    loop(self, semId);

    // destruction des tubes nommés, des sémaphores, ...
    unlink(PIPE_CLIENT_MASTER);
    unlink(PIPE_MASTER_CLIENT);
    semctl(semId, -1, IPC_RMID);

    return EXIT_SUCCESS;
}

// N'hésitez pas à faire des fonctions annexes ; si les fonctions main
// et loop pouvaient être "courtes", ce serait bien
