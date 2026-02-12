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

#ifndef DEF
#define DEF

#define MAX_LEN 1024
#define MAX_PROGRAMMS_RUN 100
#define ID_INFO_SIZE 25

typedef enum {
    TASK_PENDING,
    TASK_DELAYED,
    TASK_READY,
    TASK_RUNNING,
    TASK_COMPLETED,
    TASK_FAILED,
    TASK_TIMEOUT,
    TASK_CANCELLED
} TaskState;

typedef struct {
    unsigned long long id;
    unsigned long long created_at;
    unsigned long long scheduled_at;
    unsigned long long started_at;
    unsigned long long finished_at;
    TaskState state;
    unsigned int delay;
    unsigned int timeout;
    int exit_code;
    char program_to_exec[MAX_LEN];
} Task;


typedef struct QueueNode{
    Task *task;
    struct QueueNode *next;
    unsigned deploy_tm;
} QueueNode;


#endif