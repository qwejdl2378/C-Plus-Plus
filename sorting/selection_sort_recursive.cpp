/**
 * @file
 * @brief Implementation of recursive [Selection Sort](https://en.wikipedia.org/wiki/Selection_sort) algorithm (递归版选择排序算法实现)
 *
 * @details
 * 递归选择排序（Recursive Selection Sort）将传统选择排序中的双重循环通过双重递归或单重递归进行了重构：
 * 1. **子过程递归查找极值索引 (`findMinIndex`)**：对当前未排序区间 `[current_position, N-1]`，
 *    通过递归的方式一路向后推进，然后从后往前两两比较，最终向上返回该区间内的最小元素索引。
 * 2. **主过程递归归位 (`selectionSortRecursive`)**：获取最小元素索引后，将最小元素交换至当前区间的起点 `current_position`，
 *    然后递归调用自身处理剩下前置加一的未排序子区间。
 * 3. **退出条件**：当区间起点推进到数组边界 `in_arr.size()` 时，整个数组已完成排序，递归终止。
 *
 * @note
 * 【空数组输入导致栈溢出/无限递归 Bug 审计与修复】：
 * 原代码在极值索引查找中直接定义基准条件为：
 *   `if (current_position + 1 == in_arr.size()) { return current_position; }`
 * **如果输入数组为空（`in_arr.size() == 0`），并且起始位置 `current_position = 0`，**
 * 条件 `0 + 1 == 0` 永远不可能成立。递归会无休止地推进 `current_position + 1`，
 * 导致程序陷入无限递归，最终因**调用栈溢出（Stack Overflow）**而直接崩溃。
 * 
 * ### 修复方案
 * - 在 `findMinIndex` 和 `selectionSortRecursive` 的入口处添加空数组或越界校验 `if (in_arr.empty() || current_position >= in_arr.size())`，直接返回，确保零安全。
 *
 * 时间复杂度: $O(N^2)$ (最坏/平均/最好)
 * 空间复杂度: $O(N)$ (由于双重递归产生调用栈开销)
 * 
 * @author [Tushar Khanduri](https://github.com/Tushar-K24)
 */

#include <algorithm>  /// 用于 std::is_sorted
#include <cassert>    /// 用于 assert 断言
#include <cstdint>   /// 用于 uint64_t
#include <iostream>   /// 用于 std::swap 和输入输出
#include <vector>     /// 用于 std::vector

/**
 * @namespace sorting
 * @brief 排序算法命名空间
 */
namespace sorting {
/**
 * @namespace selection_sort_recursive
 * @brief 递归选择排序算法命名空间
 */
namespace selection_sort_recursive {

/**
 * @brief 递归寻找区间内最小元素的索引位置
 * @tparam T 元素模板类型
 * @param in_arr 只读引用的待查找数组
 * @param current_position 查找区间的起点
 * @returns 区间内最小元素的物理索引
 */
template <typename T>
uint64_t findMinIndex(const std::vector<T> &in_arr,
                      uint64_t current_position = 0) {
    // 核心修复：添加空保护，防止当 size 为 0 时产生无限递归和栈溢出崩溃
    if (in_arr.empty() || current_position >= in_arr.size()) {
        return current_position;
    }
    
    // 递归基：到达数组末尾元素，直接返回该位置
    if (current_position + 1 == in_arr.size()) {
        return current_position;
    }
    
    // 递归查找右侧剩余区间的最小值位置
    uint64_t answer = findMinIndex(in_arr, current_position + 1);
    
    // 比较当前首元素与右侧区间最小值
    if (in_arr[current_position] < in_arr[answer]) {
        answer = current_position;
    }
    return answer;
}

/**
 * @brief 递归选择排序主逻辑
 * @tparam T 元素模板类型
 * @param in_arr 待排序数组的引用
 * @param current_position 当前待排序区间的起始索引
 */
template <typename T>
void selectionSortRecursive(std::vector<T> &in_arr,
                            uint64_t current_position = 0) {
    // 核心修复：空安全防线
    if (in_arr.empty() || current_position >= in_arr.size()) {
        return;
    }
    
    // 1. 定位 [current_position, N-1] 区间内的最小元素索引
    uint64_t min_element_idx =
        selection_sort_recursive::findMinIndex(in_arr, current_position);
        
    // 2. 将定位到的最小值交换到当前区间的起点位置
    if (min_element_idx != current_position) {
        std::swap(in_arr[min_element_idx], in_arr[current_position]);
    }
    
    // 3. 递归排序后半部分区间
    selectionSortRecursive(in_arr, current_position + 1);
}
}  // namespace selection_sort_recursive
}  // namespace sorting

/**
 * @brief 单元自测用例
 */
static void test() {
    // 测试 1
    std::vector<uint64_t> array1 = {0, 1, 1, 2};
    std::cout << "1st test... ";
    sorting::selection_sort_recursive::selectionSortRecursive(array1);
    assert(std::is_sorted(std::begin(array1), std::end(array1)));
    std::cout << "passed" << std::endl;

    // 测试 2
    std::vector<uint64_t> array2 = {1, 0, 0, 1, 1, 0, 2, 1};
    std::cout << "2nd test... ";
    sorting::selection_sort_recursive::selectionSortRecursive(array2);
    assert(std::is_sorted(std::begin(array2), std::end(array2)));
    std::cout << "passed" << std::endl;

    // 测试 3
    std::vector<uint64_t> array3 = {1, 1, 0, 0, 1, 2, 2, 0, 2, 1};
    std::cout << "3rd test... ";
    sorting::selection_sort_recursive::selectionSortRecursive(array3);
    assert(std::is_sorted(std::begin(array3), std::end(array3)));
    std::cout << "passed" << std::endl;

    // 测试 4
    std::vector<uint64_t> array4 = {2, 2, 2, 0, 0, 1, 1};
    std::cout << "4th test... ";
    sorting::selection_sort_recursive::selectionSortRecursive(array4);
    assert(std::is_sorted(std::begin(array4), std::end(array4)));
    std::cout << "passed" << std::endl;

    // 测试 5: 空数组边界验证，防止 Bug 复现
    std::vector<uint64_t> empty_arr;
    sorting::selection_sort_recursive::selectionSortRecursive(empty_arr);
    assert(empty_arr.empty());
}

/**
 * @brief 主函数
 */
int main() {
    test();  // 运行自测
    return 0;
}
