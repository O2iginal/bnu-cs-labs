#include <iostream>
#include <unistd.h>
#include <sys/sem.h>
// 初始化信号量集semid中第index个信号量，赋值为initial_value
void initialize_semaphores(int semid,int index,int initial_value)
{
    // 使用 semctl 进行初始化
    if (semctl(semid, index, SETVAL, initial_value) == -1)
    {
        perror("semctl");
        exit(EXIT_FAILURE);
    }
}

// 执行 P 操作
// 对semid的信号量数组的第semaphore_num信号量操作
void semaphore_P(int semid, int semaphore_num)
{
    struct sembuf operation;
    operation.sem_num = semaphore_num;
    operation.sem_op = -1; // P 操作
    operation.sem_flg = 0;
    if (semop(semid, &operation, 1) == -1)
    {
        perror("semop P");
        exit(EXIT_FAILURE);
    }
}

// 执行 V 操作
// 对semid的信号量数组的第semaphore_num信号量操作
void semaphore_V(int semid, int semaphore_num)
{
    struct sembuf operation;
    operation.sem_num = semaphore_num;
    operation.sem_op = 1; // V 操作
    operation.sem_flg = 0;
    if (semop(semid, &operation, 1) == -1)
    {
        perror("semop V");
        exit(EXIT_FAILURE);
    }
}