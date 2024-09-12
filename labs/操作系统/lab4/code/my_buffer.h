#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <cstring>

#define BUFFER_CELL_SIZE 1024
#define BUFFER_CELL_NUM 5

// 一个缓存块
struct BufferCell
{
    char text[BUFFER_CELL_SIZE];
};

// 缓存区，包含若干缓存块
// 从数据结构角度，实际即循环队列
struct Buffer
{
    int in, out, n;  // 队列尾，队列头，队列空间大小
    BufferCell data[BUFFER_CELL_NUM];

    // 将tmp指向的文本装入Buffer队尾
    void push_back(char *tmp)
    {
        // 1. 装入数据
        strncpy(data[in].text, tmp, (BUFFER_CELL_SIZE-1) * sizeof(char));
        data[in].text[BUFFER_CELL_SIZE - 1] = '\0';
        // 2. 更新指针
        in++;
        in %= BUFFER_CELL_NUM;
        n++;
    }

    // 将Buffer队头的元素移出，并将其内容拷贝到tmp
    void pop_front(char *tmp)
    {
        // 1. 拷贝出数据到data
        strncpy(tmp, data[out].text, (BUFFER_CELL_SIZE-1) * sizeof(char));
        tmp[BUFFER_CELL_SIZE - 1] = '\0';
        // 2. 更新指针
        out++;
        out %= BUFFER_CELL_NUM;
        n--;
    }

    // 打印当前Buffer状态（尾指针位置in，头指针位置out，n个位置的空满状态）
    void print()
    {
        // 1. 标记出buffer的BUFFER_CELL_NUM缓存块中，空缓冲区标记为1，满为0
        int *tmp = (int *)calloc(BUFFER_CELL_NUM, sizeof(int));
        for (int i = out, j = 1; j <= n; i = (i+1) % BUFFER_CELL_NUM, j++)
            tmp[i] = 1;

        // 2. 打印输出(两指针位置， 缓冲块存储状态 ‘#’为满 ‘-’为空)
        printf("缓冲区状态：[buffer.in] = %d,  [buffer.out] = %d,  ",in,out);
        printf("[data] = [ ");
        for (int i = 0; i < BUFFER_CELL_NUM; i++)
            printf("%s", (tmp[i] ? "#" : "-"));

        printf(" ]\n");
        free(tmp);
    }
};