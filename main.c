#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#include "headl.h"

int main(void)
{
    char input[MAX_LEN];
//      start name delay timeout
    while(1)
    {
        unsigned long long id_maker = 0;
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
        sscanf(input, "%s %s %ld %ld", command, name, &delay, &timeout);

        if(strcmp(command, "start") == 0)
        {

        }


    }
}
