#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char * argv[])
{
    int child_process_num = 2;
    int i;
    pid_t pid;

    for (i = 0; i < child_process_num; i ++)
    {
        while((pid = fork()) < 0)
            ; 
        if (pid == 0) break;
    }

    if (pid < 0)  
    {   // fork failed !
        printf("fork failed!\n");
        exit(-1);
    }
    else if(pid > 0)  
    {   // parent process
        printf("这是父进程!\n");
    }
    else  
    {   // pid == 0, child process
        if (i == 0)  
        {   // 1st child process, 执行ls程序
            char *argv_list[] = {"ps", "-l", "-a", NULL};
            int res = execv("/bin/ps",argv_list);
            if (res < 0)
                printf("ls exec error!\n");
        }
        else if (i == 1)  
        {   // 2ed child process， 执行helloworld程序
            char *argv_list[] = {"helloworld", NULL};
            int res = execv("./helloworld",argv_list);
            if (res < 0)
                printf("helloworld exec error!\n");
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
            ;
        printf("\n");
    }

    return 0;
}