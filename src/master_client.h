#ifndef CLIENT_CRIBLE
#define CLIENT_CRIBLE

// On peut mettre ici des éléments propres au couple master/client :
//    - des constantes pour rendre plus lisible les comunications
//    - des fonctions communes (création tubes, écriture dans un tube,
//      manipulation de sémaphores, ...)

// fichier et identifiant utilisés comme paramètres de ftok pour créer une clé lors de l'utilisation de sémaphores dans les fichiers master.c et client.c
#define MYFILE "./master_client.h"
#define PROJ_ID 5
#define KEY ftok(MYFILE, PROJ_ID)

// noms des tubes nommés utilisés pour communiquer entre le(s) client(s) et le master
#define PIPE_MASTER_CLIENT "results"
#define PIPE_CLIENT_MASTER "datas"

// ordres possibles pour le master
#define ORDER_NONE                0
#define ORDER_STOP               -1
#define ORDER_COMPUTE_PRIME       1
#define ORDER_HOW_MANY_PRIME      2
#define ORDER_HIGHEST_PRIME       3
#define ORDER_COMPUTE_PRIME_LOCAL 4   // ne concerne pas le master

// bref n'hésitez à mettre nombre de fonctions avec des noms explicites
// pour masquer l'implémentation

// structure du master
struct MasterP;
typedef struct MasterP *Master;

// Fonctions communes
int my_semcreate(int nbSemaphores, int *initVal);
int my_semget();
void sem_take(int semId, int num_sem);
void sem_sell(int semId, int num_sem);
void my_sem_destroy(int semId);
void my_mkfifo(const char *pathname);
void my_unlink(const char *pathname);
int my_open(const char *pathname, int flags);
void my_close(int fd);
void my_read(int fd, void *buf, size_t count);
void my_write(int fd, const void *buf, size_t count);


#endif
