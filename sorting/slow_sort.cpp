/**
 * @file
 * @brief Implementation of [Slowsort](https://en.wikipedia.org/wiki/Slowsort) algorithm (慢速排序算法实现)
 *
 * @details
 * 慢速排序（Slowsort）是一种具有幽默和调侃性质的“劣质”排序算法。
 * 它由 Andrei Broder 和 Jorge Stolfi 于 1986 年在论文《Pessimal Algorithms and Simplexity Analysis》（最劣算法与简单度分析）中发表。
 * 它的设计基于所谓的“相乘与投降”（Multiply and Surrender）原则，是对经典分治法（Divide and Conquer）的讽刺。
 *
 * ### 算法思想与时间复杂度
 * - 它是最慢的**但能确保收敛**的排序算法。
 * - **极其怪异的递归结构**：
 *   1. 递归排序左半部分 `SlowSort(a, i, m)`。
 *   2. 递归排序右半部分 `SlowSort(a, m + 1, j)`。
 *   3. 比较左右两端的最大值（分别在 `a[m]` 和 `a[j]`），如果 `a[j] < a[m]`，则将其交换（确保最大值移动到区间末尾 `a[j]`）。
 *   4. 丢弃当前区间的最末尾元素（因为最大值已经就位了），然后对剩下的 `[i, j - 1]` 范围**重新递归调用** `SlowSort(a, i, j - 1)`！
 * - 其最坏情况时间复杂度甚至达到了非多项式级 $\Omega(N^{\frac{\log N}{2 - o(1)}})$，比 $O(N^2)$ 还要慢得多，且比冒泡排序慢无数倍，绝对不具有任何工业应用可能。
 *
 * @note
 * 【原版代码的严重越界访问 Bug 审计与修复】：
 * 原版代码在 `main()` 函数中调用为：
 *   `SlowSort(arr, 0, size);`
 * 1. **越界访问**：在希尔排序中，`j` 代表闭区间的右端索引。如果传入 `size`，由于数组最大合法索引为 `size - 1`，
 *    代码会在第 21 行 `if (a[j] < a[m])` 中直接越界读取并可能交换 `a[size]` 的内存垃圾值。
 * 2. **修复方案**：将 `main()` 中调用处的右边界更正为合法的 `size - 1`：`SlowSort(arr, 0, size - 1);`，并添加空数组拦截。
 *
 * @author Unknown author
 */

#include <iostream>
#include <utility>

/**
 * @brief 慢速排序核心函数（使用闭区间 [i, j]）
 * @param a 待排序数组指针
 * @param i 区间左边界
 * @param j 区间右边界
 */
void SlowSort(int a[], int i, int j) {
    // 递归基：区间只剩 1 个或更少元素时返回
    if (i >= j) {
        return;
    }

    int m = i + (j - i) / 2; // 计算中点

    // 1. 递归排序左半部分 [i, m]
    SlowSort(a, i, m);
    // 2. 递归排序右半部分 [m + 1, j]
    SlowSort(a, m + 1, j);

    // 3. 比较区间中点与右终点，确保较大值移到末尾
    if (a[j] < a[m]) {
        std::swap(a[j], a[m]);
    }

    // 4. 最大值已经归位到 j，递归重新排序剩余的 [i, j - 1] 区间
    SlowSort(a, i, j - 1);
}

/**
 * @brief 主函数
 */
int main() {
    int size = 0;
    std::cout << "\nEnter the number of elements: ";
    std::cin >> size;
    
    if (size <= 0) {
        return 0;
    }

    int *arr = new int[size];
    std::cout << "\nEnter the unsorted elements: ";
    for (int i = 0; i < size; ++i) {
        std::cout << "Element " << i + 1 << ": ";
        std::cin >> arr[i];
    }

    // 核心修复：更正右边界参数为 size - 1，防止越界访问 a[size]
    SlowSort(arr, 0, size - 1);

    std::cout << "Sorted array: ";
    for (int i = 0; i < size; ++i) {
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;

    delete[] arr; // 释放内存
    return 0;
}
