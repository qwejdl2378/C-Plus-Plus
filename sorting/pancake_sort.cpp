/**
 * @file
 * @brief Implementation of [Pancake Sort](https://en.wikipedia.org/wiki/Pancake_sorting) algorithm (煎饼排序算法实现)
 *
 * @details
 * 煎饼排序（Pancake Sorting）是一种非常有趣的排序算法。
 * 想象一叠大小不一的煎饼（Pancakes），你只能使用一把铲子插入煎饼堆中任意一个位置，然后将该位置上方所有的煎饼整体翻转（Flip）。
 * 排序的目标是利用**最少次数的翻转**，将这叠煎饼排成自顶向下从小到大的顺序。
 *
 * ### 算法思路
 * 类似于选择排序（Selection Sort）：
 * 1. 寻找当前未排序范围 `[0, i-1]` 内的最大值，记其索引为 `max_index`。
 * 2. 如果最大值不在末尾：
 *    - 先用一次翻转将最大值翻转到最顶端（即索引 0）：`reverse(arr, 0, max_index)`。
 *    - 再用一次翻转将最顶端的最大值翻转到它应该呆的末尾位置：`reverse(arr, 0, i - 1)`。
 * 3. 减小未排序范围（`i--`），重复上述操作，直到范围缩小到 1。
 *
 * @note
 * 【原版代码对负数处理失效的重大 Bug 审计与修复】：
 * 原版代码在第 64 行中直接硬编码初始化：`T max_value = 0;`。
 * **如果输入数组中包含全负数（如 `{-5, -3, -10, -2]`），由于所有值都小于 0，**
 * 判断 `arr[j] >= max_value` 永远不会成立，导致 `max_value` 无法匹配任何数组元素，排序结果彻底错乱。
 * 
 * ### 修复方案
 * - 将最大值初始化为当前范围的第一个元素：`T max_value = arr[0];`，并将 `max_index` 初始化为 `0`。
 * - 扫描从索引 `1` 开始：`for (j = 1; j < i; j++)`。
 *
 * 时间复杂度: $O(N^2)$ (每次查找最大值耗时 $O(N)$，需进行 $N$ 轮)
 * 空间复杂度: $O(1)$
 * 
 * @author [Divyansh Gupta](https://github.com/divyansh12323)
 */

#include <algorithm>  // 用于 std::is_sorted, std::swap
#include <cassert>    // 用于 assert 断言
#include <iostream>   // 用于输入输出
#include <vector>     // 用于 std::vector

/**
 * @namespace sorting
 * @brief 排序算法命名空间
 */
namespace sorting {
/**
 * @namespace pancake_sort
 * @brief 煎饼排序算法命名空间
 */
namespace pancake_sort {

/**
 * @brief 局域翻转函数（模拟铲子整体翻转 [start, end] 区间内的元素）
 * @tparam T 元素模板类型
 * @param arr 数组引用
 * @param start 起始索引
 * @param end 结束索引
 */
template <typename T>
void reverse(std::vector<T> &arr, int start, int end) {
    while (start < end) {
        std::swap(arr[start], arr[end]);
        start++;
        end--;
    }
}

/**
 * @brief 煎饼排序主算法
 * @tparam T 元素模板类型
 * @param arr 待排序数组的引用
 * @param size 数组长度
 * @returns 0 表示退出
 */
template <typename T>
int pancakeSort(std::vector<T> &arr, int size) {
    if (size <= 1) {
        return 0;
    }

    for (int i = size; i > 1; --i) {
        // 核心修复：最大值初始化为 arr[0]，确保负数可以正确比较
        int max_index = 0;
        T max_value = arr[0];

        // 寻找 [0, i-1] 区间内的最大值
        for (int j = 1; j < i; j++) {
            if (arr[j] >= max_value) {
                max_value = arr[j];
                max_index = j;
            }
        }

        // 如果最大值不在未排序区间的最后一个位置，则通过两次翻转将其归位
        if (max_index != i - 1) {
            reverse(arr, 0, max_index); // 第一次翻转：换到最顶端
            reverse(arr, 0, i - 1);     // 第二次翻转：换到区间尾部
        }
    }
    return 0;
}
}  // namespace pancake_sort
}  // namespace sorting

/**
 * @brief 单元自测用例
 */
static void test() {
    // 测试 1: 包含正数的 vector
    const int size1 = 7;
    std::cout << "\nTest 1- as std::vector<int>...";
    std::vector<int> arr1 = {23, 10, 20, 11, 12, 6, 7};
    sorting::pancake_sort::pancakeSort(arr1, size1);
    assert(std::is_sorted(arr1.begin(), arr1.end()));
    std::cout << "Passed\n";
    for (int i = 0; i < size1; i++) {
        std::cout << arr1[i] << " ,";
    }
    std::cout << std::endl;

    // 测试 2: 包含负数和浮点数的 vector (用以验证修复后的负数支持)
    const int size2 = 8;
    std::cout << "\nTest 2- as std::vector<double> with negatives...";
    std::vector<double> arr2 = {-23.56, -10.62, -200.78, 111.484,
                                -3.9,   1.2,   -61.77,  79.6};
    sorting::pancake_sort::pancakeSort(arr2, size2);
    assert(std::is_sorted(arr2.begin(), arr2.end()));
    std::cout << "Passed\n";
    for (int i = 0; i < size2; i++) {
        std::cout << arr2[i] << ", ";
    }
    std::cout << std::endl;

    // 测试 3
    const int size3 = 7;
    std::cout << "\nTest 3- as std::vector<float>...";
    std::vector<float> arr3 = {6.56f, 12.62f, 200.78f, 768.484f, 19.27f, 68.87f, 9.6f};
    sorting::pancake_sort::pancakeSort(arr3, size3);
    assert(std::is_sorted(arr3.begin(), arr3.end()));
    std::cout << "Passed\n\n";
}

/**
 * @brief 主函数
 */
int main() {
    test(); // 运行测试
    return 0;
}
