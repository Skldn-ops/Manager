#include "headl.h"


void queue_control(int out_fd);
void sem_lock(int semid);
void sem_unlock(int semid);

void show_table(Task* shared, int id_maker);
void show_line(Task* shared, char* input, int id_maker);

//      start program_to_exec delay timeout


int main(void)
{
    int out_fd = dup(1);
    char input[MAX_LEN];

    long long id_maker = -1;
    pid_t queue_control_pid;
        
    int sock;
    struct sockaddr_un server;

    
    int shmid;
    Task* shared = NULL;
    int semid;
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
            id_maker %= MAX_PROGRAMMS_RUN;

            sprintf(input + strlen(input), " %lld", id_maker);

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
            fd = open("/tmp/myshmid", O_RDONLY);
            read(fd, &shmid, sizeof(shmid));
            close(fd);
            shared = shmat(shmid, NULL, 0);
            fd = open("/tmp/mysemid", O_RDONLY);
            read(fd, &semid, sizeof(semid));
            close(fd);

            kill(queue_control_pid, SIGUSR2);
            if(!connect(sock, (struct sockaddr*)&server, sizeof(server)))
            {
                printf("connected to queue\n");
            }
            
        }
        else if(strcmp(command, "queue") == 0)
        {
            queue_control_pid = fork();
            if(!queue_control_pid)
            {
                queue_control(out_fd);
            }
            printf("queue ready\n");
        }
        else if((strcmp(command, "table") == 0) && (shared != NULL))
        {
            sem_lock(semid);
            show_table(shared, id_maker);
            sem_unlock(semid);
        }
        else if((strcmp(command, "line") == 0) && (shared != NULL))
        {
            sem_lock(semid);
            show_line(shared, input, id_maker);
            sem_unlock(semid);
        }
        else if(strcmp(command, "exit") == 0)
        {
            kill(queue_control_pid, SIGKILL);
            printf("Queue offline\n");

            unlink("/tmp/myservpid");
            unlink("/tmp/mysocket");
            unlink("/tmp/myshmid");
            unlink("/tmp/mysemid");
            break;
        }
    }
    printf("Exited\n");
    shmdt(shared);
    return 0;
}
