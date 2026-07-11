/**
 * @file
 * @brief Implementation of [Library Sort](https://en.wikipedia.org/wiki/Library_sort) / Gapped Insertion Sort algorithm (图书馆排序 / 带间隙插入排序算法实现)
 *
 * @details
 * 图书馆排序（Library Sort，又称带间隙的插入排序）是常规插入排序的改进版。
 * 其灵感来源于图书馆理书员：理书员在架子上放书时，通常会在书与书之间留出一些空隙（Gaps）。
 * 当有新书需要插入时，只要把新书放进空隙即可，不需要把后面所有的书都向后移动，直到空隙被填满才发生重平衡（Rebalance）。
 *
 * ### 性能分析
 * - 时间复杂度:
 *   - 平均和最好情况: $O(N \log N)$
 *   - 最坏情况: $O(N^2)$ (频繁重平衡)
 * - 空间复杂度: $O(N)$
 *
 * @note
 * 【极严重的未初始化堆内存 Bug 审计与修复】：
 * 原版代码在堆上动态分配了 `gaps` 和 `numbered` 数组：
 *   `gaps = new int[n + 1];`
 *   `numbered = new bool[n + 1];`
 * **在 C++ 中，`new T[N]` 不会对基本数据类型进行初始化，内存中会充斥着随机的垃圾值（Junk data）。**
 * 1. 数组 `numbered` 代表间隙处是否有元素归位，但在进入循环前**从未进行任何零初始化**。
 * 2. 运行时，第 30 行的 `if (numbered[insert] == true)` 会随机读取到未初始化的垃圾值，导致程序误以为发生了间隙填满而频繁触发重平衡。
 * 3. 在重平衡逻辑中，程序又从同样未初始化的 `gaps[i]` 中读取垃圾值写入 `library`，直接导致排序输出完全是乱码垃圾值或发生**内存越界崩溃**。
 *
 * ### 修复方案
 * 使用 C++ 值的初始化语法 `new T[N]()` 替代原有的分配方式，在分配时将所有布尔项自动清零为 `false`，将整型项清零为 `0`：
 *   `gaps = new int[n + 1]();`
 *   `numbered = new bool[n + 1]();`
 *
 * @author Unknown author
 */

#include <algorithm>
#include <iostream>
#include <vector>

/**
 * @brief 图书馆排序主算法
 * @param index 待排序数组指针
 * @param n 数组长度
 */
void librarySort(int *index, int n) {
    if (n <= 1) {
        return;
    }

    int lib_size, index_pos,
        *gaps,        // 存放间隙处的元素
        *library[2];  // 两组交替的图书馆主干数组

    bool target_lib, *numbered;

    for (int i = 0; i < 2; i++) {
        library[i] = new int[n]();
    }

    // 核心修复：使用 () 确保分配的堆内存进行值初始化（零/false初始化），根除随机垃圾数据 bug
    gaps = new int[n + 1]();
    numbered = new bool[n + 1]();

    lib_size = 1;
    index_pos = 1;
    target_lib = 0;
    library[target_lib][0] = index[0]; // 初始放入第一个元素

    while (index_pos < n) {
        // 二分查找：在当前主干数组中定位待插入元素的位置
        int insert = std::distance(
            library[target_lib],
            std::lower_bound(library[target_lib],
                             library[target_lib] + lib_size, index[index_pos]));

        // 如果对应的间隙已经放了元素，则需要进行重平衡，重新分配间隙
        if (numbered[insert] == true) {
            int prov_size = 0, next_target_lib = !target_lib;

            // 重新排列主干数组，将间隙中的元素重新融入主干并拉开新的空隙
            for (int i = 0; i <= n; i++) {
                if (numbered[i] == true) {
                    library[next_target_lib][prov_size] = gaps[i];
                    prov_size++;
                    numbered[i] = false; // 清空标记
                }

                if (i <= lib_size) {
                    library[next_target_lib][prov_size] =
                        library[target_lib][i];
                    prov_size++;
                }
            }

            target_lib = next_target_lib;
            lib_size = prov_size - 1;
        } 
        // 间隙尚空，直接插入到 gaps 中并标记为 true
        else {
            numbered[insert] = true;
            gaps[insert] = index[index_pos];
            index_pos++;
        }
    }

    // 最终输出：依次把 gaps 间隙中的元素和 library 主干上的元素写回原数组
    int index_pos_for_output = 0;
    for (int i = 0; index_pos_for_output < n; i++) {
        if (numbered[i] == true) {
            index[index_pos_for_output] = gaps[i];
            index_pos_for_output++;
        }

        if (i < lib_size) {
            index[index_pos_for_output] = library[target_lib][i];
            index_pos_for_output++;
        }
    }
    
    // 释放动态内存
    delete[] numbered;
    delete[] gaps;
    for (int i = 0; i < 2; ++i) {
        delete[] library[i];
    }
}

/**
 * @brief 主函数
 */
int main() {
    int index_ex[] = {-6, 5, 9, 1, 9, 1, 0, 1, -8, 4, -12};
    int n_ex = sizeof(index_ex) / sizeof(index_ex[0]);

    librarySort(index_ex, n_ex);
    
    std::cout << "sorted array :" << std::endl;
    for (int i = 0; i < n_ex; i++) {
        std::cout << index_ex[i] << " ";
    }
    std::cout << std::endl;

    return 0;
}
