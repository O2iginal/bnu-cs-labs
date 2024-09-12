#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char * argv[])
{
    int child_process_num = 2;
    int line_num = 2;
    int i;
    pid_t pid;
    // printf("something before fork in parent process --- \n");
    // printf("something before fork() in parent process --- ");
    for (i = 0; i < child_process_num; i ++)
    {
        pid = fork();
        if (pid == 0) break;
    }

    if (pid < 0)  
    {   // fork failed !
        printf("fork failed!\n");
        exit(-1);
    }
    else if(pid > 0)  
    {   // parent process
        for (int j = 1; j <= line_num; j ++)
        {
            lockf(1, 1, 0);    // lock
            printf("No.%d line of text in parent process\n", j);
            lockf(1, 0, 0);    // unlock
        }
    }
    else  
    {   // pid == 0, child process
        if (i == 0)  
        {   // 1st child process
            for (int j = 1; j <= line_num; j ++)
            {
                lockf(1, 1, 0);    // lock
                printf("No.%d line of text in the 1th child process\n", j);
                lockf(1, 0, 0);    // unlock
            }
        }
        else if (i == 1)  
        {   // 2ed child process
            for (int j = 1; j <= line_num; j ++)
            {
                lockf(1, 1, 0);    // lock
                printf("No.%d line of text in the 2ed child process\n", j);
                lockf(1, 0, 0);    // unlock
            }
        }
        else
        {
            printf("error!\n");
            exit(-1);
        }
    }

    if (pid > 0)
    {   // parent process        
        int status;
        pid_t wait_pid;
        while ((wait_pid = wait(&status)) != -1) 
        {
            
        }
        printf("\n");
    }

    return 0;
}