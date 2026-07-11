/**
 * @file
 * @brief Implementation of [Stooge Sort](https://en.wikipedia.org/wiki/Stooge_sort) algorithm (臭皮匠排序算法实现)
 *
 * @details
 * 臭皮匠排序（Stooge Sort）是一种非常慢且奇特低效的递归排序算法。
 * 其命名可能来源于“三个臭皮匠顶个诸葛亮”的调侃。
 *
 * ### 算法思想
 * 对于区间 `[i, j]` 上的元素：
 * 1. 如果起点元素 `arr[i]` 大于终点元素 `arr[j]`，则交换它们。
 * 2. 如果当前区间的元素个数大于 2（即 `j - i > 1`）：
 *    - 计算三分之一的步长：`third = (j - i + 1) / 3`。
 *    - 递归排序前三分之二：`stoogeSort(L, i, j - third)`。
 *    - 递归排序后三分之二：`stoogeSort(L, i + third, j)`。
 *    - 再次递归排序前三分之二：`stoogeSort(L, i, j - third)`，以确保整体完全有序。
 *
 * ### 性能
 * - 时间复杂度约为 $O(N^{\frac{\log 3}{\log 1.5}}) \approx O(N^{2.7})$。它的时间开销甚至比选择排序和插入排序还要高得多。
 * - 空间复杂度: $O(N)$ (递归栈开销)
 *
 * @note
 * 【空数组下溢越界 Bug 审计与修复】：
 * 原代码直接在自测或调用中写为 `stoogeSort(&L, 0, L.size() - 1);`。
 * **如果传入的 vector 为空（`L.size() == 0`），**
 * `L.size() - 1` 会产生**无符号整数下溢（Unsigned Underflow）**，变为最大的 `size_t`。
 * 导致条件 `i >= j`（即 `0 >= UINT64_MAX`）不成立，随后尝试读取 `(*L)[0]` 和 `(*L)[UINT64_MAX]` 导致程序**越界崩溃**。
 * 
 * ### 修复方案
 * - 在函数入口处增加空指针和空数组检查：`if (L == nullptr || L->empty()) return;`。
 *
 * @author Unknown author
 */

#include <vector>     /// 用于 std::vector
#include <cassert>    /// 用于 assert 断言
#include <algorithm>  /// 用于 std::is_sorted
#include <iostream>   /// 用于标准输出

/**
 * @brief 臭皮匠排序主递归函数
 * @param L 待排序 vector 指针
 * @param i 区间左边界
 * @param j 区间右边界
 */
void stoogeSort(std::vector<int>* L, size_t i, size_t j) {
    // 核心修复：空安全防线，且防止 j - 1 下溢
    if (L == nullptr || L->empty()) {
        return;
    }
    if (i >= j) {
        return;
    }
    
    // 如果首元素大于尾元素，执行交换
    if ((*L)[i] > (*L)[j]) {
        std::swap((*L)[i], (*L)[j]);
    }
    
    // 如果元素个数大于 2，按照三步进行划分递归
    if (j - i > 1) {
        size_t third = (j - i + 1) / 3;
        stoogeSort(L, i, j - third);       // 排序前 2/3
        stoogeSort(L, i + third, j);       // 排序后 2/3
        stoogeSort(L, i, j - third);       // 再次排序前 2/3 保证稳定
    }
}

/**
 * @brief 单元自测用例 1
 */
void test1() {
    std::vector<int> L = { 8, 9, 10, 4, 3, 5, 1 };
    stoogeSort(&L, 0, L.size() - 1);
    assert(std::is_sorted(std::begin(L), std::end(L)));
}

/**
 * @brief 单元自测用例 2: 单个元素边界
 */
void test2() {
    std::vector<int> L = { -1 };
    stoogeSort(&L, 0, L.size() - 1);
    assert(std::is_sorted(std::begin(L), std::end(L)));
}

/**
 * @brief 单元自测用例 3: 重复元素
 */
void test3() {
    std::vector<int> L = { 1, 2, 5, 4, 1, 5 };
    stoogeSort(&L, 0, L.size() - 1);
    assert(std::is_sorted(std::begin(L), std::end(L)));
}

/**
 * @brief 单元自测用例 4: 空数组安全验证
 */
void test4() {
    std::vector<int> L;
    stoogeSort(&L, 0, 0); // 确保不会因 L.size() - 1 下溢崩溃
    assert(L.empty());
}

/**
 * @brief 主函数
 */
int main() {
    test1();
    test2();
    test3();
    test4();
    
    std::cout << "All tests have successfully passed!\n";
    return 0;
}
