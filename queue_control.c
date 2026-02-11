#include "headl.h"

QueueNode *head_glob = NULL;
void mergeSortList(QueueNode** head);
QueueNode* insert_QueueNode(QueueNode* head, QueueNode* newQueueNode);
QueueNode* deleteNodeByPtr(QueueNode* head, QueueNode* nodeToDelete);
int server_fd, client_fd;

void handl_start(int sig)
{
    char input[MAX_LEN];
    recv(client_fd, input, sizeof(input), 0);

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


void handl_reconnect(int sig)
{
    client_fd = accept(server_fd, NULL, NULL);
}

void queue_control(void)
{
    signal(SIGUSR1, handl_start);
    signal(SIGCHLD, SIG_IGN); // Чтобы завершившиеся контролирующие процессы не висели как зомби
    signal(SIGHUP, SIG_IGN);
    signal(SIGUSR2, handl_reconnect);

    struct sockaddr_un addr;
    
    pid_t pid_saver = getpid();
    int fd = open("/tmp/myservpid", O_TRUNC | O_WRONLY | O_CREAT, 0644);
    write(fd, &pid_saver, sizeof(pid_saver));
    close(fd);
    
    unlink("/tmp/mysocket");
    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, "/tmp/mysocket");
    
    bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
    
    listen(server_fd, 5);
    
    //client_fd = accept(server_fd, NULL, NULL);
    
    // // 7. Получение данных
    // recv(client_fd, buffer, sizeof(buffer), 0);
    // printf("Client: %s\n", buffer);
    
    // 8. Отправка ответа
    // send(client_fd, "Hello from UNIX server", 23, 0);
    
    // // 9. Закрытие
    // close(client_fd);
    // close(server_fd);
    // unlink("/tmp/mysocket");


    pid_t table[MAX_PROGRAMMS_RUN]; //индекс в массиве - ячейка, в которой лежит PID процесса с (id = индекс в массиве)
    while(1)
    {
        QueueNode *head_loc = head_glob;
        while(head_loc != NULL)
        {
            head_loc->deploy_tm = head_loc->task->scheduled_at - time(NULL);
            if(head_loc->deploy_tm <= 0)
            {
                char program_to_exec_tp[MAX_LEN];
                strcpy(program_to_exec_tp, head_loc->task->program_to_exec);
                unsigned int timeout_tp = head_loc->task->timeout;

                head_glob = deleteNodeByPtr(head_glob, head_loc);
                head_loc = NULL;

                pid_t controlling_pid = fork();
                if(!controlling_pid)
                {
                    int status = -1;
                    pid_t executor_pid = fork();
                    if(!executor_pid)
                    {
                        execlp(program_to_exec_tp, program_to_exec_tp, NULL);
                    }
                    
                    sleep(timeout_tp);
                    int ret = waitpid(executor_pid, &status, WNOHANG);
                    if(ret)
                    {
                        ret = WEXITSTATUS(status);
                    }
                    else
                    {
                        kill(executor_pid, SIGKILL);
                    }
                    _exit(0);
                }
            }
            else
            {
                head_loc = head_loc->next;
            }
        }
    }
}