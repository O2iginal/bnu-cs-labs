#include <iostream>
#include <string>
#include <algorithm>
#include <list>
#include <iomanip>
using namespace std;


/*
编写C程序，模拟实现动态分区存储管理方案。要求实现首次/最佳适应算法的内存块分配和回收，假设初始状态下，可用内存空间是一块连续区域，大小为102400B。要求实现的功能有：
（1）分配内存
（2）回收内存
（3）显示内存（显示空闲分区和已分配分区的情况）
（4）退出
使用list维护空闲分区链表，使用list维护已分配分区链表。
*/

// 固定大小内存分区结构
struct Block
{
    string name;   // 进程名
    int startAddr; // 起始地址
    int size;      // 大小
    Block(string name, int startAddr, int size) : name(name), startAddr(startAddr), size(size) {}
};

const int MAX_SIZE = 102400;

list<Block> freeList;  // 空闲分区链表
list<Block> allocList; // 已分配分区链表

// 初始化，将可用内存空间作为一z整块空闲分区
void Init()
{
    freeList.clear();
    allocList.clear();
    freeList.push_back(Block("1", 0, MAX_SIZE));
}

// 首次适应算法分配内存（空闲分区链表按照起始地址排序）
// 输入：申请空间名称和大小
// 输出：分配是否成功
bool FirstFitAlloc(string name, int size)
{
    // 遍历空闲分区链表，找到第一个满足要求的分区
    for (auto it = freeList.begin(); it != freeList.end(); it++)
    {
        if (it->size >= size)
        {
            // 创建分配分区
            Block allocBlock(name, it->startAddr, size);

            // 插入已分配分区链表，按照起始地址排序
            auto it2 = allocList.begin();
            while (it2 != allocList.end() && it2->startAddr < allocBlock.startAddr)
                it2++;
            allocList.insert(it2, allocBlock);

            // 更新空闲分区链表
            if (it->size == size)
            {
                // 分区大小相等，删除当前空闲分区
                freeList.erase(it);
            }
            else
            {
                // 分区大小不等，更新空闲分区，起始地址后移，大小减小
                it->startAddr += size;
                it->size -= size;
            }
            return true;
        }
    }
    
    return false;
}

// 最佳适应算法分配内存（空闲分区链表按照大小排序）
// 输入：申请空间名称和大小
// 输出：分配是否成功
bool BestFitAlloc(string name, int size)
{
    // 遍历空闲分区链表，找到最小的满足要求的分区
    // 由于空闲分区链表按照大小排序，因此只需找到首个满足大小的分区即可
    auto it = freeList.begin();
    while (it != freeList.end() && it->size < size)
        it++;

    // 如果找到最佳分区
    if (it != freeList.end())
    {
        // 创建分配分区
        Block allocBlock(name, it->startAddr, size);

        // 插入已分配分区链表，按照起始地址排序
        // 即插入到第一个大小大于等于当前分区的分区之前
        auto it2 = allocList.begin();
        while (it2 != allocList.end() && it2->startAddr < allocBlock.startAddr)
            it2++;
        allocList.insert(it2, allocBlock); // 注意：insert插入it位置之前

        // 更新空闲分区链表
        if (it->size == size)
        {
            // 分区大小相等，删除当前空闲分区
            freeList.erase(it);
        }
        else
        {
            // 分区大小不等，更新空闲分区，起始地址后移，大小减小
            it->startAddr += size;
            it->size -= size;
        }
        return true;
    }

    return false;
}

// 读入申请空间名称和大小，根据传入的分配函数指针分配内存
// （分配函数指针可为FirstFitAlloc或BestFitAlloc）
void Alloc(bool (*AllocFunc)(string, int))
{
    string name;
    int size;
    cout << "请输入申请空间名称 > ";
    cin >> name;
    cout << "请输入申请空间大小 > ";
    cin >> size;

    if (AllocFunc(name, size))
        cout << "分配成功!" << endl;
    else
        cout << "分配失败!" << endl;
}

// 首次适应算法回收内存
// 输入：回收空间名称
// 输出：与回收分区合并的空闲分区数, -1表示回收失败(未找到)
int FirstFitFree(string name)
{
    // 遍历已分配分区链表，找到要回收的分区
    for (auto it = allocList.begin(); it != allocList.end(); it++)
    {
        if (it->name == name)
        {
            // 创建空闲分区
            Block freeBlock(name, it->startAddr, it->size);

            // 插入空闲分区链表，按照起始地址排序
            auto it2 = freeList.begin();
            while (it2 != freeList.end() && it2->startAddr < freeBlock.startAddr)
                it2++;

            freeList.insert(it2, freeBlock);
            it2--; // 将指针移动到新插入分区位置

            // 更新已分配分区链表
            allocList.erase(it);

            // 合并空闲分区
            // 仅可能与前后两个分区合并,
            // 检查是否存在且地址是否连续
            int mergeCount = 0;
            auto itBefore = it2;
            itBefore--; // 前一个分区
            if (it2 != freeList.begin() && 
                itBefore->startAddr + itBefore->size == it2->startAddr)
            {
                // 与前一个分区合并
                // 保留当前分区, 删除前一个分区
                it2->size += itBefore->size;
                it2->startAddr = itBefore->startAddr;
                freeList.erase(itBefore);
                mergeCount++;
            }

            auto itAfter = it2;
            itAfter++; // 后一个分区
            if (itAfter != freeList.end() && 
                it2->startAddr + it2->size == itAfter->startAddr)
            {
                // 与后一个分区合并
                // 保留当前分区, 删除后一个分区
                it2->size += itAfter->size;
                freeList.erase(itAfter);
                mergeCount++;
            }
            return mergeCount;
        }
    }
    return -1; // 未找到name对应的分区
}

// 最佳适应算法回收内存
// 回收分区按照分区大小排序，合并后仍按照分区大小排序
// 输入：回收空间名称
// 输出：与回收分区合并的空闲分区数, -1表示回收失败(未找到)
int BestFitFree(string name)
{
    // 遍历已分配分区链表，找到要回收的分区
    for (auto it = allocList.begin(); it != allocList.end(); it++)
    {
        if (it->name == name)
        {
            
            // 创建空闲分区
            Block freeBlock(name, it->startAddr, it->size);

            // 删除已分配分区
            allocList.erase(it);

            int mergeCount = 0;
            // 首先检查是否可与前后两个分区合并
            for (auto it2 = freeList.begin(); it2 != freeList.end(); it2++)
            {
                // 如果it2是freeBlock的前一个分区
                if (it2->startAddr + it2->size == freeBlock.startAddr)
                {
                    // 将it2合并到freeBlock, 删除it2
                    freeBlock.startAddr = it2->startAddr;
                    freeBlock.size += it2->size;
                    freeList.erase(it2);
                    mergeCount++;
                }

                // 如果it2是freeBlock的后一个分区
                if (it2->startAddr == freeBlock.startAddr + freeBlock.size)
                {
                    // 将it2合并到freeBlock, 删除it2
                    freeBlock.size += it2->size;
                    freeList.erase(it2);
                    mergeCount++;
                }

                // 如果已经合并了两个分区，退出循环
                if (mergeCount == 2)
                    break;
            }

            // 插入空闲分区链表，保持大小升序排序
            auto it2 = freeList.begin();
            while (it2 != freeList.end() && it2->size < freeBlock.size)
                it2++;
            freeList.insert(it2, freeBlock);

            return mergeCount;
        }
    }
    return -1; // 未找到name对应的分区
}

// 读入回收空间名称，根据传入的回收函数指针回收内存
// （回收函数指针可为FirstFitFree或BestFitFree）
void Free(int (*FreeFunc)(string))
{
    string name;
    cout << "请输入回收空间名称 > ";
    cin >> name;

    int mergeCount = FreeFunc(name);
    if (mergeCount == -1)
    {
        cout << "回收失败!" << endl;
    }
    else
    {
        cout << "回收成功! 合并空闲分区数： " << mergeCount << endl;
    }
}

// 显示空闲分区和已分配分区的情况
void Show()
{
    // 打印空闲分区
    cout << "\n空闲分区: " << endl;
    cout << "编号  起始地址  大小(B)\n";
    cout.setf(ios::right);

    int idx = 0;
    for (auto it = freeList.begin(); it != freeList.end(); it++)
    {
        cout << setw(3) << ++idx
             << setw(9) << it->startAddr
             << setw(10) << it->size
             << endl;
    }
    cout << endl;

    // 打印已分配分区
    cout << "已分配分区: " << endl;
    cout << "名称  起始地址  大小(B)\n";
    for (auto it = allocList.begin(); it != allocList.end(); it++)
    {
        cout << setw(3) << it->name
             << setw(9) << it->startAddr
             << setw(10) << it->size
             << endl;
    }
}

int main()
{
    Init();

    string op; // string 避免异常输入
    do
    {
        cout << "===================== 动态分区模拟实验 ===================\n";
        cout << "选择分配算法(1.首次适应算法 2.最佳适应算法),\n";
        cout << "输入算法编号 > ";
        cin >> op;
    } while (op != "1" && op != "2");

    // 根据用户选择的分配算法，设置分配函数指针
    auto AllocFunc = (op == "1") ? FirstFitAlloc : BestFitAlloc;
    auto FreeFunc = (op == "1") ? FirstFitFree : BestFitFree;

    while (true)
    {
        cout << endl;
        cout << "======================================================\n";
        cout << "选择操作(1.分配内存 2.回收内存 3.显示内存 4.退出),\n";
        cout << "请输入操作编号 > "; cin >> op;

        if (op == "1")
            Alloc(AllocFunc);
        else if (op == "2")
            Free(FreeFunc);
        else if (op == "3")
            Show();
        else if (op == "4")
            break;
        else
            cout << "输入错误，请重新输入!" << endl;
    }

    return 0;
}