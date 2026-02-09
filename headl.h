#include <cstdint>

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

typedef struct Task {
    unsigned long long id;
    char name[MAX_LEN];
    TaskState state;
    

    uint64_t created_at;
    uint64_t scheduled_at;
    uint64_t started_at;
    uint64_t finished_at;
    unsigned int delay;
    unsigned int timeout;
    
    char program_to_exec[1024];

    
    // Результат
    char result[MAX_LEN];
    char error[512];
    int exit_code;
} Task;


#endif