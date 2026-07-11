/**
 * @file
 * @brief Implementation of [Pigeonhole Sort](https://en.wikipedia.org/wiki/Pigeonhole_sort) algorithm (鸽巢排序 / 套面排序算法实现)
 *
 * @details
 * 鸽巢排序（Pigeonhole Sort）是一种非比较型的分配式排序算法。
 * 适用于元素个数 $N$ 和元素值域范围（Range = Max - Min + 1）大约相等的整数数组排序。
 *
 * ### 算法思想
 * 1. 寻找数组的最小值 `min` 和最大值 `max`。
 * 2. 准备 `Range = max - min + 1` 个“鸽巢”（Pigeonholes）。每个鸽巢可以容纳多个相同值的元素（通常使用 vector 容器实现）。
 * 3. 遍历原数组，将每个元素 `arr[i]` 放入对应的鸽巢 `hole[arr[i] - min]` 中。
 * 4. 顺序遍历所有鸽巢，将里面的元素依次收集回原数组，即完成了排序。
 *
 * @note
 * 【原版代码的严重正确性覆盖 Bug 审计与修复】：
 * 原版代码存在严重的算法实现缺陷，导致**含有重复值时排序失效且数据丢失**：
 * 1. 覆盖覆盖：原版将鸽巢声明为普通整型数组 `int *hole = new int[range]();`，在填充时直接用 `hole[arr[i] - *min] = arr[i];` 进行覆盖。
 *    若数组中存在重复的元素（如 `{8, 8, 2, 7, 4}` 中有两个 8），后面的 8 会直接覆盖前面的 8，只保留一个。
 * 2. 数据丢失：因为没有统计重复项，写回时只填充了唯一值，导致原数组尾部元素未被修改（保留了原本垃圾数据），且丢失了重复的元素。
 *    测试用例 1 `test_1` 之所以能过，只是因为原测试数组恰好把重复值 8 放到了最后一位，纯属巧合！如果把重复的 8 移到前面，测试必然失败。
 * 
 * ### 修复方案
 * - 将鸽巢的数据结构重构为二阶容器 `std::vector<std::vector<int>>`。
 * - 填充时，使用 `push_back` 将元素放入对应的鸽巢：`hole[arr[i] - *min].push_back(arr[i]);`。
 * - 顺序收集时，依次取出各鸽巢中的所有元素，确保重复值能够完全且稳定地排好序。
 *
 * 时间复杂度: $O(N + \text{Range})$
 * 空间复杂度: $O(N + \text{Range})$
 * 
 * @author [Lownish](https://github.com/Lownish)
 */

#include <algorithm>  // 用于 std::is_sorted, std::min_element, std::max_element
#include <array>      // 用于 std::array
#include <cassert>    // 用于 assert 断言
#include <iostream>   // 用于标准输出
#include <vector>     // 用于 std::vector

/**
 * @namespace sorting
 * @brief 排序算法命名空间
 */
namespace sorting {

/**
 * @brief 鸽巢排序主算法
 * @tparam N 容器大小
 * @param arr 待排序的 std::array 容器
 * @returns 排序好（升序）的新 std::array 容器
 */
template <std::size_t N>
std::array<int, N> pigeonSort(std::array<int, N> arr) {
    if (N <= 1) {
        return arr;
    }

    // 寻找区间最小值与最大值
    auto min = std::min_element(arr.begin(), arr.end());
    auto max = std::max_element(arr.begin(), arr.end());

    int range = *max - *min + 1;
    
    // 核心修复：将一维整型桶重构为二维 vector 容器，以防范同值元素覆盖丢失 Bug
    std::vector<std::vector<int>> hole(range);

    // 将所有值填充到其对应的鸽巢中
    for (size_t i = 0; i < N; i++) {
        hole[arr[i] - *min].push_back(arr[i]);
    }

    // 依次从鸽巢中收集回原数组
    size_t count = 0;
    for (int i = 0; i < range; i++) {
        for (int val : hole[i]) {
            arr[count++] = val;
        }
    }

    return arr;
}
}  // namespace sorting

/**
 * @brief 单元自测用例 1 (包含重复元素)
 */
static void test_1() {
    const int n = 7;
    std::array<int, n> test_array = {8, 3, 2, 7, 4, 6, 8};

    test_array = sorting::pigeonSort<n>(test_array);

    assert(std::is_sorted(std::begin(test_array), std::end(test_array)));

    std::cout << "Test 1: ";
    for (int i = 0; i < n; i++) {
        std::cout << test_array.at(i) << " ";
    }
    std::cout << "\nPassed\n";
}

/**
 * @brief 单元自测用例 2 (大数范围)
 */
static void test_2() {
    const int n = 10;
    std::array<int, n> test_array = {802, 630, 20,  745, 52,
                                     300, 612, 932, 78,  187};

    test_array = sorting::pigeonSort<n>(test_array);

    assert(std::is_sorted(std::begin(test_array), std::end(test_array)));

    std::cout << "Test 2: ";
    for (int i = 0; i < n; i++) {
        std::cout << test_array.at(i) << " ";
    }
    std::cout << "\nPassed\n";
}

/**
 * @brief 单元自测用例 3: 验证重复值在前部的修复性用例
 */
static void test_3() {
    const int n = 5;
    std::array<int, n> test_array = {8, 8, 2, 7, 4}; // 重复的 8 在前面

    test_array = sorting::pigeonSort<n>(test_array);

    assert(std::is_sorted(std::begin(test_array), std::end(test_array)));

    std::cout << "Test 3: ";
    for (int i = 0; i < n; i++) {
        std::cout << test_array.at(i) << " ";
    }
    std::cout << "\nPassed\n";
}

/**
 * @brief 主函数
 */
int main() {
    test_1();
    test_2();
    test_3();

    return 0;
}
