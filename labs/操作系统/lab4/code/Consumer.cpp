#include "./my_base.h"

void consume()
{
    // 申请满缓冲区
    semaphore_P(semid, 2);
    // 申请缓冲区互斥操作权限
    semaphore_P(semid, 0);

    // 进行缓冲区内互斥操作，装入产品
    buffer->pop_front(buf);

    // 释放缓冲区控制权限
    semaphore_V(semid, 0);
    // 增加空缓冲区个数
    semaphore_V(semid, 1);

    // 输出消费产品内容
    printf("消费产品内容 : %s\n", buf);
    // 输出操作后缓冲区状态
    buffer->print();
}

int main(int argc, char *argv[])
{
    // 获取共享内存区
    shmid = get_share_memory(SHMKEY, 0);
    // 获取信号量（3个，分别为缓冲区互斥信号量、满缓冲区数量，空缓冲区数量）,并赋初值
    semid = get_semaphore(SEMKEY, 0);
    // 输出缓冲区初始状态
    buffer->print();

    // 进入消费者操作循环
    while (1)
    {
        printf("可选择操作：[1] 消费产品；[2] 退出进程；[3] 删除信号量和共享内存；\n"
               "输入操作编号 > ");
        scanf("%s", buf);
        if (buf[0] == '1')
            consume();
        else if (buf[0] == '2')
            break;
        else if (buf[0] == '3')
        {
            delete_sem_and_shm();
            break;
        }
        else
            printf("输入格式操作！重新输入……\n");

        printf("\n");
    }
    printf("\nConsumer 退出进程。\n");
    return 0;
}
