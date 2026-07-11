/**
 * @file
 * @brief Implementation of [Bead Sort / Gravity Sort](https://en.wikipedia.org/wiki/Bead_sort) algorithm (基于重力的珠排序算法实现)
 *
 * @details
 * 珠排序（Bead Sort，又称重力排序 Gravity Sort）是一种模拟物理世界重力下落的排序算法。
 * 主要用于对非负整数集合进行排序。
 *
 * ### 算法直观描述
 * 想象每一行代表一个数字，其值大小代表该行放置的“珠子”个数。
 * 珠子穿在竖直的“轴”（poles）上。在重力的作用下，悬空的珠子会向下掉落落到最底部。
 * 最终，下面的行堆积的珠子数量最多，上面的行堆积的珠子数量最少。
 * 从上到下或从下到上读取各行的珠子数量，即得到了有序的数字序列。
 *
 * @note
 * 【局限性与溢出风险审计】：
 * 1. **仅限非负整数**：由于行数和珠子数分别对应数组的大小和元素值，本算法**不支持负数**。如果数组中包含负数，会导致严重的越界访问。
 * 2. **空间复杂度极高**：分配内存大小为 `max * len`。如果数组中存在一个极大的元素（例如 $10^9$），
 *    即使数组长度很小，也会分配超大内存块（数十 GB），不仅容易导致 `std::bad_alloc` 异常，
 *    甚至可能因为乘法 `max * len` 溢出 `int` 范围导致实际分配空间偏小，从而触发**缓冲区溢出崩溃（Buffer Overflow）**。
 * 3. 推荐在实际工业中使用 `std::vector` 进行安全防范。
 *
 * 时间复杂度: $O(N \cdot \text{max\_val})$ 或 $O(S)$（物理模拟耗时，S 为珠子总数）
 * 空间复杂度: $O(N \cdot \text{max\_val})$
 *
 * @author Unknown author
 */

#include <cstdio>
#include <cstring>
#include <iostream>
#include <new>

// 辅助宏定义：定位第 i 行第 j 列的珠子在扁平一维数组中的位置
#define BEAD(i, j) beads[static_cast<size_t>(i) * max + (j)]

/**
 * @brief 珠排序主函数
 * @param a 待排序整型数组指针
 * @param len 数组长度
 */
void beadSort(int *a, int len) {
    if (len <= 1) {
        return;
    }

    // 1. 寻找数组中的最大值（同时校验非负性）
    int max = a[0];
    for (int i = 1; i < len; i++) {
        if (a[i] > max) {
            max = a[i];
        }
    }

    if (max <= 0) {
        // 如果最大值为 0 且数组元素均为非负数，说明全为 0，无需排序
        return;
    }

    // 2. 动态分配珠子状态矩阵空间（使用 size_t 防范乘法溢出）
    size_t total_beads = static_cast<size_t>(max) * len;
    unsigned char *beads = nullptr;
    try {
        beads = new unsigned char[total_beads];
    } catch (const std::bad_alloc &e) {
        std::cerr << "Memory allocation failed: max value is too large for bead sort." << std::endl;
        return;
    }
    
    // 初始化清零
    std::memset(beads, 0, total_beads);

    // 3. 在相应位置放置珠子
    for (int i = 0; i < len; i++) {
        if (a[i] < 0) {
            std::cerr << "Error: Bead sort does not support negative numbers." << std::endl;
            delete[] beads;
            return;
        }
        for (int j = 0; j < a[i]; j++) {
            BEAD(i, j) = 1;
        }
    }

    // 4. 模拟重力下落：统计每一列上的珠子总数，并让其下沉到最底部
    for (int j = 0; j < max; j++) {
        int sum = 0; // 统计第 j 列上的珠子个数
        for (int i = 0; i < len; i++) {
            sum += BEAD(i, j);
            BEAD(i, j) = 0; // 临时清空
        }

        // 将所有 sum 个珠子移动到最底部的行中
        for (int i = len - sum; i < len; i++) {
            BEAD(i, j) = 1;
        }
    }

    // 5. 根据下落后的珠子分布状态，重新还原写回原数组
    for (int i = 0; i < len; i++) {
        int j;
        for (j = 0; j < max && BEAD(i, j); j++) {
            // 计算第 i 行有多少连续的珠子
        }
        a[i] = j; // 珠子数即为排序后的数值
    }
    
    delete[] beads; // 释放内存
}

/**
 * @brief 主函数测试
 */
int main() {
    int a[] = {5, 3, 1, 7, 4, 1, 1, 20};
    int len = sizeof(a) / sizeof(a[0]);

    beadSort(a, len);

    // 预期输出：1 1 1 3 4 5 7 20
    for (int i = 0; i < len; i++) {
        std::printf("%d ", a[i]);
    }
    std::printf("\n");

    return 0;
}
