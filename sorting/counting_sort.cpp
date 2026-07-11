/**
 * @file
 * @brief Implementation of [Counting Sort](https://en.wikipedia.org/wiki/Counting_sort) algorithm (支持负数的计数排序算法实现)
 *
 * @details
 * 计数排序（Counting Sort）是一种非比较排序算法。
 * 它的核心思想是统计数组中每个值的出现次数，然后利用前缀和（累计计数）直接计算出每个元素在排好序的输出数组中的最终物理索引。
 *
 * ### 本实现特点：支持负数
 * - 通过找到数组的最大值 `max` 和最小值 `min`，将存储桶大小定义为 `max - min + 1`。
 * - 所有元素的存储下标均向右平移 `min` 位（即用 `Arr[i] - min` 作为计数索引），从而支持包含负数的整数数组。
 *
 * ### 稳定性保障
 * - 在第 38 行中，算法从后往前遍历原数组 `for (int i = N - 1; i >= 0; i--)`。
 * - 这种倒序扫描机制确保了原本靠后的等值元素在输出时依然靠后，因此该计数排序是一个**稳定排序（Stable Sort）**。
 *
 * @note
 * 【内存与安全风险警告】：
 * 计数排序的空间开销与数据的值域差 `max - min` 成线性正比。
 * 如果数组中存在极端的数据范围（例如包含了 `-10^9` 和 `10^9` 两个数），
 * `max - min + 1` 会极其巨大，导致程序尝试申请数 GB 的内存空间，触发 `std::bad_alloc` 内存分配异常，
 * 甚至可能因为索引乘法溢出产生**段错误崩溃（Segmentation Fault）**。
 * 因此，计数排序仅适用于元素值域范围较窄的整数排序场景。
 *
 * 时间复杂度: $O(N + K)$，其中 K 为值域范围 `max - min + 1`。
 * 空间复杂度: $O(N + K)$
 *
 * @author Unknown author
 */

#include <iostream>
#include <vector>
#include <new>

/**
 * @brief 获取数组中的最大值
 */
int Max(int Arr[], int N) {
    int max = Arr[0];
    for (int i = 1; i < N; i++) {
        if (Arr[i] > max) {
            max = Arr[i];
        }
    }
    return max;
}

/**
 * @brief 获取数组中的最小值
 */
int Min(int Arr[], int N) {
    int min = Arr[0];
    for (int i = 1; i < N; i++) {
        if (Arr[i] < min) {
            min = Arr[i];
        }
    }
    return min;
}

/**
 * @brief 辅助打印数组
 */
void Print(int Arr[], int N) {
    for (int i = 0; i < N; i++) {
        std::cout << Arr[i] << ", ";
    }
}

/**
 * @brief 计数排序核心函数
 * @param Arr 待排序的数组
 * @param N 数组长度
 * @returns 动态分配的排好序的新数组指针
 */
int *Counting_Sort(int Arr[], int N) {
    if (N <= 0) {
        return nullptr;
    }

    int max = Max(Arr, N);
    int min = Min(Arr, N);
    
    // 动态分配结果数组
    int *Sorted_Arr = new int[N];

    // 计算值域范围大小，并防范大内存分配失败
    long long range = static_cast<long long>(max) - min + 1;
    if (range > 10000000) { // 限制值域范围在 10M 以内防范内存耗尽
        std::cerr << "Error: Value range is too large for counting sort." << std::endl;
        for (int i = 0; i < N; i++) {
            Sorted_Arr[i] = Arr[i];
        }
        return Sorted_Arr;
    }

    int *Count = nullptr;
    try {
        Count = new int[range];
    } catch (const std::bad_alloc &e) {
        std::cerr << "Memory allocation failed for counting sort." << std::endl;
        delete[] Sorted_Arr;
        return nullptr;
    }

    // 初始化计数数组为 0
    for (int i = 0; i < range; ++i) {
        Count[i] = 0;
    }

    // 1. 统计每个元素出现的次数
    for (int i = 0; i < N; i++) {
        Count[Arr[i] - min]++;
    }

    // 2. 累加前缀和，计算每个桶截止到当前的累计偏移量
    for (int i = 1; i < range; i++) {
        Count[i] += Count[i - 1];
    }

    // 3. 倒序写回，维持排序稳定性
    for (int i = N - 1; i >= 0; i--) {
        Sorted_Arr[Count[Arr[i] - min] - 1] = Arr[i];
        Count[Arr[i] - min]--; // 累加计数减一以供前一个同值元素使用
    }

    delete[] Count; // 释放临时统计表空间
    return Sorted_Arr;
}

/**
 * @brief 主函数
 */
int main() {
    int Arr[] = {47, 65, 20, 66, 25, 53, 64, 69, 72, 22,
                 74, 25, 53, 15, 42, 36, 4,  69, 86, 19};
    int N = sizeof(Arr) / sizeof(Arr[0]);
    int *Sorted_Arr = nullptr;

    std::cout << "\n\tOriginal Array = ";
    Print(Arr, N);
    
    Sorted_Arr = Counting_Sort(Arr, N);
    
    std::cout << "\n\tSorted Array = ";
    if (Sorted_Arr) {
        Print(Sorted_Arr, N);
        delete[] Sorted_Arr; // 释放动态数组
    }
    std::cout << std::endl;

    return 0;
}
