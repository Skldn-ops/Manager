#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/prctl.h>
#include <fcntl.h>

#ifndef DEF
#define DEF

#define MAX_LEN 1024
#define MAX_PROGRAMMS_RUN 100
#define ID_INFO_SIZE 25

typedef enum TaskState{
    TASK_DELAYED,
    TASK_RUNNING,
    TASK_COMPLETED,
    TASK_FAILED,
    TASK_TIMEOUT,
} TaskState;

enum{
    TASK_TAB = 0,
    LOG_FD = 1
};

typedef struct Task{
    long long id;
    unsigned long long created_at;
    unsigned long long scheduled_at;
    unsigned long long started_at;
    unsigned long long finished_at;
    unsigned int delay;
    unsigned int timeout;
    int exit_code;
    int attempts;
    char program_to_exec[MAX_LEN];
    TaskState state;
} Task;


typedef struct QueueNode{
    Task *task;
    struct QueueNode *next;
    long deploy_tm;
} QueueNode;


#endif