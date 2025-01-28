#ifndef DEFS_H
#define DEFS_H

typedef struct {
    int servings;
    int cook_waiting; 
    int sav_waiting;
}shared;

#define SHM_NAME "/shared"
#define SEM_MUTEX "/sem_mutex"
#define SEM_COOK_QUEUE "/sem_cook"
#define SEM_SAV_QUEUE "/sem_sav"

#endif