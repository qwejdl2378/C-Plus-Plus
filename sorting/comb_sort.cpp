/**
 * @file
 * @brief Implementation of [Comb Sort](https://en.wikipedia.org/wiki/Comb_sort) algorithm (梳排序算法实现)
 *
 * @details
 * 梳排序（Comb Sort）是冒泡排序的一种改良版本。
 * 冒泡排序中，排在较后位置的小元素（被称为“乌龟” turtles）移动到前面的速度非常慢（每趟循环只能向前移动一个位置）。
 * 梳排序通过引入一个**大于 1 的步长（Gap）**，使得乌龟节点可以成跨度地向前跳跃，从而极大地提高了排序效率。
 * 
 * ### 步长衰减（Shrink Factor）
 * - 每次遍历后，步长都会除以收缩因子 1.3，即：`gap = gap / 1.3`（代码中以整数乘法实现 `(gap * 10) / 13`）。
 * - 1.3 是通过大量实验数据得出的最优收缩因子。
 * - 最终，当步长衰减到 1 时，梳排序退化为带交换标记的冒泡排序，以确保数组完全有序。
 *
 * 时间复杂度:
 *   - 最好情况: $O(N \log N)$
 *   - 最坏情况: $O(N^2)$
 *   - 平均情况: $O(N^2 / 2^p)$ (通常接近于 $O(N \log N)$)
 * 空间复杂度: $O(1)$
 *
 * @author Unknown author
 */

#include <algorithm>
#include <cassert>
#include <iostream>

/**
 * @brief 通过除以收缩因子 1.3 来计算下一个步长的值
 * @param gap 当前的步长
 * @returns 衰减后的下一个步长值（最小为 1）
 */
int FindNextGap(int gap) {
    gap = (gap * 10) / 13; // 使用整数运算规避浮点精度问题
    return std::max(1, gap);
}

/**
 * @brief 梳排序算法
 * @param arr 待排序数组指针
 * @param l 排序区间起始索引（包含，通常为 0）
 * @param r 排序区间结束索引（不包含，通常为数组长度 n）
 */
void CombSort(int *arr, int l, int r) {
    // 初始步长设为区间的最大跨度
    int gap = r;

    // 是否发生交换的标记
    bool swapped = true;

    // 当步长不为 1，或者仍有元素被交换时，继续循环
    while (gap != 1 || swapped) {
        // 计算新一步的缩减步长
        gap = FindNextGap(gap);

        swapped = false; // 初始设为无交换

        // 在当前步长跨度下遍历比较并交换逆序对
        for (int i = l; i < r - gap; ++i) {
            if (arr[i] > arr[i + gap]) {
                std::swap(arr[i], arr[i + gap]);
                swapped = true; // 发生交换
            }
        }
    }
}

/**
 * @brief 单元自测用例
 */
void tests() {
    // 测试 1
    int arr1[10] = {34, 56, 6, 23, 76, 34, 76, 343, 4, 76};
    CombSort(arr1, 0, 10);
    assert(std::is_sorted(arr1, arr1 + 10));
    std::cout << "Test 1 passed\n";

    // 测试 2: 负数与重复数据
    int arr2[8] = {-6, 56, -45, 56, 0, -1, 8, 8};
    CombSort(arr2, 0, 8);
    assert(std::is_sorted(arr2, arr2 + 8));
    std::cout << "Test 2 Passed\n";
}

/**
 * @brief 主函数
 */
int main() {
    tests(); // 运行单元测试

    int n = 0;
    std::cout << "Enter number of elements for custom sort: ";
    std::cin >> n;
    if (n <= 0) {
        return 0;
    }
    
    int *arr = new int[n];
    std::cout << "Enter the elements:" << std::endl;
    for (int i = 0; i < n; ++i) {
        std::cin >> arr[i];
    }
    
    CombSort(arr, 0, n);
    
    std::cout << "Sorted array: ";
    for (int i = 0; i < n; ++i) {
        std::cout << arr[i] << ' ';
    }
    std::cout << std::endl;
    
    delete[] arr;
    return 0;
}
