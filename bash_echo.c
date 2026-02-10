#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

int main(void)
{
    system("gnome-terminal -- bash -c \"echo 'Hello'; exec bash\"");

    return 0;
}