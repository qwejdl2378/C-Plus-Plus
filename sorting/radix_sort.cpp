/**
 * @file
 * @brief Implementation of [Radix Sort](https://en.wikipedia.org/wiki/Radix_sort) algorithm (基数排序算法实现)
 *
 * @details
 * 基数排序（Radix Sort）是一种非比较型整数排序算法。
 * 它的原理是将整数按位数切割成不同的数字，然后按每个位数分别比较进行排序。
 * 通常采用最低有效位优先（LSD, Least Significant Digit first）法。
 * 
 * ### LSD 基数排序过程
 * 1. 找到数组中的最大值，确定其最大位数 `maxdigits`。
 * 2. 从个位（$10^0$）开始，对所有数按当前位的数值（0~9）进行桶分配和收集。
 * 3. 依次向高位进行（十位 $10^1$、百位 $10^2$ ...），直到最高位。由于低位排序是**稳定的（Stable）**，高位排序完毕后，整个数组即完全有序。
 *
 * @note
 * 【局限性与性能审计】：
 * 1. **仅限非负整数**：本代码中未考虑负数取模后的负下标问题。如果数组中包含负数，`a[i] % (10 * t)` 会得到负数结果，
 *    进而尝试写入 `count` 数组的负索引，导致**越界访问或内存段错误崩溃**。
 * 2. **浮点运算隐患**：代码中使用 `std::pow(10, j)` 计算位权重。在 C++ 中，`std::pow` 是浮点运算，对于高位运算可能存在精度失真，且效率较低。推荐在外部维护一个整数变量 `exp`（初始为 1），每次迭代乘 10。
 * 3. **多余遍历开销**：在收集阶段，代码为了维持稳定性，对每个数位数字 $p \in [0, 9]$ 都完整遍历了一遍数组（见第 28-35 行，相当于内层跑了 10 次 $O(N)$ 遍历）。
 *    这在数据量大时会带来巨大的多余计算。可以使用经典的计数排序（累计直方图偏移量）将收集耗时降为单次 $O(N)$。
 *
 * 时间复杂度: $O(d \cdot (N + K))$，其中 d 为最大位数，K 为基数（此处为 10）。
 * 空间复杂度: $O(N + K)$
 * 
 * @author Unknown author
 */

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>

/**
 * @brief 基数排序主函数
 * @param a 待排序整型数组
 * @param n 数组长度
 */
void radixsort(int a[], int n) {
    if (n <= 1) {
        return;
    }

    int count[10];
    int* output = new int[n];
    std::memset(output, 0, n * sizeof(*output));
    std::memset(count, 0, sizeof(count));

    // 1. 寻找数组最大值以确定最高位数
    int max = a[0];
    for (int i = 1; i < n; ++i) {
        if (a[i] > max) {
            max = a[i];
        }
    }

    // 2. 计算最大值的十进制位数
    int maxdigits = 0;
    int temp_max = max;
    while (temp_max > 0) {
        maxdigits++;
        temp_max /= 10;
    }

    // 3. 从低位到高位依次进行稳定排序
    int exp = 1; // 维护数位权重（1, 10, 100...）代替 std::pow 浮点函数
    for (int j = 0; j < maxdigits; j++) {
        // 统计当前数位上数字的出现分布
        for (int i = 0; i < n; i++) {
            if (a[i] < 0) {
                std::cerr << "Error: Radix sort does not support negative numbers." << std::endl;
                delete[] output;
                return;
            }
            int digit = (a[i] / exp) % 10;
            count[digit]++;
        }

        // 利用桶分配，从小到大（0~9）收集元素，维持排序稳定性
        int k = 0;
        for (int p = 0; p < 10; p++) {
            for (int i = 0; i < n; i++) {
                int digit = (a[i] / exp) % 10;
                if (digit == p) {
                    output[k] = a[i];
                    k++;
                }
            }
        }

        // 清空计数器并更新当前权重倍数
        std::memset(count, 0, sizeof(count));
        exp *= 10;

        // 回写数据到原数组，作为下一位排序的基础
        for (int i = 0; i < n; ++i) {
            a[i] = output[i];
        }
    }
    
    delete[] output; // 释放临时结果空间
}

/**
 * @brief 打印数组
 */
void print(int a[], int n) {
    for (int i = 0; i < n; ++i) {
        std::cout << a[i] << " ";
    }
    std::cout << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    int a[] = {170, 45, 75, 90, 802, 24, 2, 66};
    int n = sizeof(a) / sizeof(a[0]);

    radixsort(a, n);
    
    // 预期输出：2 24 45 66 75 90 170 802
    print(a, n);

    return 0;
}
