/**
 * @file
 * @brief Implementation of [Cocktail Selection Sort](https://www.geeksforgeeks.org/cocktail-selection-sort/) algorithm (鸡尾酒选择排序算法实现)
 *
 * @details
 * 鸡尾酒选择排序（Cocktail Selection Sort）是选择排序的一种双向变体。
 * 传统的选择排序在每一趟扫描中仅定位最小值并将其放到合适的位置。
 * 而鸡尾酒选择排序在单趟扫描中**同时定位最小值和最大值**，
 * 并分别将它们与当前区间的左边界 `low` 和右边界 `high` 进行交换，从而将单趟效率提升一倍。
 *
 * @note
 * 【原版代码的严重正确性交换 Bug 审计】：
 * 在原本的双向交换逻辑中，如果最大值恰好位于左边界 `low`（即 `maximumindex == low`），
 * 那么在首先交换最小值 `std::swap(vec[low], vec[minimumindex])` 后，
 * 原本位于 `low` 处的最大值已经被挪动到了 `minimumindex` 处！
 * 此时直接执行第二次交换 `std::swap(vec[high], vec[maximumindex])`，实际上会将已经放好在左边界 `low` 处的最小值，
 * 重新交换到右边界 `high`，从而将已排好的顺序彻底打乱！
 * 
 * ### 修复方案
 * 在完成第一次交换（最小值换到 `low`）后，如果最大值的原索引 `maximumindex` 等于 `low`，
 * 必须将 `maximumindex` 更新为 `minimumindex`（因为最大值已经被换到该位置了），然后再执行第二次交换。
 * 另外，原版递归版本 `CocktailSelectionSort_v2` 在尾部错误地调用了迭代版本 `CocktailSelectionSort`，现已修正为正确的递归自调用。
 *
 * 时间复杂度: $O(N^2)$
 * 空间复杂度: $O(1)$（迭代）/ $O(N)$（递归）
 * 
 * @author Unknown author
 */

#include <algorithm>
#include <iostream>
#include <vector>
#include <cassert>

/**
 * @brief 迭代版双向选择排序
 * @param vec 待排序的数组指针
 * @param low 排序区间左边界 (0-indexed)
 * @param high 排序区间右边界 (0-indexed)
 */
void CocktailSelectionSort(std::vector<int> *vec, int low, int high) {
    while (low < high) {
        int minimum = (*vec)[low];
        int minimumindex = low;
        int maximum = (*vec)[high];
        int maximumindex = high;

        // 单趟扫描：同时寻找 [low, high] 区间的最大值与最小值
        for (int i = low; i <= high; i++) {
            if ((*vec)[i] > maximum) {
                maximum = (*vec)[i];
                maximumindex = i;
            }
            if ((*vec)[i] < minimum) {
                minimum = (*vec)[i];
                minimumindex = i;
            }
        }

        // 核心修复：防止两次交换时位置交叉覆盖
        std::swap((*vec)[low], (*vec)[minimumindex]);
        if (maximumindex == low) {
            maximumindex = minimumindex; // 最大值已被交换到 minimumindex 位置
        }
        std::swap((*vec)[high], (*vec)[maximumindex]);

        low++;
        high--;
    }
}

/**
 * @brief 递归版双向选择排序
 * @param vec 待排序的数组指针
 * @param low 排序区间左边界 (0-indexed)
 * @param high 排序区间右边界 (0-indexed)
 */
void CocktailSelectionSort_v2(std::vector<int> *vec, int low, int high) {
    if (low >= high) {
        return;
    }

    int minimum = (*vec)[low];
    int minimumindex = low;
    int maximum = (*vec)[high];
    int maximumindex = high;

    for (int i = low; i <= high; i++) {
        if ((*vec)[i] > maximum) {
            maximum = (*vec)[i];
            maximumindex = i;
        }
        if ((*vec)[i] < minimum) {
            minimum = (*vec)[i];
            minimumindex = i;
        }
    }

    // 核心修复：应用与迭代版相同的防覆盖交换保护
    std::swap((*vec)[low], (*vec)[minimumindex]);
    if (maximumindex == low) {
        maximumindex = minimumindex;
    }
    std::swap((*vec)[high], (*vec)[maximumindex]);

    // 修正：正确的递归调用自身 v2
    CocktailSelectionSort_v2(vec, low + 1, high - 1);
}

/**
 * @brief 单元自测用例（包含了触发原版 Bug 的测试数组 [3, 1, 2]）
 */
static void tests() {
    std::vector<int> data1 = {3, 1, 2};
    std::vector<int> data2 = {3, 1, 2};

    CocktailSelectionSort(&data1, 0, 2);
    CocktailSelectionSort_v2(&data2, 0, 2);

    assert(std::is_sorted(data1.begin(), data1.end()));
    assert(std::is_sorted(data2.begin(), data2.end()));
    std::cout << "Self-tests passed!" << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    tests(); // 运行单元测试
    
    int n = 0;
    std::cout << "Enter number of elements\n";
    std::cin >> n;
    if (n <= 0) {
        return 0;
    }
    
    std::vector<int> v(n);
    std::cout << "Enter all the elements\n";
    for (int i = 0; i < n; ++i) {
        std::cin >> v[i];
    }

    int method = 0;
    std::cout << "Enter method: \n\t0: iterative\n\t1: recursive:\t";
    std::cin >> method;

    if (method == 0) {
        CocktailSelectionSort(&v, 0, n - 1);
    } else if (method == 1) {
        CocktailSelectionSort_v2(&v, 0, n - 1);
    } else {
        std::cerr << "Unknown method" << std::endl;
        return -1;
    }

    std::cout << "Sorted elements are\n";
    for (int i = 0; i < n; ++i) {
        std::cout << v[i] << " ";
    }
    std::cout << std::endl;

    return 0;
}
