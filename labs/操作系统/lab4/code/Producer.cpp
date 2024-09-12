#include "./my_base.h"

// 操作1，生产产品
void produce()
{
    // 读入生产产品
    printf("输入产品内容 > ");
    scanf("%s", buf);

    semaphore_P(semid, 1);  // 申请空缓冲区
    semaphore_P(semid, 0);  // 申请缓冲区互斥操作权限

    // 进行缓冲区内互斥操作，装入产品
    buffer->push_back(buf);
    
    semaphore_V(semid, 0);  // 释放缓冲区控制权限
    semaphore_V(semid, 2);  // 增加满缓冲区个数

    // 输出操作后缓冲区状态
    buffer->print();
}

int main(int argc, char *argv[])
{
    // 获取共享内存区
    shmid = get_share_memory(SHMKEY);
    // 获取信号量（3个，分别为缓冲区互斥信号量、满缓冲区数量，空缓冲区数量）,并赋初值
    semid = get_semaphore(SEMKEY);
    // 输出缓冲区初始状态
    buffer->print();

    // 进入生产者操作循环
    while (1)
    {
        printf("可选择操作：[1] 生产产品；[2] 退出进程；[3] 删除信号量和共享内存；\n输入操作编号 > ");
        scanf("%s", buf);
        if (buf[0] == '1')
            produce();
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

    printf("\nProducer 退出进程。\n");

    return 0;
}
