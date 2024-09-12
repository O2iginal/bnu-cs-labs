#include <stdlib.h>
#include <stdio.h>
#include <sys/msg.h>
#include <signal.h>
#include <unistd.h>

#define MSGKEY 75  // 用于消息队列KEY=75，双方使用此消息队列通信
#define SERVER 1   // server接收类型为1的消息

struct msgform
{
    long mtype;       // 消息类型
    int mfrom;        // 消息发送者PID
    int mindex;       // 消息编号
    char mtext[1016]; // 消息文本
    // 其中 mtype 为消息队列中消息结构的固定内容，不计入消息大小
    // mform、mindex、mtext 大小共1024B，符合实验1K大小的消息要求
} msg;

int msgqid;

void cleanup(int i)
{
    msgctl(msgqid, IPC_RMID, 0);
    exit(0);
}

int main()
{
    int i, pid;
    // 软中断处理
    for (i = 0; i < 20; i++)
        signal(i, cleanup);
    // 获得当前server进程pid
    pid = getpid();
    // 重复尝试创建消息队列（若成功，必定是有server创建此消息队列）
    while ((msgqid=msgget(MSGKEY,0777|IPC_CREAT|IPC_EXCL))==-1)  
        ;
    printf("(SERVER  %d) created message queue with key %d\n", pid, MSGKEY);

    // cnt1 为server收到的消息编号为1的数量，若累计到2个，则结束服务器进程
    int cnt1 = 0;
    while (cnt1 < 2)
    {
        // 服务器接收类型为SERVER=1的消息
        msgrcv(msgqid, &msg, sizeof(msg) - sizeof(long), 1, 0);
        printf("(SERVER  %d) received message %d from CLIENT %d\n", pid, msg.mindex, msg.mfrom);
        // 检查收到的消息编号是否为1，如果是，则累加到cnt1
        if (msg.mindex == 1)
            cnt1++;
        // 写入返回消息信息，消息类型应为此消息的发送者client的pid（使得对应client能够收到）
        msg.mtype = msg.mfrom;
        msg.mfrom = pid;
        // server发送返回消息给对应的client
        msgsnd(msgqid, &msg, sizeof(msg) - sizeof(long), 0);
        printf("(SERVER  %d) sent return message %d of CLIENT %ld\n", pid, msg.mindex, msg.mtype);
    }

    return 0;
}
