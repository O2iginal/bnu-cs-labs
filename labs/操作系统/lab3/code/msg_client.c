#include <stdio.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <unistd.h>

#define MSGKEY 75
#define SERVER 1

struct msgform
{
    long mtype;       // 消息类型
    int mfrom;        // 消息发送者PID
    int mindex;       // 消息编号
    char mtext[1016]; // 消息文本
    // 其中 mtype 为消息队列中消息结构的固定内容，不计入消息大小
    // mform、mindex、mtext 大小共1024B，符合实验1K大小的消息要求
} msg;

void client(int clientid)
{
    int msgqid, pid;
    // 申请得到key=75的消息队列（如果不存在，则重复申请）
    while ((msgqid = msgget(MSGKEY, 0777)) == -1)
        ;
    // 获得当前client进程pid
    pid = getpid();
    // 依此发送编号10~1的消息
    for (int i = 10; i >= 1; i--)
    {
        // 设置消息信息
        msg.mfrom = pid;
        msg.mtype = SERVER;
        msg.mindex = i;
        // client向server发送消息
        msgsnd(msgqid, &msg, sizeof(msg) - sizeof(long), 0);
        printf("(CLIENT%d %d) sent message %d\n", clientid, pid, i);
        // client接收server的返回消息
        msgrcv(msgqid, &msg, sizeof(msg) - sizeof(long), pid, 0);
        printf("(CLIENT%d %d) received return message %d from SERVER %d\n",
                                     clientid, pid, msg.mindex, msg.mfrom);
    }
}

int main(int argc, char *argv[])
{
    int clientid = atoi(argv[1]);
    client(clientid);
    return 0;
}
