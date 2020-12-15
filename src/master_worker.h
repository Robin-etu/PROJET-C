#ifndef MASTER_WORKER_H
#define MASTER_WORKER_H

// On peut mettre ici des éléments propres au couple master/worker :
//    - des constantes pour rendre plus lisible les comunications
//    - des fonctions communes (écriture dans un tube, ...)

// Fonctions communes
static void my_read(int fd, void *buf, size_t count);
static void my_write(int fd, const void *buf, size_t count);
static void my_close(int fd);

#endif
