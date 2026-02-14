#include "headl.h"

QueueNode *head_glob = NULL;
void mergeSortList(QueueNode** head);
QueueNode* insert_QueueNode(QueueNode* head, QueueNode* newQueueNode);
QueueNode* deleteNodeByPtr(QueueNode* head, QueueNode* nodeToDelete);
void sem_lock(int semid, unsigned short pos);
void sem_unlock(int semid, unsigned short pos);
int server_fd, client_fd;
int need_add_glob = 0;

void ping(int sig){}

void handl(int sig)
{
    need_add_glob++;
}

void start(Task *shared, int semid, int log_fd)
{
    char input[MAX_LEN];
    recv(client_fd, input, sizeof(input), 0);

    char program_to_exec[MAX_LEN];
    time_t delay;
    time_t timeout;
    long long id_maker;

    sscanf(input + strlen("start"), "%s %ld %ld %lld", program_to_exec, &delay, &timeout, &id_maker);
            
    Task *task = malloc(sizeof(Task));
    task->created_at = time(NULL);
    task->delay = delay;
    task->exit_code = -1;
    task->finished_at = 0;
    task->id = id_maker;
    strcpy(task->program_to_exec, program_to_exec);
    task->scheduled_at = task->created_at + delay;
    task->started_at = 0;
    task->state = TASK_DELAYED;
    task->timeout = timeout;
    //////
    sem_lock(semid, TASK_TAB);
    shared[id_maker] = *task;
    sem_unlock(semid, TASK_TAB);
    //////
    sem_lock(semid, LOG_FD);
    dprintf(log_fd, "Task: %s with id: %lld saved\n", task->program_to_exec, task->id);
    sem_unlock(semid, LOG_FD);
    //////
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
    sem_lock(semid, LOG_FD);
    dprintf(log_fd, "Task: %s with id: %lld put to queue\n", task->program_to_exec, task->id);
    sem_unlock(semid, LOG_FD);
}


void handl_reconnect(int sig)
{
    client_fd = accept(server_fd, NULL, NULL);
}


void queue_control(int log_fd)
{
    //prctl(PR_SET_NAME, "prog_queue", 0, 0, 0);
    int shmid;
    struct Task *shared;
    int semid;
    int fd = open("/tmp/myshmid", O_RDONLY);
    if(fd < 0)
    {
        shmid = shmget(IPC_PRIVATE, sizeof(Task) * MAX_PROGRAMMS_RUN, IPC_CREAT | 0666);
        shared = shmat(shmid, NULL, 0);
        memset(shared, 0, sizeof(shared));

        semid = semget(IPC_PRIVATE, 2, IPC_CREAT | 0666);
        semctl(semid, 0, SETVAL, 1);
        semctl(semid, 1, SETVAL, 1);

        fd = open("/tmp/myshmid", O_TRUNC | O_WRONLY | O_CREAT, 0644);
        write(fd, &shmid, sizeof(shmid));
        close(fd);
        fd = open("/tmp/mysemid", O_TRUNC | O_WRONLY | O_CREAT, 0644);
        write(fd, &semid, sizeof(semid));
        close(fd);
    }
    else
    {
        read(fd, &shmid, sizeof(shmid));
        close(fd);
        shared = shmat(shmid, NULL, 0);
        fd = open("/tmp/mysemid", O_RDONLY);
        read(fd, &semid, sizeof(semid));
        close(fd);
    }

    //dprintf(out_fd, "queue online\n");
    
    signal(SIGUSR1, handl);
    signal(SIGCHLD, SIG_IGN); // Чтобы завершившиеся контролирующие процессы не висели как зомби
    signal(SIGHUP, SIG_IGN);    // Чтобы не умер при смерти main
    signal(SIGUSR2, handl_reconnect);

    struct sockaddr_un addr;
    
    pid_t pid_saver = getpid();
    fd = open("/tmp/myservpid", O_TRUNC | O_WRONLY | O_CREAT, 0644);
    write(fd, &pid_saver, sizeof(pid_saver));
    close(fd);
    
    unlink("/tmp/mysocket");
    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, "/tmp/mysocket");
    
    bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
    
    listen(server_fd, 5);

    while(1)
    {
        while(need_add_glob > 0)
        {
            start(shared, semid, log_fd);
            need_add_glob--;
        }
        if(head_glob == NULL)
        {
            pause();
            //dprintf(out_fd, "GOT SIGNAL\n");
        }
        QueueNode *head_loc = head_glob;
        if(head_loc != NULL && head_loc->deploy_tm > 1)
            sleep(head_loc->deploy_tm - 1);
        
        while(head_loc != NULL)
        {
            head_loc->deploy_tm = head_loc->task->scheduled_at - time(NULL);
            if(head_loc->deploy_tm <= 0)
            {
                char program_to_exec_tp[MAX_LEN];
                strcpy(program_to_exec_tp, head_loc->task->program_to_exec);
                unsigned int timeout_tp = head_loc->task->timeout;
                QueueNode *temp_tp = head_loc->next;
                long long id = head_loc->task->id;

                head_glob = deleteNodeByPtr(head_glob, head_loc);
                head_loc = NULL;
                sem_lock(semid, LOG_FD);
                dprintf(log_fd, "Task: %s with id: %lld out of queue\n", program_to_exec_tp, id);
                sem_unlock(semid, LOG_FD);

                pid_t controlling_pid = fork();
                if(!controlling_pid)
                {
                    signal(SIGCHLD, ping); //Не висит до конца таймаута, вовремя фикс конец executer
                    signal(SIGHUP, SIG_IGN);
                    int status = -1;
                    int exit_code = -1;

                    sem_lock(semid, TASK_TAB);
                    shared[id].state = TASK_RUNNING;
                    shared[id].started_at = time(NULL);
                    sem_unlock(semid, TASK_TAB);
                    ///////
                    sem_lock(semid, LOG_FD);
                    dprintf(log_fd, "Task: %s with id: %lld execution started\n", program_to_exec_tp, id);
                    sem_unlock(semid, LOG_FD);
                    ///////
                    pid_t executor_pid = fork();
                    if(!executor_pid)
                    {
                        shmdt(shared);
                        execlp(program_to_exec_tp, program_to_exec_tp, NULL);
                    }
                    
                    sleep(timeout_tp);
                    int ret = waitpid(executor_pid, &status, WNOHANG);
                    if(ret)
                    {
                        exit_code = WEXITSTATUS(status);
                    }
                    else
                    {
                        kill(executor_pid, SIGKILL);
                    }
                    sem_lock(semid, TASK_TAB);
                    if(ret && !exit_code)
                    {
                        shared[id].state = TASK_COMPLETED;
                        shared[id].exit_code = exit_code;
                    }
                    else if(ret) // && exit_code
                    {
                        shared[id].state = TASK_FAILED;
                        shared[id].exit_code = exit_code;
                    }
                    else if(!ret)
                    {
                        shared[id].state = TASK_TIMEOUT;
                        shared[id].exit_code = -1;
                    }
                    shared[id].finished_at = time(NULL);
                    sem_unlock(semid, TASK_TAB);
                    ///////
                    sem_lock(semid, LOG_FD);
                    dprintf(log_fd, "Task: %s with id: %lld execution done\n", program_to_exec_tp, id);
                    sem_unlock(semid, LOG_FD);
                    ///////
                    shmdt(shared);
                    exit(0);
                }
            }
            else
            {
                head_loc = head_loc->next;
            }
        }
    }
}