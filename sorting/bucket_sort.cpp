/**
 * @file
 * @brief Implementation of [Bucket Sort](https://en.wikipedia.org/wiki/Bucket_sort) algorithm (桶排序算法实现)
 *
 * @details
 * 桶排序（Bucket Sort）是一种分配式排序算法。
 * 它的工作原理是将数组元素分发到有限数量的“桶”里。每个桶再分别进行排序（通常使用其他排序算法或递归使用桶排序），
 * 最后将各个桶中的非空元素拼接，从而得到有序序列。
 *
 * ### 经典适用范围
 * - 本算法非常适合在**区间 `[0, 1)` 内均匀分布**的浮点数集合。
 *
 * @note
 * 【区间限制与越界崩溃 Bug 警示】：
 * 在第 13-14 行中：
 *   `int bi = n * arr[i];`
 *   `b[bi].push_back(arr[i]);`
 * 这里存在一个假设缺陷：如果输入数字小于 0，或者大于等于 1.0，
 * 计算得到的 `bi` 索引会变为负数或 $\ge n$。这会导致 `b[bi]` 发生**严重的数组越界（Out of Bounds）**内存崩溃。
 * 因此，使用该桶排序实现前，必须确保所有浮点数 $x$ 满足 $0 \le x < 1$。
 *
 * 时间复杂度:
 *   - 最好情况: $O(N)$ (当元素均匀分布到各个桶中时)
 *   - 最坏情况: $O(N^2)$ (所有元素都分发到了同一个桶中，退化为普通排序)
 * 空间复杂度: $O(N + K)$ (K 为桶的个数)
 *
 * @author Unknown author
 */

#include <algorithm>
#include <iostream>
#include <vector>

/**
 * @brief 桶排序算法主函数
 * @param arr 待排序浮点数数组
 * @param n 数组长度
 */
void bucketSort(float arr[], int n) {
    if (n <= 1) {
        return;
    }

    // 1) 动态创建 n 个空桶
    std::vector<float> *b = new std::vector<float>[n];

    // 2) 将数组元素按映射关系分发到相应的桶中
    for (int i = 0; i < n; i++) {
        // 校验输入范围以防越界崩溃
        if (arr[i] < 0.0f || arr[i] >= 1.0f) {
            std::cerr << "Error: Bucket sort elements must be in range [0, 1). Element " 
                      << arr[i] << " is out of bounds." << std::endl;
            delete[] b;
            return;
        }
        int bi = static_cast<int>(n * arr[i]); // 计算桶索引
        b[bi].push_back(arr[i]);
    }

    // 3) 对每个独立的桶内部进行排序（使用 std::sort 快速排序）
    for (int i = 0; i < n; i++) {
        std::sort(b[i].begin(), b[i].end());
    }

    // 4) 依次将各个有序桶中的数据合并回原数组
    int index = 0;
    for (int i = 0; i < n; i++) {
        for (size_t j = 0; j < b[i].size(); j++) {
            arr[index++] = b[i][j];
        }
    }
    
    delete[] b; // 释放桶分配的内存
}

/**
 * @brief 主函数
 */
int main() {
    float arr[] = {0.897f, 0.565f, 0.656f, 0.1234f, 0.665f, 0.3434f};
    int n = sizeof(arr) / sizeof(arr[0]);
    
    bucketSort(arr, n);

    std::cout << "Sorted array is \n";
    for (int i = 0; i < n; i++) {
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;
    return 0;
}
