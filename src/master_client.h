#ifndef CLIENT_CRIBLE
#define CLIENT_CRIBLE

// On peut mettre ici des éléments propres au couple master/client :
//    - des constantes pour rendre plus lisible les comunications
//    - des fonctions communes (création tubes, écriture dans un tube,
//      manipulation de sémaphores, ...)

// fichier et identifiant utilisés comme paramètres de ftok pour créer une clé lors de l'utilisation de sémaphores dans les fichiers master.c et client.c
#define MYFILE "master_client.h"
#define PROJ_ID 5

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

// Fonctions communes
static int my_semget();
static void sem_take(int semId, int num_sem);
static void sem_sell(int semId, int num_sem);
static void my_sem_destroy(int semId);
static void my_unlink(const char *pathname);
static int my_open(const char *pathname, int flags);
static void my_close(int fd);
static void my_read(int fd, void *buf, size_t count);
static void my_write(int fd, const void *buf, size_t count);


#endif
