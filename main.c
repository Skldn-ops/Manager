#include "headl.h"


void queue_control(int log_fd);
void sem_lock(int semid, unsigned short pos);
void sem_unlock(int semid, unsigned short pos);

void show_table(Task* shared, int id_maker);
void show_line(Task* shared, char* input, int id_maker);

pid_t queue_control_pid = -1;
//      start program_to_exec delay timeout
void queue_killed(int sig)
{
    system("gnome-terminal -- bash -c \"echo 'Queue was killed'; exec bash\"");
    waitpid(queue_control_pid, NULL, WNOHANG);
    queue_control_pid = -1;
    signal(SIGCHLD, SIG_IGN);
}

int main(void)
{
    //signal(SIGCHLD, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);

    int log_fd = open("./LOG_FILE.txt", O_CREAT | O_WRONLY | O_APPEND, 0666);
    char input[MAX_LEN];

    long long id_maker = -1;
    int fd = open("/tmp/myid_maker", O_RDONLY);
    if(fd > 0)
    {
        read(fd, &id_maker, sizeof(id_maker));
        close(fd);
        printf("Restored the id_maker %lld\n", id_maker);
    }

        
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

        if(strcmp(command, "start") == 0 && queue_control_pid > 0)
        {
            id_maker++;
            id_maker %= MAX_PROGRAMMS_RUN;
            fd = open("/tmp/myid_maker", O_TRUNC | O_WRONLY | O_CREAT, 0644);
            write(fd, &id_maker, sizeof(id_maker));
            close(fd);

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
            
            fd = open("/tmp/myservpid", O_RDONLY);
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
                sem_lock(semid, LOG_FD);
                dprintf(log_fd, "CONNECTED TO QUEUE\n");
                sem_unlock(semid, LOG_FD);
            }
            
        }
        else if(strcmp(command, "queue") == 0 && queue_control_pid == -1)
        {
            queue_control_pid = fork();
            if(!queue_control_pid)
            {
                queue_control(log_fd);
            }
            printf("queue ready\n");
            sem_lock(semid, LOG_FD);
            dprintf(log_fd, "QUEUE STARTED\n");
            sem_unlock(semid, LOG_FD);
            signal(SIGCHLD, queue_killed);
        }
        else if((strcmp(command, "table") == 0) && (shared != NULL))
        {
            sem_lock(semid, TASK_TAB);
            show_table(shared, id_maker);
            sem_unlock(semid, TASK_TAB);
        }
        else if((strcmp(command, "line") == 0) && (shared != NULL))
        {
            sem_lock(semid, TASK_TAB);
            show_line(shared, input, id_maker);
            sem_unlock(semid, TASK_TAB);
        }
        else if(strcmp(command, "exit") == 0)
        {
            signal(SIGCHLD, SIG_IGN);
            if(queue_control_pid > 0)
                kill(queue_control_pid, SIGKILL);
            
            printf("Queue offline\n");

            unlink("/tmp/myservpid");
            unlink("/tmp/mysocket");
            unlink("/tmp/myshmid");
            unlink("/tmp/mysemid");
            unlink("/tmp/myid_maker");
            break;
        }
    }
    
    dprintf(log_fd, "EXIT SUCCESS\n");
    
    printf("Exited\n");
    shmdt(shared);
    return 0;
}
