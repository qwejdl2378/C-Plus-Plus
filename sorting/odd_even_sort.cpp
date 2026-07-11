/**
 * @file
 * @brief Implementation of [Odd-Even Sort / Brick Sort](https://en.wikipedia.org/wiki/Odd%E2%80%93even_sort) algorithm (奇偶排序 / 砖块排序算法实现)
 *
 * @details
 * 奇偶排序（Odd-Even Sort，或称砖块排序 Brick Sort）是一种并行的比较排序算法，最初设计用于具有本地互连的并行处理器上。
 * 它的核心逻辑非常像双向波动的冒泡排序：
 * 算法交替进行两趟扫描：
 * 1. **奇数相位（Odd Phase）**：比较并交换所有奇数索引处的相邻对，即对每个奇数 $i$，比较 `(arr[i], arr[i+1])`。
 * 2. **偶数相位（Even Phase）**：比较并交换所有偶数索引处的相邻对，即对每个偶数 $i$，比较 `(arr[i], arr[i+1])`。
 * 
 * 只要在任意一个相位中发生了元素交换，就将 `sorted` 标记设为 `false` 并继续迭代。
 * 当在一整轮（奇数相位 + 偶数相位）中没有发生任何交换时，说明数组已经完全有序，算法结束。
 *
 * 时间复杂度:
 *   - 最好情况: $O(N)$
 *   - 最坏情况: $O(N^2)$ (在单处理器串行运行时)
 *   - 平均情况: $O(N^2)$
 *   - 完全并行化处理下，可在 $O(N)$ 时间内完成排序。
 * 空间复杂度: $O(1)$
 * 
 * @author Unknown author
 */

#include <iostream>
#include <vector>
#include <utility>

/**
 * @brief 奇偶排序算法主函数
 * @param arr 待排序数组的引用
 * @param size 数组元素个数
 */
void oddEven(std::vector<int> &arr, int size) {
    if (size <= 1) {
        return;
    }

    bool sorted = false;
    while (!sorted) {
        sorted = true; // 先假设已经排好序

        // 1. 奇数相位 (Odd Phase)
        for (int i = 1; i < size - 1; i += 2) {
            if (arr[i] > arr[i + 1]) {
                std::swap(arr[i], arr[i + 1]);
                sorted = false; // 发生交换，重置标记
            }
        }

        // 2. 偶数相位 (Even Phase)
        for (int i = 0; i < size - 1; i += 2) {
            if (arr[i] > arr[i + 1]) {
                std::swap(arr[i], arr[i + 1]);
                sorted = false; // 发生交换，重置标记
            }
        }
    }
}

/**
 * @brief 打印数组内容
 * @param A 打印的数组（传值产生副本）
 * @param size 数组长度
 */
void show(const std::vector<int> &A, int size) {
    for (int i = 0; i < size; i++) {
        std::cout << A[i] << " ";
    }
    std::cout << "\n";
}

/**
 * @brief 主函数
 */
int main() {
    int size = 0, temp = 0;
    std::cout << "\nEnter the number of elements: ";
    std::cin >> size;
    if (size <= 0) {
        return 0;
    }

    std::vector<int> arr;
    std::cout << "\nEnter the unsorted elements: \n";
    for (int i = 0; i < size; ++i) {
        std::cin >> temp;
        arr.push_back(temp);
    }

    oddEven(arr, size);

    std::cout << "Sorted array:\n";
    show(arr, size);
    return 0;
}
