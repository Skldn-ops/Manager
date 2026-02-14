#include "headl.h"

void sem_lock(int semid, unsigned short pos) {
    struct sembuf sb = {pos, -1, 0};
    semop(semid, &sb, 1);
}

void sem_unlock(int semid, unsigned short pos) {
    struct sembuf sb = {pos, 1, 0};
    semop(semid, &sb, 1);
}