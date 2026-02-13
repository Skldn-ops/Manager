#include "headl.h"

void get_state(TaskState state, char* ans)
{
    if(state == TASK_COMPLETED)
    {
        strcpy(ans, "COMPLETED");
    }
    else if(state == TASK_RUNNING)
    {
        strcpy(ans, "RUNNING");
    }
    else if(state == TASK_DELAYED)
    {
        strcpy(ans, "DELAYED");
    }
    else if(state == TASK_FAILED)
    {
        strcpy(ans, "FAILED");
    }
    else if(state == TASK_TIMEOUT)
    {
        strcpy(ans, "TIMEOUT");
    }
}

void show_table(Task* shared, int id_maker)
{
    char state[16];
    char unknown = '-';
    printf("id | name | status    | exit_code | runtime\n");
    for(int i = 0; i <= id_maker; i++)
    {
        get_state(shared[i].state, state);

        printf("%-2lld | %-4s | %-9s |", shared[i].id, shared[i].program_to_exec, state);
        if(shared[i].exit_code != -1)
            printf(" %-9d |", shared[i].exit_code);
        else
            printf(" %-9c |", unknown);
        if(shared[i].finished_at > 0)
            printf(" %llu\n", shared[i].finished_at - shared[i].started_at);
        else
            printf(" %c\n", unknown);
    }
}


void show_line(Task* shared, char* input, int id_maker)
{
    char state[16];
    char unknown = '-';
    char pattern[MAX_LEN];
    sscanf(input +strlen("line"), "%s", pattern);
    printf("id | name | status    | exit_code | runtime\n");
    for(int i = 0; i <= id_maker; i++)
    {
        if(strcmp(shared[i].program_to_exec, pattern) == 0)
        {    
            get_state(shared[i].state, state);

            printf("%-2lld | %-4s | %-9s |", shared[i].id, shared[i].program_to_exec, state);
            if(shared[i].exit_code != -1)
                printf(" %-9d |", shared[i].exit_code);
            else
                printf(" %-9c |", unknown);
            if(shared[i].finished_at > 0)
                printf(" %llu\n", shared[i].finished_at - shared[i].started_at);
            else
                printf(" %c\n", unknown);
        }
    }
}