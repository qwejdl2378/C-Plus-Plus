/**
 * @file
 * @brief Implementation of [Bitonic Sort](https://en.wikipedia.org/wiki/Bitonic_sorter) algorithm (双调排序算法实现)
 *
 * @details
 * 双调排序（Bitonic Sort）是一种基于比较的并行排序算法。
 * 它是构建排序网络（Sorting Networks）的重要算法之一，非常适合多处理器并行硬件（如 GPU）的硬件实现。
 *
 * ### 核心概念
 * 1. **双调序列（Bitonic Sequence）**：一个先单调递增、后单调递减的序列（或者可通过循环移位达到此状态的序列）。
 * 2. **双调合并（Bitonic Merge）**：对于一个双调序列，如果我们把前半部分和后半部分对应的元素进行比较并交换（按方向 `dir`），
 *    就能得到两个新的双调序列，且其中一个序列的所有元素均不小于另一个序列。递归这一过程，最终可使序列完全有序。
 * 3. **算法构建**：双调排序通过自底向上递归，将相邻的两个子数组分别排成递增和递减顺序，
 *    从而拼接成一个双调序列，然后再调用 `bitonicMerge` 进行合并。
 *
 * @note
 * 【输入大小限制警示】：
 * 经典的双调排序网络结构要求**输入数组的长度必须是 2 的整数次幂**（例如 2, 4, 8, 16 等）。
 * 如果输入数组的长度不是 2 的幂，算法在拆分阶段（如 `cnt / 2` 无法对齐）将无法保证结果的正确性，甚至导致数组越界。
 *
 * 时间复杂度: $O(N \log^2 N)$（单线程串行执行时间，如果完全并行化，可达到 $O(\log^2 N)$）
 * 空间复杂度: $O(\log^2 N)$（递归调用栈深度）
 * 
 * @author Unknown author
 */

#include <algorithm>
#include <iostream>

/**
 * @brief 比较并交换两个元素
 * @param a 数组指针
 * @param i 第一个元素索引
 * @param j 第二个元素索引
 * @param dir 排序方向（1 为升序，0 为降序）
 */
void compAndSwap(int a[], int i, int j, int dir) {
    // 如果排序方向与当前大小顺序不一致，则交换两者
    if (dir == (a[i] > a[j])) {
        std::swap(a[i], a[j]);
    }
}

/**
 * @brief 递归合并双调序列
 * @param a 数组指针
 * @param low 起始索引
 * @param cnt 合并元素个数
 * @param dir 排序方向（1 为升序，0 为降序）
 */
void bitonicMerge(int a[], int low, int cnt, int dir) {
    if (cnt > 1) {
        int k = cnt / 2; // 折半步长
        // 比较间隔为 k 的成对元素，使较大或较小的值沉底
        for (int i = low; i < low + k; i++) {
            compAndSwap(a, i, i + k, dir);
        }
        // 递归合并两个子序列
        bitonicMerge(a, low, k, dir);
        bitonicMerge(a, low + k, k, dir);
    }
}

/**
 * @brief 递归生成双调序列并合并
 * @param a 数组指针
 * @param low 起始索引
 * @param cnt 排序元素个数
 * @param dir 排序方向（1 为升序，0 为降序）
 */
void bitonicSort(int a[], int low, int cnt, int dir) {
    if (cnt > 1) {
        int k = cnt / 2;

        // 1. 将前半部分递归排成升序 (dir = 1)
        bitonicSort(a, low, k, 1);

        // 2. 将后半部分递归排成降序 (dir = 0)
        // 升降序拼接后，整个区间 low 到 low+cnt 形成了一个标准的双调序列
        bitonicSort(a, low + k, k, 0);

        // 3. 对该双调序列执行合并以达成整体 dir 方向的排序
        bitonicMerge(a, low, cnt, dir);
    }
}

/**
 * @brief 排序主入口包装函数
 * @param a 待排序数组
 * @param N 数组长度（必须是 2 的幂）
 * @param up 排序方向（1 为升序，0 为降序）
 */
void sort(int a[], int N, int up) { 
    bitonicSort(a, 0, N, up); 
}

/**
 * @brief 主函数
 */
int main() {
    int a[] = {3, 7, 4, 8, 6, 2, 1, 5};
    int N = sizeof(a) / sizeof(a[0]);

    int up = 1;  // 1 代表升序排序
    sort(a, N, up);

    std::cout << "Sorted array: \n";
    for (int i = 0; i < N; i++) {
        std::cout << a[i] << " ";
    }
    std::cout << std::endl;
    return 0;
}
