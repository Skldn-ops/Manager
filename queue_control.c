#include "headl.h"


QueueNode *head_glob = NULL;
extern start_fd[2];


void handl_start(int sig)
{
    char input[MAX_LEN];
    read(start_fd[0], input, sizeof(input));

    char program_to_exec[MAX_LEN];
    time_t delay;
    time_t timeout;
    unsigned long long id_maker;

    sscanf(input + strlen("start"), "%s %ld %ld %llu", program_to_exec, &delay, &timeout, &id_maker);
            
    Task *task = malloc(sizeof(Task));
    task->created_at = time(NULL);
    task->delay = delay;
    task->exit_code = -1;
    task->finished_at = -1;
    task->id = id_maker;
    strcpy(task->program_to_exec, program_to_exec);
    task->scheduled_at = task->created_at + delay;
    task->started_at = -1;
    task->state = (delay==0) ? TASK_PENDING : TASK_DELAYED;
    task->timeout = timeout;

    QueueNode *node = malloc(sizeof(QueueNode));
    node->deploy_tm = task->scheduled_at - time(NULL);  //Сколько осталось до запуска
    node->task = task;
    node->next = NULL;
    if(head_glob != NULL)
    {
        mergeSortList(&head_glob);
        head_glob = insert_QueueNode(head_glob, node);
    }
    else
    {
        head_glob = node;
    }
}


void queue_control(void)
{
    close(start_fd[1]);
    signal(SIGUSR1, handl_start);
    
    while(1)
    {
        QueueNode *head_loc = head_glob;
        while(head_loc != NULL)
        {
            head_loc->deploy_tm = head_loc->task->scheduled_at - time(NULL);
            head_loc = head_loc->next;
        }
    }
}