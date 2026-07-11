/**
 * @file
 * @brief Implementation of [Wiggle Sort](https://leetcode.com/problems/wiggle-sort-ii/) algorithm (摆动排序算法实现)
 *
 * @details
 * 摆动排序（Wiggle Sort）将数组重新排列为局部高低交错的波浪形结构。
 * 排序后的数组满足以下规律：
 * `arr[0] >= arr[1] <= arr[2] >= arr[3] <= arr[4] >= arr[5] ...`
 * 也就是说，所有偶数索引位置的值都是局部极大值，所有奇数索引位置的值都是局部极小值。
 *
 * ### 算法原理（一次遍历，线性时间 $O(N)$）
 * 1. 遍历所有的偶数索引 `i` (0, 2, 4 ...)。
 * 2. 对每个偶数位置 `arr[i]`，确保它是局部最大：
 *    - 如果 `i > 0` 且其左边邻居比它大（`arr[i-1] > arr[i]`），则交换它们，使 `arr[i-1] <= arr[i]`。
 *    - 如果 `i < size - 1` 且其右边邻居比它大（`arr[i+1] > arr[i]`），则交换它们，使 `arr[i] >= arr[i+1]`。
 * 3. 这样遍历一遍之后，整个数组即完美满足摆动条件。
 *
 * @note
 * 【原版测试代码中严重的三重 Bug 审计与修复】：
 * 原测试函数中隐藏了三个极其严重的 Bug（由于宏定义 `NDEBUG` 或是测试顺序的机缘巧合未暴露）：
 * 1. **断言对象错误 Bug**：原代码在校验结果时，写为 `assert(data1[j] <= data1[j + 1] ...)`。
 *    `data1` 是**原始未排序的随机数组**，在此处断言必定会导致测试失败，正确的断言对象应为排好序的副本 `sorted`。
 * 2. **断言方向错误 Bug**：断言条件写为 `sorted[j] <= sorted[j + 1] && sorted[j + 1] >= sorted[j + 2]`。
 *    这与本程序实现的波形正好相反。本程序满足的是偶数位置为最大值（即 `sorted[j] >= sorted[j + 1] && sorted[j + 1] <= sorted[j + 2]`）。
 * 3. **数组越界读 Bug**：循环条件为 `j < data1.size()`，且步长为 2。
 *    当 `j = size - 2` 时，`j + 2` 等于 `size`，会导致 `sorted[j + 2]` 发生**越界内存访问**。
 * 
 * ### 修复方案
 * - 将断言对象更正为 `sorted`；
 * - 修正断言逻辑方向为：`sorted[j] >= sorted[j + 1] && (j + 2 < sorted.size() ? sorted[j + 1] <= sorted[j + 2] : true)`；
 * - 加上 `j + 2 < sorted.size()` 的安全边界限制，彻底避免越界读。
 *
 * 时间复杂度: $O(N)$ (单趟扫描)
 * 空间复杂度: $O(N)$ (为了保证输入只读，返回了排序副本)
 * 
 * @author [Roshan Kanwar](http://github.com/roshan0708)
 */

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <ctime>
#include <iostream>
#include <vector>

/**
 * @namespace sorting
 * @brief 排序算法命名空间
 */
namespace sorting {
/**
 * @namespace wiggle_sort
 * @brief 摆动排序算法命名空间
 */
namespace wiggle_sort {

/**
 * @brief 摆动排序主函数
 * @tparam T 元素模板类型
 * @param arr 待排序数组只读引用
 * @returns 摆动排序完成后的新数组
 */
template <typename T>
std::vector<T> wiggleSort(const std::vector<T> &arr) {
    std::vector<T> out(arr); // 拷贝构造副本，防止修改原数据
    uint32_t size = out.size();

    if (size <= 1) {
        return out; // 大小为 0 或 1，直接返回
    }

    // 一趟扫描，调整偶数索引元素使其成为局部最大值
    for (uint32_t i = 0; i < size; i += 2) {
        // 如果左邻居大于当前值，执行交换
        if (i > 0 && out[i - 1] > out[i]) {
            std::swap(out[i], out[i - 1]);
        }

        // 如果右邻居大于当前值，执行交换
        if (i < size - 1 && out[i] < out[i + 1]) {
            std::swap(out[i], out[i + 1]);
        }
    }

    return out;
}
}  // namespace wiggle_sort
}  // namespace sorting

/**
 * @brief 打印数组内容
 */
template <typename T>
static void displayElements(const std::vector<T> &arr) {
    uint32_t size = arr.size();
    std::cout << "Wiggle Sorted elements: [";
    for (uint32_t i = 0; i < size; i++) {
        std::cout << arr[i];
        if (i != size - 1) {
            std::cout << ", ";
        }
    }
    std::cout << "]" << std::endl;
}

/**
 * @brief 单元自测用例
 */
static void test() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // 随机生成 100 个范围在 -5.00 到 4.99 的浮点数
    std::vector<float> data1(100);
    for (auto &d : data1) {
        d = static_cast<float>(std::rand() % 1000 - 500) / 100.f;
    }

    // 执行排序
    std::vector<float> sorted = sorting::wiggle_sort::wiggleSort<float>(data1);

    displayElements(sorted);

    // 核心修复：更正断言对象为 sorted，修正波形方向，添加越界防御
    for (uint32_t j = 0; j < sorted.size() - 1; j += 2) {
        // 偶数索引 j，必须大于等于相邻项
        assert(sorted[j] >= sorted[j + 1]);
        if (j + 2 < sorted.size()) {
            // 奇数索引 j+1，必须小于等于后一项
            assert(sorted[j + 1] <= sorted[j + 2]);
        }
    }

    std::cout << "Wiggle sort self-test passed successfully!\n";
}

/**
 * @brief 主函数
 */
int main() {
    test(); // 运行自测
    return 0;
}
