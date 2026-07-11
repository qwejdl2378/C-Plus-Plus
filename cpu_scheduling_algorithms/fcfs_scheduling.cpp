/**
 * @file
 * @brief Implementation of FCFS CPU scheduling algorithm (先来先服务 CPU 调度算法实现)
 * @details
 * 先来先服务（FCFS, First Come First Serve）是一种非抢占式的 CPU 调度算法。
 * 哪个进程先到达（Arrival Time 较小），哪个进程就先获得 CPU 执行。
 * 如果两个或多个进程同时到达，则进程 ID（Process ID）较小的进程优先执行。
 * 
 * @link https://bit.ly/3ABNXOC
 * @author [Pratyush Vatsa](https://github.com/Pratyush219)
 */

#include <algorithm>      /// 用于 std::sort
#include <cassert>        /// 用于 assert 断言
#include <cstdint>
#include <cstdlib>        /// 用于随机数生成
#include <ctime>          /// 用于 time 函数
#include <iomanip>        /// 用于输出对齐格式控制
#include <iostream>       /// 用于标准输入输出
#include <queue>          /// 用于 std::priority_queue 优先队列
#include <unordered_set>  /// 用于 std::unordered_set 去重
#include <vector>         /// 用于 std::vector 容器

using std::cin;
using std::cout;
using std::endl;
using std::get;
using std::left;
using std::make_tuple;
using std::priority_queue;
using std::rand;
using std::srand;
using std::tuple;
using std::unordered_set;
using std::vector;

/**
 * @brief 用于向量排序的比较函数
 * @details 优先根据到达时间（Arrival Time）进行升序排序；若到达时间相同，则根据进程 ID 进行升序排序。
 * @tparam S 进程 ID 的数据类型
 * @tparam T 到达时间的数据类型
 * @tparam E 执行时间（Burst Time）的数据类型
 * @param t1 第一个元组
 * @param t2 第二个元组
 * @returns `true` 代表顺序正确；`false` 否则
 */
template <typename S, typename T, typename E>
bool sortcol(tuple<S, T, E>& t1, tuple<S, T, E>& t2) {
    if (get<1>(t1) < get<1>(t2)) {
        return true;
    } else if (get<1>(t1) == get<1>(t2) && get<0>(t1) < get<0>(t2)) {
        return true;
    }
    return false;
}

/**
 * @class Compare
 * @brief 优先队列（最小堆）的自定义比较类
 * @tparam S 进程 ID 数据类型
 * @tparam T 到达时间数据 type
 * @tparam E 执行时间数据类型
 */
template <typename S, typename T, typename E>
class Compare {
 public:
    /**
     * @brief 判定是否需要交换两个进程元组（以实现最小堆）
     * @param t1 第一个进程元组
     * @param t2 第二个进程元组
     * @returns `true` 表示需要调整顺序（t1 应该排在后面）；`false` 否则
     */
    bool operator()(tuple<S, T, E, double, double, double>& t1,
                    tuple<S, T, E, double, double, double>& t2) {
        // 比较到达时间，时间较早的排在堆顶
        if (get<1>(t2) < get<1>(t1)) {
            return true;
        }
        // 如果到达时间相同，则进程 ID 较小的排在堆顶
        else if (get<1>(t2) == get<1>(t1)) {
            return get<0>(t2) < get<0>(t1);
        }
        return false;
    }
};

/**
 * @class FCFS
 * @brief 先来先服务 CPU 调度控制类
 */
template <typename S, typename T, typename E>
class FCFS {
    /**
     * 存储各个进程调度状态元组的最小优先队列。
     * 元组各位置含义：
     * 0: 进程 ID
     * 1: 到达时间 (Arrival Time)
     * 2: 执行时间 (Burst Time)
     * 3: 完成时间 (Completion Time)
     * 4: 周转时间 (Turnaround Time)
     * 5: 等待时间 (Waiting Time)
     */
    priority_queue<tuple<S, T, E, double, double, double>,
                   vector<tuple<S, T, E, double, double, double>>,
                   Compare<S, T, E>>
        schedule;

    // 存储完成调度后的最终结果集
    vector<tuple<S, T, E, double, double, double>> result;

    // 用于快速去重检查的进程 ID 集合
    unordered_set<S> idList;

 public:
    /**
     * @brief 添加一个进程到就绪状态中
     * @param id 进程 ID
     * @param arrival 到达时间
     * @param burst 执行时间
     */
    void addProcess(S id, T arrival, E burst) {
        // 防止重复添加相同 ID 的进程
        if (idList.find(id) == idList.end()) {
            tuple<S, T, E, double, double, double> t =
                make_tuple(id, arrival, burst, 0, 0, 0);
            schedule.push(t);
            idList.insert(id);
        }
    }

    /**
     * @brief 执行先来先服务调度算法计算
     * @details
     * 按到达时间（以及 ID）顺序模拟执行。
     * 1. 周转时间 (Turnaround Time) = 完成时间 (Completion Time) - 到达时间 (Arrival Time)
     * 2. 等待时间 (Waiting Time) = 周转时间 (Turnaround Time) - 执行时间 (Burst Time)
     * @returns 排序和计算完成后的结果向量
     */
    vector<tuple<S, T, E, double, double, double>> scheduleForFcfs() {
        double timeElapsed = 0; // 模拟的时间轴

        while (!schedule.empty()) {
            tuple<S, T, E, double, double, double> cur = schedule.top();

            // 如果当前进程的到达时间大于当前时间轴，说明 CPU 出现空闲，直接跳跃到到达时刻
            if (get<1>(cur) > timeElapsed) {
                timeElapsed += get<1>(cur) - timeElapsed;
            }

            timeElapsed += get<2>(cur); // 累加该进程的执行时间

            get<3>(cur) = timeElapsed;  // 记录完成时间
            get<4>(cur) = get<3>(cur) - get<1>(cur); // 计算周转时间
            get<5>(cur) = get<4>(cur) - get<2>(cur); // 计算等待时间

            result.push_back(cur);
            schedule.pop();
        }
        return result;
    }

    /**
     * @brief 打印各个进程计算完毕后的状态报表
     */
    void printResult() {
        cout << "Status of all the processes post completion is as follows:" << endl;

        cout << std::setw(17) << left << "Process ID" << std::setw(17) << left
             << "Arrival Time" << std::setw(17) << left << "Burst Time"
             << std::setw(17) << left << "Completion Time" << std::setw(17)
             << left << "Turnaround Time" << std::setw(17) << left
             << "Waiting Time" << endl;

        for (size_t i{}; i < result.size(); i++) {
            cout << std::setprecision(2) << std::fixed << std::setw(17) << left
                 << get<0>(result[i]) << std::setw(17) << left
                 << get<1>(result[i]) << std::setw(17) << left
                 << get<2>(result[i]) << std::setw(17) << left
                 << get<3>(result[i]) << std::setw(17) << left
                 << get<4>(result[i]) << std::setw(17) << left
                 << get<5>(result[i]) << endl;
        }
    }
};

/**
 * @brief 辅助测试验证函数。通过对输入排序来保证 FCFS 计算结果的准确性
 * @param input 输入进程的原始元组集合
 * @returns 最终的状态元组列表
 */
template <typename S, typename T, typename E>
vector<tuple<S, T, E, double, double, double>> get_final_status(
    vector<tuple<uint32_t, uint32_t, uint32_t>> input) {
    sort(input.begin(), input.end(), sortcol<S, T, E>);
    vector<tuple<S, T, E, double, double, double>> result(input.size());
    double timeElapsed = 0;
    for (size_t i{}; i < input.size(); i++) {
        T arrival = get<1>(input[i]);
        E burst = get<2>(input[i]);

        if (arrival > timeElapsed) {
            timeElapsed += arrival - timeElapsed;
        }
        timeElapsed += burst;
        double completion = timeElapsed;
        double turnaround = completion - arrival;
        double waiting = turnaround - burst;

        get<0>(result[i]) = get<0>(input[i]);
        get<1>(result[i]) = arrival;
        get<2>(result[i]) = burst;
        get<3>(result[i]) = completion;
        get<4>(result[i]) = turnaround;
        get<5>(result[i]) = waiting;
    }
    return result;
}

/**
 * @brief 单元自测用例集
 */
static void test() {
    for (int i{}; i < 1000; i++) {
        srand(time(nullptr));
        uint32_t n = 1 + rand() % 1000;
        FCFS<uint32_t, uint32_t, uint32_t> readyQueue;
        vector<tuple<uint32_t, uint32_t, uint32_t>> input(n);

        for (uint32_t i{}; i < n; i++) {
            get<0>(input[i]) = i;
            srand(time(nullptr));
            get<1>(input[i]) = 1 + rand() % 10000;
            srand(time(nullptr));
            get<2>(input[i]) = 1 + rand() % 10000;
        }

        for (uint32_t i{}; i < n; i++) {
            readyQueue.addProcess(get<0>(input[i]), get<1>(input[i]),
                                  get<2>(input[i]));
        }
        vector<tuple<uint32_t, uint32_t, uint32_t, double, double, double>>
            res = get_final_status<uint32_t, uint32_t, uint32_t>(input);
        assert(res == readyQueue.scheduleForFcfs());
    }
    cout << "All the tests have successfully passed!" << endl;
}

/**
 * @brief 主函数
 * @returns 0
 */
int main() {
    test();  // 运行测试
    return 0;
}
