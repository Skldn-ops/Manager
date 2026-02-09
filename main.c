#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#include "headl.h"


void mergeSortList(QueueNode** head);
QueueNode* insert_QueueNode(QueueNode* head, QueueNode* newQueueNode);

int main(void)
{
    char input[MAX_LEN];
//      start name delay timeout
    QueueNode *head = NULL;
    QueueNode *tail = NULL;
    unsigned long long id_maker = 0;

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
        char name[MAX_LEN];
        time_t delay;
        time_t timeout;
        sscanf(input, "%s", command);

        if(strcmp(command, "start") == 0)
        {
            sscanf(input + strlen(command), "%s %ld %ld", name, &delay, &timeout);
            
            Task *task = malloc(sizeof(Task));
            task->created_at = time(NULL);
            task->delay = delay;
            task->exit_code = -1;
            task->finished_at = 0;
            task->id = id_maker++;
            strcpy(task->program_to_exec, name);
            task->scheduled_at = task->created_at + delay;
            task->started_at = 0;
            task->state = (delay==0) ? TASK_PENDING : TASK_DELAYED;
            task->timeout = timeout;

            QueueNode *node = malloc(sizeof(QueueNode));
            node->deploy_tm = task->scheduled_at - time(NULL);
            node->task = task;
            node->next = NULL;
            if(head != NULL)
            {
                mergeSortList(&head);
                head = insert_QueueNode(head, node);
            }
            else
            {
                head = node;
            }
        }

    }
}
