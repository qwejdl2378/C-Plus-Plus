/**
 * @file
 * @brief Implementation of [Shell Sort](https://en.wikipedia.org/wiki/Shellsort) algorithm (希尔排序算法实现)
 *
 * @details
 * 希尔排序（Shell Sort，又称缩小增量排序）是插入排序的一种更高效的改进版本。
 *
 * ### 算法思想
 * 1. 它是非稳定排序算法。
 * 2. 传统插入排序每次只能将元素移动一位。如果一个极小值在数组末尾，则需要移动 $O(N)$ 次。
 * 3. 希尔排序通过引入一个“增量”（Gap，本实现初始增量设为 `size / 2`）将原数组分组。
 *    对每个分组内部进行直接插入排序。
 * 4. 随后，增量逐渐减半（`i = i / 2`），直到增量减为 1。此时数组已基本有序，最后一趟进行增量为 1 的插入排序即可极其快速地完成排序。
 *
 * @note
 * 【原版代码的严重堆内存缓冲区溢出 Bug 审计与修复】：
 * 原版代码执行顺序为：
 *   `int size = 10;`
 *   `int* array = new int[size];` // 先在堆上分配了固定长度 10 的数组
 *   `std::cin >> size;` // 随后才从标准输入读取用户期望输入的实际大小 size
 * **如果用户输入的实际大小 `size` 大于 10（例如输入 15），**
 * 代码会在后续的输入循环 `for (int i = 0; i < size; i++) std::cin >> array[i]` 中，
 * 直接往只有 10 个空间的堆内存中写入 15 个数据，触发**堆缓冲区溢出（Heap Buffer Overflow）**，
 * 从而导致内存数据被污染、段错误（Segmentation Fault）或者程序崩溃。
 * 
 * ### 修复方案
 * - 调整执行流程：先从键盘读取用户输入的大小 `size`。
 * - 验证大小的有效性（必须大于 0）。
 * - 然后再使用 `new int[size]` 在堆上动态分配准确大小的内存空间，彻底根除缓冲区溢出隐患。
 *
 * 时间复杂度: 依赖于增量序列的选择。本实现使用 Shell 原始序列：
 *   - 最坏情况: $O(N^2)$
 *   - 最好情况: $O(N \log N)$
 *   - 平均情况: $O(N^{1.3})$ 至 $O(N^2)$ 之间
 * 空间复杂度: $O(1)$
 * 
 * @author Unknown author
 */

#include <iostream>

/**
 * @brief 主函数
 */
int main() {
    int size = 0;

    // 核心修复：先获取用户期望输入的实际数组长度
    std::cout << "How many numbers do you want to enter in unsorted array: ";
    std::cin >> size;
    
    // 安全检查，防止非法长度导致分配失败
    if (size <= 0) {
        std::cerr << "Invalid array size." << std::endl;
        return 0;
    }

    // 核心修复：在读取 size 之后再动态分配堆空间，根除堆溢出崩溃
    int* array = new int[size];

    std::cout << "Enter the numbers for unsorted array:" << std::endl;
    for (int i = 0; i < size; i++) {
        std::cin >> array[i];
    }

    // 希尔排序主流程
    // 外层循环：控制增量（Gap）的变化，逐步折半
    for (int i = size / 2; i > 0; i = i / 2) {
        // 中层循环：从索引等于增量处开始往后扫描元素
        for (int j = i; j < size; j++) {
            // 内层循环：组内进行直接插入排序
            for (int k = j - i; k >= 0; k = k - i) {
                if (array[k] < array[k + i]) {
                    break; // 顺序正确，不需要交换，直接跳出组内循环
                } else {
                    // 发现逆序，执行交换
                    int temp = array[k + i];
                    array[k + i] = array[k];
                    array[k] = temp;
                }
            }
        }
    }

    // 输出排序后的数组
    std::cout << "Sorted array: ";
    for (int i = 0; i < size; ++i) {
        std::cout << array[i] << " ";
    }
    std::cout << std::endl;

    delete[] array; // 释放堆内存
    return 0;
}
