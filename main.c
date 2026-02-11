#include "headl.h"


void queue_control(void);


//      start program_to_exec delay timeout

//int start_fd[2];

int main(void)
{
    char input[MAX_LEN];

    // QueueNode *head = NULL;
    //QueueNode *tail = NULL;

    unsigned long long id_maker = 0;
    
    // pipe(start_fd);
    pid_t queue_control_pid;
    // close(start_fd[0]);
        
    int sock;
    struct sockaddr_un server;
    

    
    
    
    //////////////////
    //printf("Queue ready: prog pid is %d and queue oid is %d\n", getpid(), queue_control_pid);
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

            send(sock, input, sizeof(input), 0);
            kill(queue_control_pid, SIGUSR1);
        }
        else if(strcmp(command, "connect") == 0)
        {
            sock = socket(AF_UNIX, SOCK_STREAM, 0);
    
            memset(&server, 0, sizeof(server));
            server.sun_family = AF_UNIX;
            strcpy(server.sun_path, "/tmp/mysocket");
            
            int fd = open("/tmp/myservpid", O_RDONLY);
            read(fd, &queue_control_pid, sizeof(queue_control_pid));
            close(fd);

            kill(queue_control_pid, SIGUSR2);
            if(!connect(sock, (struct sockaddr*)&server, sizeof(server)))
            {
                printf("connected to queue\n");
            }
            
            // send(sock, "Hello over UNIX socket", 23, 0);
            
            // // 5. Получение ответа
            // recv(sock, buffer, sizeof(buffer), 0);
            // printf("Server: %s\n", buffer);
            
            // // 6. Закрытие сокета
            // close(sock);
        }
        else if(strcmp(command, "queue") == 0)
        {
            queue_control_pid = fork();
            if(!queue_control_pid)
            {
                queue_control();
            }
            printf("Queue online\n");
        }
        else if(strcmp(command, "exit") == 0)
        {
            kill(queue_control_pid, SIGKILL);
            printf("Queue offline\n");
            unlink("/tmp/myservpid");
            unlink("/tmp/mysocket");
            break;
        }
    }
    printf("Exited\n");
    return 0;
}
