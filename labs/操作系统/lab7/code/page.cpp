#include <iostream>
#include <cstring>
#include <vector>
#include <algorithm>
#include <queue>
using namespace std;

// 页表项
struct Page
{
    int logicNumber;    // 页号
    int physicalNumber; // 物理块号
    int diskNumber;     // 在磁盘上的位置
    bool write;         // 修改位标志
    bool flag;          // 存在位标志

    // 初始化页表项
    Page(int logicNumber, int physicalNumber, int diskNumber, bool write, bool flag)
    {
        this->logicNumber = logicNumber;
        this->physicalNumber = physicalNumber;
        this->diskNumber = diskNumber;
        this->write = write;
        this->flag = flag;
    }
};

struct PageTable
{
    vector<Page> pages;         // 页表，存储页表项
    int pageSize;               // 页大小, 用于计算页号与页内偏移
    int blockNumberMax;         // 主存块最大数
    queue<int> removePageQueue; // 淘汰队列，FIFO
    queue<int> emptyBlockQueue; // 空闲队列

    // 初始化页表
    PageTable(int pageSize)
    {
        this->pageSize = pageSize;
    }

    // 初始化页表, 读入页表若干表项的页号、磁盘号
    void InitPages()
    {
        int lNumber, dNumber, idx = 0; // 页号, 磁盘号, 输入序号
        cout << "创建页表, 输入页表的信息(若页号为-1, 则结束输入)\n";
        cout << "输入页号和磁盘地址\nIn[" << idx++ << "] > ";
        cin >> lNumber >> dNumber;

        while (lNumber != -1)
        {
            pages.push_back(Page(lNumber, -1, dNumber, false, false));
            cout << "输入页号和磁盘地址\nIn[" << idx++ << "] > ";
            cin >> lNumber >> dNumber;
        }

        cout << "页表初始化完成!\n\n";
    }

    // 初始化主存块, 读入可用主存块号, 存入空闲队列
    void InitBlock()
    {
        int blockNumber, idx = 0;
        cout << "输入主存块号, 主存块数要小于[" << pages.size() << "], (以-1结束)\n";
        cout << "In[" << idx++ << "] > ";
        cin >> blockNumber;

        while (blockNumber != -1)
        {
            emptyBlockQueue.push(blockNumber);
            cout << "In[" << idx++ << "] > ";
            cin >> blockNumber;
        }

        // 主存块数最大值为页表项数
        blockNumberMax = emptyBlockQueue.size();
        if (blockNumberMax > pages.size())
        {
            cout << "Error: 主存块数大于页表项数, 程序退出!\n";
            exit(0);
        }

        cout << "主存块初始化完成!\n\n";
    }

    // 预调页, 将可用主存块按顺序分配给页表项
    void PreschedulePage()
    {
        int blockNumber;
        cout << "预调页中, 将可用主存块按顺序分配给页表项...\n";
        int tableIndex = 0;
        while (!emptyBlockQueue.empty())
        {
            blockNumber = emptyBlockQueue.front();
            emptyBlockQueue.pop();
            pages[tableIndex].physicalNumber = blockNumber;
            pages[tableIndex].flag = true;
            removePageQueue.push(tableIndex);
            cout << "-> 页[" << pages[tableIndex].logicNumber
                 << "]调入主存块[" << pages[tableIndex].physicalNumber << "]\n";
            tableIndex++;
        }
        cout << "预调页完成!\n\n";
    }

    // 打印页表
    void PrintPageTable()
    {
        cout << "====== 页表 ====== >\n";
        cout << "页号\t物理块号\t磁盘块号\t修改位\t存在位\n";
        for (int i = 0; i < pages.size(); i++)
        {
            cout << pages[i].logicNumber << "\t"
                 << pages[i].physicalNumber << "\t\t"
                 << pages[i].diskNumber << "\t\t"
                 << pages[i].write << "\t"
                 << pages[i].flag << "\n";
        }
    }

    // 打印淘汰队列
    void PrintRemoveQueue()
    {
        cout << "内存块淘汰队列（页号）: ";
        queue<int> temp = removePageQueue;
        while (!temp.empty())
        {
            cout << pages[temp.front()].logicNumber;
            temp.pop();
            if (!temp.empty())
                cout << " <- ";
        }
        cout << "\n";
    }

    // 淘汰一页(先进先出)
    void RemovePage()
    {
        if (removePageQueue.empty())
        {
            cout << "Error: 淘汰队列为空, 程序退出!\n";
            exit(0);
        }
        // 从淘汰队列中取出页, FIFO
        Page &page = pages[removePageQueue.front()];
        removePageQueue.pop();
        // 打印淘汰信息
        cout << "\n-> 淘汰主存块[" << page.physicalNumber
             << "]中的页[" << page.logicNumber << "]"
             << "...\n";
        if (page.write)
        {
            // 若该页被修改, 将其写回磁盘
            cout << "-> 页[" << page.logicNumber << "]被修改, 将其写回磁盘。\n";
            page.write = false;
        }
        else
        {
            // 若该页未被修改, 不需要写回磁盘
            cout << "-> 页[" << page.logicNumber << "]未被修改, 不需要写回磁盘。\n";
        }
        cout << "\n";

        // 将该主存块加入空闲队列
        emptyBlockQueue.push(page.physicalNumber);
        // 从主存中删除该页
        page.flag = false;
        page.physicalNumber = -1;
    }

    // 访问页
    // logicAddress: 逻辑地址
    // write: 本次访问操作是否修改
    void SchedulePage(int logicAddress, bool write)
    {
        // 计算页号与页内偏移
        int logicNumber = logicAddress / pageSize;
        int offset = logicAddress % pageSize;
        cout << "-> 访问逻辑地址[" << logicAddress
             << "]... \n-> 页号：[" << logicNumber
             << "]，页内偏移：[" << offset << "]\n\n";
        // 查找页表中是否存在该页
        int tableIndex = -1;
        for (int i = 0; i < pages.size(); i++)
            if (pages[i].logicNumber == logicNumber)
            {
                tableIndex = i;
                break;
            }
        // 若页表中不存在该页
        if (tableIndex == -1)
        {
            cout << "Error: 页表中不存在页[" << logicNumber << "] !\n\n";
            return;
        }
        // 若页表中存在该页
        if (pages[tableIndex].flag == true)
        {
            cout << "-> 页[" << logicNumber
                 << "]已在主存块[" << pages[tableIndex].physicalNumber << "]中"
                 << "\n";
        }
        else
        {
            // 若主存块已满
            cout << "-> 页[" << logicNumber
                 << "]不在主存块中"
                 << "\n";
            // 若无空闲主存块, 淘汰一页
            if (emptyBlockQueue.empty())
            {
                cout << "-> 主存块已满 ...\n";
                RemovePage();
            }
            int blockNumber = emptyBlockQueue.front();
            emptyBlockQueue.pop();
            pages[tableIndex].physicalNumber = blockNumber;
            pages[tableIndex].flag = true;
            removePageQueue.push(tableIndex);
            cout << "-> 从磁盘第[" << pages[tableIndex].diskNumber
                 << "]块中调入页[" << logicNumber
                 << "]到主存块[" << pages[tableIndex].physicalNumber << "]"
                 << "\n";
        }
        // 计算物理地址
        int physicalAddress = pages[tableIndex].physicalNumber * pageSize + offset;
        cout << "-> 对应物理地址为[" << physicalAddress << "]"
             << "\n";
        // 若本次访问修改
        if (write)
        {
            pages[tableIndex].write = true;
            cout << "-> 本次访问页[" << logicNumber << "]被修改\n";
        }
    }
};

int main()
{
    // freopen("in.txt", "r", stdin);
    // freopen("out.txt", "w", stdout);
    cout << "******************* 请求分页模拟 ******************\n\n";

    // 页表初始化
    int pageSize = 1 << 10; // 页大小
    PageTable pageTable = PageTable(pageSize);

    // 读入页表信息
    pageTable.InitPages();

    // 读入主存块信息
    pageTable.InitBlock();

    // 预调页
    pageTable.PreschedulePage();

    // 打印页表
    pageTable.PrintPageTable();

    // 打印淘汰队列
    pageTable.PrintRemoveQueue();

    while (true)
    {
        int write, logicAddress;

        // 读入指令性质
        cout << "\n**************************************************\n";
        cout << "输入指令性质(1-修改, 0-不修改, 其他-结束程序运行)\n";
        cout << "In > ";
        cin >> write;
        if (write != 1 && write != 0)
            break;

        // 读入逻辑地址
        cout << "输入逻辑地址\n";
        cout << "In > ";
        cin >> logicAddress;
        if (logicAddress < 0)
        {
            cout << "Error: 逻辑地址不能为负数, 程序退出!\n";
            exit(0);
        }
        cout << "\n";

        // 访问页
        pageTable.SchedulePage(logicAddress, write);

        // 打印页表、淘汰队列
        cout << "\n";
        pageTable.PrintPageTable();
        pageTable.PrintRemoveQueue();
    }
    cout << "程序运行结束!\n";

    return 0;
}