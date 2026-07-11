/**
 * @file
 * @brief Implementation of recursive [Bubble Sort](https://www.geeksforgeeks.org/recursive-bubble-sort/) algorithm (递归版冒泡排序算法实现)
 *
 * @details
 * 递归版冒泡排序通过函数递归调用栈取代了传统冒泡排序的外层循环。
 * 
 * ### 递归过程
 * 1. **单趟冒泡**：通过单层 `for` 循环遍历已缩小的子区间 `[0, n-1]`，将当前区间内的最大值通过相邻交换“浮动”到该区间的末尾位置 `n-1`。
 * 2. **锁定末尾并递归**：最后一项元素已经归位。递归调用自身对剩余前 `n-1` 个元素继续进行冒泡排序。
 * 3. **递归出口（Base Case）**：当子区间大小 `n` 缩小到 1 时，意味着全部元素均已排好序，递归返回。
 *
 * 时间复杂度: $O(N^2)$ (最坏/平均)，$O(N)$ (最好，若加入单趟交换检查优化)
 * 空间复杂度: $O(N)$ (递归调用栈深度达 N 层)
 *
 * @note
 * 【空数组下溢越界 Bug 审计与修复】：
 * 原代码的递归基判定为：`if (n == 1) return;`。
 * **如果输入数组为空（`nums->size() == 0`，即 `n = 0`），**
 * 代码会跳过递归基校验，进入循环 `for (uint64_t i = 0; i < n - 1; i++)`。
 * 由于 `n` 是无符号整型 `uint64_t`，`0 - 1` 会产生**无符号整数下溢（Unsigned Underflow）**，
 * 变成 $2^{64}-1$，导致循环启动并越界访问空 vector 的元素，造成**段错误崩溃（Segmentation Fault）**。
 * 
 * ### 修复方案
 * - 将递归出口修改为更安全的：`if (n <= 1) return;`。
 *
 * @author [Aditya Prakash](https://adityaprakash.tech)
 */

#include <algorithm>  /// 用于 std::is_sorted
#include <cassert>    /// 用于 assert 断言
#include <cstdint>   /// 用于 uint64_t
#include <iostream>   /// 用于输入输出
#include <vector>     /// 用于 std::vector

/**
 * @namespace sorting
 * @brief 排序算法命名空间
 */
namespace sorting {

/**
 * @brief 递归版冒泡排序主函数
 * @tparam T 元素模板类型
 * @param nums 待排序的 vector 指针
 * @param n 当前待排序子区间的长度
 */
template <typename T>
void recursive_bubble_sort(std::vector<T> *nums, uint64_t n) {
    // 核心修复：修改为 <= 1，防止空数组输入时产生 unsigned 0-1 下溢越界崩溃
    if (n <= 1) {
        return;
    }

    // 单趟扫描：将当前范围内的最大值交换到末尾
    for (uint64_t i = 0; i < n - 1; i++) {
        if ((*nums)[i] > (*nums)[i + 1]) {
            std::swap((*nums)[i], (*nums)[i + 1]);
        }
    }

    // 递归处理剩下前 n-1 个元素的子区间
    recursive_bubble_sort(nums, n - 1);
}
}  // namespace sorting

/**
 * @brief 单元自测用例
 */
static void test() {
    // 测试 1: 整型数组
    std::cout << "1st test using `int`\n";
    const uint64_t size = 6;
    std::vector<int64_t> arr;
    arr.push_back(22);
    arr.push_back(46);
    arr.push_back(94);
    arr.push_back(12);
    arr.push_back(37);
    arr.push_back(63);

    sorting::recursive_bubble_sort(&arr, size);
    assert(std::is_sorted(std::begin(arr), std::end(arr)));
    std::cout << " 1st test passed!\n";
    for (uint64_t i = 0; i < size; i++) {
        std::cout << arr[i] << ", ";
    }
    std::cout << std::endl;

    // 测试 2: 浮点数数组
    std::cout << "2nd test using doubles\n";
    std::vector<double> double_arr;
    double_arr.push_back(20.4);
    double_arr.push_back(62.7);
    double_arr.push_back(12.2);
    double_arr.push_back(43.6);
    double_arr.push_back(74.1);
    double_arr.push_back(57.9);

    sorting::recursive_bubble_sort(&double_arr, size);
    assert(std::is_sorted(std::begin(double_arr), std::end(double_arr)));
    std::cout << " 2nd test passed!\n";
    for (uint64_t i = 0; i < size; i++) {
        std::cout << double_arr[i] << ", ";
    }
    std::cout << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    test();  // 运行自测
    return 0;
}
