#include <iostream>
#include <vector>
using namespace std;

vector<string> resource_name;   // 资源名称
vector<int> Available;          // 可用资源个数
vector<vector<int>> Max;        // 最大需求矩阵
vector<vector<int>> Allocation; // 已分配矩阵
vector<vector<int>> Need;       // 需求矩阵
int resource_num;   // 资源种类数
int task_num;       // 进程数

// 重载vector输出
template <typename T>
std::ostream &operator<<(std::ostream &o, const std::vector<T> &vec) {
    int num = vec.size();
    for (int i = 0; i < num; i++)
        cout << (i ? " " : "") << vec[i];
    return o;
}

// 重载vector减法
vector<int> operator-(vector<int> &a, vector<int> &b) {
    if (a.size() != b.size()) {
        cout << "vector运算的两个操作数大小不同！\n";
        exit(-1);
    }
    int n = a.size();
    vector<int> res(n);
    for (int i = 0; i < n; i ++) {
        res[i] = a[i] - b[i];
    }
    return res;
}

// 重载vector加法
vector<int> operator+(vector<int> &a, vector<int> &b) {
    if (a.size() != b.size()) {
        cout << "vector运算的两个操作数大小不同！\n";
        exit(-1);
    }
    int n = a.size();
    vector<int> res(n);
    for (int i = 0; i < n; i ++) {
        res[i] = a[i] + b[i];
    }
    return res;
}

// 重载vector比较<=, 用于判断是否资源数小于等于关系
bool operator<=(vector<int> const &a, vector<int> const &b) {
    if (a.size() != b.size()) {
        cout << "vector运算的两个操作数大小不同！\n";
        exit(-1);
    }
    int n = a.size();
    for (int i = 0; i < n; i ++) {
        if (a[i] > b[i]) return false;
    }
    return true;
}

// 计算Need矩阵
void GetNeed() {
    for (int i = 0; i < task_num; i ++)
        for (int j = 0; j < resource_num; j ++)
            Need[i][j] = Max[i][j] - Allocation[i][j];
}

// 获取用户输入的选项
int GetOption() {
    cout << "**********************银行家算法演示**************" << '\n';
    cout << "*               1:显示当前资源分配情况" << '\n';
    cout << "*               2:检查当前状态安全性" << '\n';
    cout << "*               3:分配资源" << '\n';
    cout << "*               4:创建进程" << '\n';
    cout << "*               5:删除进程" << '\n';
    cout << "*               6:离开" << '\n';
    cout << "*************************************************" << '\n';
    cout << "请选择功能号：";
    int op;
    cin >> op;
    return op;
}

// 操作1，显示当前资源分配情况
void ListResouceStatus() {
    cout << "系统目前可用的资源[Avaliable]: " << '\n';
    cout << resource_name << '\n';
    cout << Available << '\n';

    cout << "             Max      Allocation     Need" << '\n';
    cout << "进程名     ";
    for (int i = 0; i < 3; i++)
        cout << resource_name << "       ";
    cout << '\n';

    for (int i = 0; i < task_num; i++) {
        cout << " " << i << "         ";
        cout << Max[i] << "       ";
        cout << Allocation[i] << "       ";
        cout << Need[i] << '\n';
    }
}

// 操作2，检查当前状态安全性，若安全则输出安全序列；否则输出不安全
// 返回值：是否安全
bool CheckStatusSecurity() {
    vector<int> Work = Available;
    vector<bool> Finish(task_num, false);
    vector<int> SafeSequence;
    for (int count = 0; count < task_num; count ++) {
        bool flag = false;
        for (int i = 0; i < task_num; i++) {
            if (Finish[i] == false && Need[i] <= Work) {
                Work = Work + Allocation[i];
                Finish[i] = true;
                SafeSequence.push_back(i);
                flag = true;
                break;
            }
        }
        if (flag == false) {
            cout << "系统处于不安全状态，无法分配！" << '\n';
            return false;
        }
    }
    cout << "系统是安全的!" << '\n';
    cout << "安全序列为：";
    for (int i = 0; i < task_num; i++)
        cout << SafeSequence[i] << (i == task_num - 1 ? "" : "->");
    cout << '\n';
    return true;
}

// 操作3，分配资源
// 先判断是否安全，若安全则分配，否则不分配（回滚）
void AllocateResource() {
    int task_id;
    cout << "请输入要求分配的资源进程号0-" << task_num - 1 << ":";
    cin >> task_id;
    vector<int> Request(resource_num);
    cout << "请输入进程 " << task_id << " 申请的资源:\n";
    for (int i = 0; i < resource_num; i++)
    {
        cout << resource_name[i] << ":";
        cin >> Request[i];
    }
    if (Request > Need[task_id]) {
        cout << "进程" << task_id << "申请的资源大于其需要量 分配出错，不予分配!"
             << '\n';
        return;
    }
    if (Request > Available) {
        cout << "进程" << task_id << "申请的资源大于系统现在可利用的资源 分配出错，不予分配!"
             << '\n';
        return;
    }
    Available = Available - Request;
    Allocation[task_id] = Allocation[task_id] + Request;
    Need[task_id] = Need[task_id] - Request;

    cout << '\n';
    ListResouceStatus();
    if (CheckStatusSecurity() == false)
    {
        // 回滚
        Available = Available + Request;
        Allocation[task_id] = Allocation[task_id] - Request;
        Need[task_id] = Need[task_id] + Request;
        
        cout << "系统目前可用的资源[Avaliable]仍然为:" << '\n';
        cout << resource_name << '\n';
        cout << Available << '\n';
    }
}

// 操作4，创建进程
void CreateProcess() {
    // 获取新进程信息
    vector<int> process_max(resource_num);
    cout << "请输入进程最大需求量:\n";
    for (int i = 0; i < resource_num; i++) {
        cout << resource_name[i] << ":";
        cin >> process_max[i];
    }

    vector<int> process_allocation(resource_num);
    cout << "请输入进程已经申请的资源量:\n";
    for (int i = 0; i < resource_num; i++) {
        cout << resource_name[i] << ":";
        cin >> process_allocation[i];
    }

    vector<int> process_need = process_max - process_allocation;

    // 更新各个矩阵
    Max.push_back(process_max);
    Allocation.push_back(process_allocation);
    Need.push_back(process_need);

    // 更新进程数
    task_num++;

    // 显示当前进程及资源分配情况
    cout << "进程创建成功!" << '\n';
    cout << "当前进程及资源分配情况:" << '\n';
    ListResouceStatus();
}

// 操作5，删除进程
void DeleteProcess() {
    // 获取要删除的进程号
    int process_id;
    cout << "请输入要删除的进程号0-" << task_num - 1 << ":";
    cin >> process_id;

    // 更新各个矩阵
    Available = Available + Allocation[process_id];
    Max.erase(Max.begin() + process_id);
    Allocation.erase(Allocation.begin() + process_id);
    Need.erase(Need.begin() + process_id);

    // 更新进程数
    task_num--;

    // 显示当前进程及资源分配情况
    cout << "进程删除成功!" << '\n';
    cout << "当前进程及资源分配情况:" << '\n';
    ListResouceStatus();
}

// 初始化, 获取用户输入, 初始化各个变量(资源数, 进程数, 各个矩阵)
void Init() {
    string name;
    int num;
    cout << "*****************单处理机系统进程调度实现*****************"<< '\n';

    cout << "请首先输入系统可供资源种类的数量:";
    cin >> resource_num;
    resource_name.resize(resource_num);
    Available.resize(resource_num);
    for (int i = 0; i < resource_num; i++) {
        cout << "资源" << i+1 << "的名称:";
        cin >> resource_name[i];
        cout << "资源的数量:";
        cin >> Available[i];
    }
    cout << '\n';

    cout << "请输入作业的数量:";
    cin >> task_num;
    Max = Allocation = Need =
        vector<vector<int>>(task_num, vector<int>(resource_num, 0));

    printf("请输入各进程的最大需求量(%d*%d矩阵)[Max]:\n", task_num, resource_num);
    for (int i = 0; i < task_num; i++)
        for (int j = 0; j < resource_num; j++)
            cin >> Max[i][j];

    printf("请输入各进程已经申请的资源量(%d*%d矩阵)[Allocation]:\n", task_num, resource_num);
    for (int i = 0; i < task_num; i++)
        for (int j = 0; j < resource_num; j++)
            cin >> Allocation[i][j];

    GetNeed();  // 由Max和Allocation计算Need
    
    cout << '\n';
    ListResouceStatus();  // 显示当前资源分配情况
    CheckStatusSecurity();  // 检查当前状态安全性
    cout << "\n\n";
}


int main() { 
    Init();
    int op;
    while (op = GetOption()) {
        cout << '\n';
        switch (op) {
            case 1:
                ListResouceStatus();  // 显示当前资源分配情况
                break;
            case 2:
                CheckStatusSecurity();  // 检查当前状态安全性
                break;
            case 3:
                AllocateResource();  // 分配资源
                break;
            case 4:
                CreateProcess();  // 创建进程
                break;
            case 5:
                DeleteProcess();  // 删除进程
                break;
            case 6:
                return 0;      // 退出
            default:
                cout << "输入错误，请重新输入！" << '\n';
        }
        cout << '\n' << '\n';
    }
    return 0; 
}
