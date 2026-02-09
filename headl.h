

#ifndef DEF
#define DEF

#define MAX_LEN 1024

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
    //char name[MAX_LEN];
    
    TaskState state;
        
    unsigned long long created_at;
    unsigned long long scheduled_at;
    unsigned long long started_at;
    unsigned long long finished_at;
    unsigned int delay;
    unsigned int timeout;
    
    char program_to_exec[1024];
    int exit_code;
} Task;


typedef struct QueueNode{
    Task *task;
    struct QueueNode *next;
    unsigned deploy_tm;
} QueueNode;


#endif
