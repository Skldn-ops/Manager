#include "headl.h"


void mergeSortList(QueueNode** head);
QueueNode* insert_QueueNode(QueueNode* head, QueueNode* newQueueNode);
void queue_control(void);


//      start program_to_exec delay timeout

int start_fd[2];

int main(void)
{
    char input[MAX_LEN];

    // QueueNode *head = NULL;
    //QueueNode *tail = NULL;

    unsigned long long id_maker = 0;
    
    pipe(start_fd);
    pid_t queue_control_pid = fork();
    if(!queue_control_pid)
    {
        queue_control();
    }
    close(start_fd[0]);

    while(1)
    {
        printf("> ");

        if (fgets(input, MAX_LEN, stdin) == NULL) {
            clearerr(stdin);
            printf("\n");
            continue;
        }

        size_t len = strlen(input);
        if (len > 0 && input[len - 1] == '\n') {
            input[len - 1] = '\0';
        }
        

        if (strlen(input) == 0) {
            continue;
        }
        
        char command[MAX_LEN];
        char program_to_exec[MAX_LEN];
        time_t delay;
        time_t timeout;
        sscanf(input, "%s", command);

        if(strcmp(command, "start") == 0)
        {
            id_maker++;

            sprintf(input + strlen(input), " %llu", id_maker);

            write(start_fd[1], input, sizeof(input));
            kill(queue_control_pid, SIGUSR1);
        }
    }
}
