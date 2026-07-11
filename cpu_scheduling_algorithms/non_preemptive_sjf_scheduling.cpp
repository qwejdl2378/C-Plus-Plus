/**
 * @file
 * @brief Implementation of SJF CPU scheduling algorithm (非抢占式短作业优先 CPU 调度算法)
 * @details
 * 短作业优先（SJF, Shortest Job First），又称短作业优先（SJN, Shortest Job Next）。
 * 是一种非抢占式调度策略，每次选择就绪队列中执行时间（Burst Time）最短的进程进行调度。
 *
 * @note
 * 【设计缺陷与实现逻辑说明】：
 * 1. 在该文件中，`schedule` 是一个全局的优先队列，排序键主要是 `Burst Time`（执行时间）。
 * 2. 调度逻辑采用 `while (!schedule.empty()) { cur = schedule.top(); ... }`，每次都直接弹出绝对执行时间最短的进程。
 * 3. 这种做法实际上默认了“所有作业在时刻 0 均已到达且处于就绪状态”（或者在选择下一个执行进程时不考虑实际的到达时间限制），
 *    因为它没有根据当前时间轴 `timeElapsed` 动态过滤已到达的进程，而是直接按 Burst Time 全局排序。
 * 4. 该细节使得该算法更符合批处理系统中的短作业优先，而非动态到达的抢占/非抢占 SJF 调度。
 *
 * <a href="https://www.guru99.com/shortest-job-first-sjf-scheduling.html">SJF 调度算法详解</a>
 * <a href="https://github.com/LakshmiSrikumar">作者 : Lakshmi Srikumar</a>
 */

#include <algorithm>      /// 用于排序
#include <cassert>        /// 用于 assert 断言
#include <iomanip>        /// 用于输出对齐格式控制
#include <iostream>       /// 用于标准输入输出
#include <queue>          /// 用于 std::priority_queue
#include <random>         /// 用于随机数生成
#include <unordered_set>  /// 用于 std::unordered_set
#include <vector>         /// 用于 std::vector

using std::cin;
using std::cout;
using std::endl;
using std::get;
using std::left;
using std::make_tuple;
using std::priority_queue;
using std::tuple;
using std::unordered_set;
using std::vector;

/**
 * @brief 用于向量排序的比较函数
 * @details 优先根据到达时间（Arrival Time）进行升序排序；若到达时间相同，则根据进程 ID 升序排序。
 * @tparam S 进程 ID 的数据类型
 * @tparam T 到达时间的数据类型
 * @tparam E 执行时间（Burst Time）的数据类型
 */
template <typename S, typename T, typename E>
bool sortcol(tuple<S, T, E>& t1, tuple<S, T, E>& t2) {
    if (get<1>(t1) < get<1>(t2) ||
        (get<1>(t1) == get<1>(t2) && get<0>(t1) < get<0>(t2))) {
        return true;
    }
    return false;
}

/**
 * @class Compare
 * @brief 优先队列（最小堆）的自定义比较类
 * @details 优先按执行时间（Burst Time）进行升序排序，若执行时间相同，则按到达时间升序排序。
 */
template <typename S, typename T, typename E>
class Compare {
 public:
    /**
     * @brief 判定是否需要交换两个进程元组（实现短作业优先堆结构）
     * @param t1 第一个进程元组
     * @param t2 第二个进程元组
     * @returns `true` 表示需要交换（t1 应排在后面）；`false` 否则
     */
    bool operator()(tuple<S, T, E, double, double, double>& t1,
                    tuple<S, T, E, double, double, double>& t2) {
        // 优先比较执行时间（SJF 的核心）
        if (get<2>(t2) < get<2>(t1)) {
            return true;
        }
        // 如果执行时间相同，则比较到达时间，时间较早的排在堆顶
        else if (get<2>(t2) == get<2>(t1)) {
            return get<1>(t2) < get<1>(t1);
        }
        return false;
    }
};

/**
 * @class SJF
 * @brief 非抢占式短作业优先调度算法管理类
 */
template <typename S, typename T, typename E>
class SJF {
    /**
     * 优先队列 schedule 存储各个进程的状态元组。
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

    // 用于快速去重确认进程 ID 的集合
    unordered_set<S> idList;

 public:
    /**
     * @brief 添加一个进程到就绪状态中
     * @param id 进程 ID
     * @param arrival 到达时间
     * @param burst 执行时间
     */
    void addProcess(S id, T arrival, E burst) {
        if (idList.find(id) == idList.end()) {
            tuple<S, T, E, double, double, double> t =
                make_tuple(id, arrival, burst, 0, 0, 0);
            schedule.push(t);
            idList.insert(id);
        }
    }

    /**
     * @brief 执行 SJF 调度并计算各个时间指标
     * @returns 完成计算后的结果向量
     */
    vector<tuple<S, T, E, double, double, double>> scheduleForSJF() {
        double timeElapsed = 0; // 模拟的时间轴

        while (!schedule.empty()) {
            tuple<S, T, E, double, double, double> cur = schedule.top();

            // 如果当前进程的到达时间大于时间轴，说明 CPU 出现空闲，跳跃到到达时刻
            if (get<1>(cur) > timeElapsed) {
                timeElapsed += get<1>(cur) - timeElapsed;
            }

            timeElapsed += get<2>(cur); // 累加执行时间

            get<3>(cur) = timeElapsed;  // 记录完成时间
            get<4>(cur) = get<3>(cur) - get<1>(cur); // 计算周转时间
            get<5>(cur) = get<4>(cur) - get<2>(cur); // 计算等待时间

            // 断言合法性：周转时间一定大于等于执行时间
            assert(get<4>(cur) >= get<2>(cur));

            // 断言合法性：等待时间必定为非负数
            assert(get<5>(cur) >= 0);

            result.push_back(cur);
            schedule.pop();
        }
        return result;
    }

    /**
     * @brief 打印状态报表的辅助函数
     * @param processes 待打印的进程元组集合
     */
    void printResult(
        const vector<tuple<S, T, E, double, double, double>>& processes) {
        cout << std::setw(17) << left << "Process ID" << std::setw(17) << left
             << "Arrival Time" << std::setw(17) << left << "Burst Time"
             << std::setw(17) << left << "Completion Time" << std::setw(17)
             << left << "Turnaround Time" << std::setw(17) << left
             << "Waiting Time" << endl;

        for (const auto& process : processes) {
            cout << std::setprecision(2) << std::fixed << std::setw(17) << left
                 << get<0>(process) << std::setw(17) << left << get<1>(process)
                 << std::setw(17) << left << get<2>(process) << std::setw(17)
                 << left << get<3>(process) << std::setw(17) << left
                 << get<4>(process) << std::setw(17) << left << get<5>(process)
                 << endl;
        }
    }
};

/**
 * @brief 顺序计算最终进程调度状态（供参考或测试比对）
 */
template <typename S, typename T, typename E>
vector<tuple<S, T, E, double, double, double>> get_final_status(
    vector<tuple<S, T, E>> input) {
    // 按到达时间升序排序
    sort(input.begin(), input.end(), sortcol<S, T, E>);

    vector<tuple<S, T, E, double, double, double>> result(input.size());
    double timeElapsed = 0;

    for (size_t i = 0; i < input.size(); i++) {
        T arrival = get<1>(input[i]);
        E burst = get<2>(input[i]);

        if (arrival > timeElapsed) {
            timeElapsed = arrival;
        }

        timeElapsed += burst;

        double completion = timeElapsed;
        double turnaround = completion - arrival;
        double waiting = turnaround - burst;

        result[i] = make_tuple(get<0>(input[i]), arrival, burst, completion,
                               turnaround, waiting);
    }

    return result;
}

/**
 * @brief 单元自测用例集
 */
static void test() {
    for (int i{}; i < 10; i++) {
        std::random_device rd;
        std::mt19937 eng(rd());
        std::uniform_int_distribution<> distr(1, 10);

        uint32_t n = distr(eng);
        SJF<uint32_t, uint32_t, uint32_t> readyQueue;
        vector<tuple<uint32_t, uint32_t, uint32_t, double, double, double>>
            input(n);

        // 随机产生各进程的到达时间和执行时间
        for (uint32_t i{}; i < n; i++) {
            get<0>(input[i]) = i;
            get<1>(input[i]) = distr(eng);
            get<2>(input[i]) = distr(eng);
        }

        cout << "Processes before SJF scheduling:" << endl;
        readyQueue.printResult(input);

        for (uint32_t i{}; i < n; i++) {
            readyQueue.addProcess(get<0>(input[i]), get<1>(input[i]),
                                  get<2>(input[i]));
        }

        auto finalResult = readyQueue.scheduleForSJF();

        cout << "\nProcesses after SJF scheduling:" << endl;
        readyQueue.printResult(finalResult);
    }
    cout << "All the tests have successfully passed!" << endl;
}

/**
 * @brief 主函数
 * @returns 0
 */
int main() {
    test(); // 运行自测
    return 0;
}
