/**
 * @file
 * @brief Implementation of iterative Quick Sort using user-managed stack (基于自定义栈的非递归/迭代快速排序算法实现)
 *
 * @details
 * 迭代版快速排序（Iterative Quick Sort）通过显式地在堆上维护一个 `std::stack`，
 * 代替了函数递归调用时的系统调用栈，以此来保存和恢复划分区间边界。
 * 
 * ### 与递归版的性能对比
 * - **时间复杂度**：与递归快排一致，最好/平均 $O(N \log N)$，最坏 $O(N^2)$。
 * - **空间复杂度**：由于消除了系统递归，极大地降低了系统函数调用栈开销，特别是在深层递归时防止了操作系统级的**栈溢出（Stack Overflow）**。
 *   自定义栈的最大空间复杂度为 $O(N)$。
 *
 * @note
 * 【空数组越界/下溢 Bug 审计与修复】：
 * 原版代码直接执行：
 *   `int end = arr.size() - 1;`
 *   `stack.push(start);`
 *   `stack.push(end);`
 * 若传入的 vector 数组为空（`arr.size() == 0`），那么 `end` 将下溢为 `-1`。
 * 导致在第一轮循环中直接调用 `partition(arr, 0, -1)`。
 * 并在 `partition` 函数内部尝试访问 `arr[-1]`，这会直接触发 **段错误崩溃（Segmentation Fault）**。
 * 
 * ### 修复方案
 * - 在 `iterativeQuickSort` 函数入口处添加 `if (arr.size() <= 1) return;`，保证空数组和单元素数组安全直接退出。
 *
 * @author [Sebe324](https://github.com/sebe324)
 */

#include <iostream>  /// 用于 std::cout
#include <vector>    /// 用于 std::vector
#include <stack>     /// 用于 std::stack
#include <algorithm> /// 用于 std::is_sorted
#include <cassert>   /// 用于 assert 断言

/**
 * @namespace sorting
 * @brief 排序算法命名空间
 */
namespace sorting {

/**
 * @brief 单向划分（Lomuto 划分方案），以最后一个元素作为 Pivot 基准值
 * @param arr 待划分数组的引用
 * @param start 起始索引
 * @param end 结束索引
 * @returns 划分完成后基准值的正确最终索引
 */
int partition(std::vector<int> &arr, int start, int end) {
    int pivot = arr[end];
    int index = start - 1;

    for (int j = start; j < end; j++) {
        if (arr[j] <= pivot) {
            std::swap(arr[++index], arr[j]);
        }
    }

    std::swap(arr[index + 1], arr[end]);
    return index + 1;
}

/**
 * @brief 迭代快速排序主函数
 * @param arr 待排序数组的引用
 */
void iterativeQuickSort(std::vector<int> &arr) {
    // 核心修复：添加空安全保护，防止 arr.size() 为 0 时产生越界崩溃
    if (arr.size() <= 1) {
        return;
    }

    std::stack<int> stack;
    int start = 0;
    int end = arr.size() - 1;

    // 将初始左右边界推入栈中
    stack.push(start);
    stack.push(end);

    while (!stack.empty()) {
        // 先出栈右边界，再出栈左边界
        end = stack.top();
        stack.pop();
        start = stack.top();
        stack.pop();

        // 执行单向划分，定位 Pivot
        int pivotIndex = partition(arr, start, end);

        // 如果 Pivot 左侧还有未排序区间，将左区间边界推入栈中
        if (pivotIndex - 1 > start) {
            stack.push(start);
            stack.push(pivotIndex - 1);
        }

        // 如果 Pivot 右侧还有未排序区间，将右区间边界推入栈中
        if (pivotIndex + 1 < end) {
            stack.push(pivotIndex + 1);
            stack.push(end);
        }
    }
}

} // namespace sorting

/**
 * @brief 单元自测用例
 */
void tests() {
    // 测试 1: 正数
    std::vector<int> case1 = {100, 534, 1000000, 553, 10, 61, 2000, 238, 2756, 9, 12, 56, 30};
    std::cout << "TEST 1\n";
    std::cout << "Before: \n";
    for (auto x : case1) std::cout << x << ",";
    std::cout << "\n";
    sorting::iterativeQuickSort(case1);
    assert(std::is_sorted(std::begin(case1), std::end(case1)));
    std::cout << "Test 1 successful!\n";
    std::cout << "After: \n";
    for (auto x : case1) std::cout << x << ",";
    std::cout << "\n";

    // 测试 2: 负数
    std::vector<int> case2 = {-10, -2, -5, -2, -3746, -785, -123, -452, -32456};
    std::cout << "TEST 2\n";
    std::cout << "Before: \n";
    for (auto x : case2) std::cout << x << ",";
    std::cout << "\n";
    sorting::iterativeQuickSort(case2);
    assert(std::is_sorted(std::begin(case2), std::end(case2)));
    std::cout << "Test 2 successful!\n";
    std::cout << "After: \n";
    for (auto x : case2) std::cout << x << ",";
    std::cout << "\n";
}

/**
 * @brief 主函数
 */
int main() {
    tests(); // 运行自测
    return 0;
}
