/**
 * @file
 * @brief Implementation of [Timsort](https://en.wikipedia.org/wiki/Timsort) algorithm (Tim 排序算法实现)
 *
 * @details
 * Timsort 是一种高效的、混合的、稳定的排序算法，结合了**归并排序（Merge Sort）**和**插入排序（Insertion Sort）**。
 * 它由 Tim Peters 于 2002 年为 Python 语言开发，目前是 Python (自 2.3 起)、Java SE 7、Android 等平台中默认的通用排序算法。
 *
 * ### 算法原理
 * 1. **划分 Run**：Timsort 会将数组分割为多个称为 "Run" 的连续递增（或递减，如果是递减则会反转）子区间。
 *    对于小规模数据，或者为了提高性能，程序规定一个最小的 Run 长度（代码中硬编码为 `RUN = 32`）。
 * 2. **局部插入排序**：遍历数组，将每个大小为 `RUN` 的小区间，使用**插入排序（Insertion Sort）**排成有序。
 * 3. **迭代自底向上归并**：在所有小区间有序后，从 `size = RUN` 开始，
 *    以二倍递增的方式（32 -> 64 -> 128 ...）在原数组上迭代进行双路归并，直至整个数组完全有序。
 *
 * 时间复杂度:
 *   - 最好情况: $O(N)$ (当输入数组已完全有序时)
 *   - 最坏情况: $O(N \log N)$
 *   - 平均情况: $O(N \log N)$
 * 空间复杂度: $O(N)$ (归并时需要分配临时辅助存储空间)
 * 
 * @author Unknown author
 */

#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <new>

// 最小 Run 区间大小
const int RUN = 32;

/**
 * @brief 局域插入排序（闭区间 [left, right]）
 * @param arr 数组指针
 * @param left 区间左端点索引
 * @param right 区间右端点索引
 */
void insertionSort(int arr[], int left, int right) {
    for (int i = left + 1; i <= right; i++) {
        const int temp = arr[i];
        int j = i - 1;
        // 将大于 temp 的元素向右挪动
        while (j >= left && arr[j] > temp) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = temp;
    }
}

/**
 * @brief 归并两个相邻的已排序区间：[l, m] 和 [m + 1, r]
 * @param arr 数组指针
 * @param l 左区间起点
 * @param m 中点（左区间终点）
 * @param r 右区间终点
 */
void merge(int arr[], int l, int m, int r) {
    // 原始数组划分为两个子区间：左部分和右部分
    const int len1 = m - l + 1;
    const int len2 = r - m;

    // 安全检查：如果右区间没有元素，无需进行归并
    if (len2 <= 0) {
        return;
    }

    int *left = new int[len1];
    int *right = new int[len2];

    // 复制数据到临时数组中
    for (int i = 0; i < len1; i++) {
        left[i] = arr[l + i];
    }
    for (int i = 0; i < len2; i++) {
        right[i] = arr[m + 1 + i];
    }

    int i = 0; // 左数组指针
    int j = 0; // 右数组指针
    int k = l; // 原数组写回指针

    // 双路归并合并
    while (i < len1 && j < len2) {
        if (left[i] <= right[j]) {
            arr[k] = left[i];
            i++;
        } else {
            arr[k] = right[j];
            j++;
        }
        k++;
    }

    // 拷贝左数组中剩余的元素
    while (i < len1) {
        arr[k] = left[i];
        k++;
        i++;
    }

    // 拷贝右数组中剩余的元素
    while (j < len2) {
        arr[k] = right[j];
        k++;
        j++;
    }

    // 释放临时分配内存
    delete[] left;
    delete[] right;
}

/**
 * @brief Timsort 算法入口
 * @param arr 待排序数组
 * @param n 数组长度
 */
void timSort(int arr[], int n) {
    // 核心安全防卫，防止 n <= 1 或负数时产生不正确的索引越界
    if (n <= 1) {
        return;
    }

    // 1. 对大小为 RUN 的每个小区间进行局部插入排序
    for (int i = 0; i < n; i += RUN) {
        insertionSort(arr, i, std::min((i + RUN - 1), (n - 1)));
    }

    // 2. 自底向上迭代归并已排序的小区间
    for (int size = RUN; size < n; size = 2 * size) {
        for (int left = 0; left < n; left += 2 * size) {
            // 计算中点 mid 以及右边界 right
            const int mid = std::min((left + size - 1), (n - 1));
            const int right = std::min((left + 2 * size - 1), (n - 1));

            // 合并 arr[left...mid] 和 arr[mid+1...right]
            merge(arr, left, mid, right);
        }
    }
}

/**
 * @brief 辅助打印数组
 */
void printArray(int arr[], int n) {
    for (int i = 0; i < n; i++) {
        std::printf("%d  ", arr[i]);
    }
    std::cout << std::endl;
}

/**
 * @brief 单元自测用例
 */
void tests() {
    // 情况 1：长度为 65 的逆序数组（验证跨越 RUN 边界时的排序正确性）
    constexpr int N = 65;
    int arr[N];

    std::iota(arr, arr + N, 0);
    std::reverse(arr, arr + N);
    assert(!std::is_sorted(arr, arr + N));

    timSort(arr, N);
    assert(std::is_sorted(arr, arr + N));
    std::cout << "Test with 65 reverse elements passed!" << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    tests(); // 运行测试用例

    int arr[] = {5, 21, 7, 23, 19};
    const int n = sizeof(arr) / sizeof(arr[0]);
    std::printf("Given Array is\n");
    printArray(arr, n);

    timSort(arr, n);

    std::printf("After Sorting Array is\n");
    printArray(arr, n);
    return 0;
}
