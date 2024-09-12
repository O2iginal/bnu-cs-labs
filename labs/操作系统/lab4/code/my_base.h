#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <cstring>
#include "./my_buffer.h"
#include "./my_semaphore.h"

#define SHMKEY 75
#define SEMKEY 75

char buf[BUFFER_CELL_SIZE]; // 缓存输入字符串
int shmid;                  // 共享内存id
int semid;                  // 信号量id
Buffer *buffer;             // 共享内存中缓冲区指针

// 获得共享内存区，进行初始化，返回其id
int get_share_memory(int key, int create = 1)
{
    // 尝试获取，如果不存在则返回-1
    int t_shmid = shmget(key, sizeof(Buffer), 0777);  
    // 如果返回-1，则说明需要创建此共享内存
    int isInitial = t_shmid == -1;
    // 消费者调用此函数时，create=0，不能由消费者创建共享内存
    if (!create && isInitial)
    {
        printf("消费者获取 KEY=%d 的共享内存失败，需要通过生产者创建！\n", key);
        exit(1);
    }
    // 尝试创建共享内存
    if (isInitial && (t_shmid = shmget(key, sizeof(Buffer), 0777 | IPC_CREAT)) == -1)
    {
        printf("Share Memory Creat Failed!\n");
        exit(1);
    }
    //  获得共享内存起始位置指针（转为BufferCell指针，方便存取操作）
    buffer = (Buffer *)shmat(t_shmid, 0, 0);
    //  初始化（如果是初次创建共享内存，则初始化， 队列的头尾指针被赋值为0）
    if (isInitial)
    {
        printf("初次创建 KEY=%d 的共享内存，共享内存初始化……\n", key);
        memset(buffer, 0, sizeof(Buffer));
    }
    else
        printf("已存在 KEY=%d 的共享内存，直接获取……\n", key);

    return t_shmid;
}

// 获取信号量（3个）, 赋初值，返回其id
// key为信号量数组创建键值； create为1表示不存在则创建，为0不存在则报错
int get_semaphore(int key, int create = 1)
{
    // 本实验共需3个信号量
    int num_semaphores = 3;
    // 尝试获取，如果不存在则返回-1
    int t_semid = semget(key, num_semaphores, 0777);  
    // 如果返回-1，则说明需要创建此信号量
    int isInitial = t_semid == -1;
    // 消费者调用此函数时，create=0，不能由消费者创建信号量
    if (!create && isInitial)
    {
        printf("消费者获取 KEY=%d 的信号量失败，需要通过生产者创建！\n", key);
        exit(1);
    }
    // 尝试创建信号量
    if ((t_semid = semget(key, num_semaphores, 0777 | IPC_CREAT)) == -1)
    {
        printf("Semaphore Array Creat Failed!\n");
        exit(1);
    }
    // 如果是首次创建信号量，则进行初始化
    if (isInitial)
    {
        printf("初次创建 KEY=%d 信号量数组，信号量初始化……\n", key);
        //  a. 将第0个信号量， 初值设为1 （用于缓冲区互斥访问）
        initialize_semaphores(t_semid, 0, 1);
        //  b. 将第1个信号量， 初值设为 BUFFER_CELL_NUM （EMPTY缓冲区个数）
        initialize_semaphores(t_semid, 1, BUFFER_CELL_NUM);
        //  c. 将第2个信号量， 初值设为0 （FULL 缓冲区个数）
        initialize_semaphores(t_semid, 2, 0);
    }
    else
        printf("已存在 KEY=%d 的信号量，直接获取……\n", key);

    return t_semid;
}

// 操作3，删除信号量和共享内存；
void delete_sem_and_shm()
{
    // 删除信号量集
    if (semctl(semid, 3, IPC_RMID) == -1)
    {
        perror("semctl IPC_RMID");
        exit(EXIT_FAILURE);
    }

    // 删除共享内存
    if (shmctl(shmid, IPC_RMID, 0) == -1)
    {
        perror("shmctl IPC_RMID");
        exit(EXIT_FAILURE);
    }
}