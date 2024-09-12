#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char * argv[])
{
    int child_process_num = 2;
    int i;
    pid_t pid;
    // printf("something before fork in parent process --- \n");
    // printf("something before fork() in parent process --- ");

    for (i = 0; i < child_process_num && pid > 0; i ++)
    {
        pid = fork();
    }

    if (pid < 0)  
    {   // fork failed !
        printf("fork failed!\n");
        exit(-1);
    }
    else if(pid > 0)  
    {   // parent process
        lockf(1, 1, 0);    // lock
        printf("the parent process\n");
        lockf(1, 0, 0);    // unlock
    }
    else  
    {   // pid == 0, child process

        if (i == 1)  
        {   // 1st child process
            lockf(1, 1, 0);    // lock
            printf("the 1st child process\n");
            lockf(1, 0, 0);    // unlock
        }
        else if (i == 2)  
        {   // 2ed child process
            lockf(1, 1, 0);    // lock
            printf("the 2ed child process\n");
            lockf(1, 0, 0);    // unlock
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