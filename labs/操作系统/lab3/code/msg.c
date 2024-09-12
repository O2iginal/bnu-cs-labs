#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    int child_process_num = 2;
    int i;
    pid_t pid;
    // 创建2个子进程
    for (i = 0; i < child_process_num; i++)
    {
        while ((pid = fork()) < 0)
            ;
        if (pid == 0)
            break;
    }
    // 判断当前进程，选择对应程序执行
    if (pid > 0)
    {
        // 父进程，执行服务器程序
        char *argv_list[] = {"./msg_server", NULL};
        execv(argv_list[0], argv_list);
    }
    else
    {
        if (i == 0)
        {
            // 子进程1，执行客户端程序，传入参数1
            char *argv_list[] = {"./msg_client", "1", NULL};
            execv(argv_list[0], argv_list);
        }
        else if (i == 1)
        {
            // 子进程2，执行客户端程序，传入参数2
            char *argv_list[] = {"./msg_client", "2", NULL};
            execv(argv_list[0], argv_list);
        }
    }
    return 0;
}
