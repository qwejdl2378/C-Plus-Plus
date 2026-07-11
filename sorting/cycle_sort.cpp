/**
 * @file
 * @brief Implementation of [Cycle Sort](https://en.wikipedia.org/wiki/Cycle_sort) algorithm (圈排序 / 循环排序算法实现)
 *
 * @details
 * 圈排序（Cycle Sort）是一种就地（In-place）的非稳定排序算法。
 * 其最独特的特点在于：它是**理论上使内存写入（Writes）次数最少**的排序算法。
 * 每一个元素被写入内存的次数最多为 1 次或 2 次。因此，在内存写入代价极高（如非易失性闪存 Flash）的硬件设备上，该算法非常有用。
 *
 * ### 算法思想
 * 1. 算法将排序过程看作是将图拆分成若干个互不相交的“置换圈（Cycles）”。
 * 2. 遍历每个环的起点 `cycle_start`，将起点元素记为 `item`。
 * 3. 统计在 `cycle_start` 右侧有多少个元素比 `item` 小，设为 `pos`。`pos` 就是 `item` 在排序后应当放置的绝对正确位置。
 * 4. 如果该位置已有相同值，向右跳过重复元素。然后将 `item` 与 `arr[pos]` 交换。
 * 5. 现在新的 `item`（换出来的原 `arr[pos]`）需要继续寻找它正确的放置位置，重复该查找和交换过程，直到回到起点 `cycle_start`，整个“圈”闭合。
 *
 * @note
 * 【空数组下溢越界 Bug 审计与修复】：
 * 原版代码写为 `cycle_start <= arr.size() - 1;`：
 * 如果输入的数组为空（`arr.size() == 0`），无符号数减一 `0 - 1` 会产生**无符号整数下溢（Unsigned Underflow）**，
 * 变成最大的 `size_t` 值。导致循环错误地启动并访问 `arr[0]`，从而触发 **段错误崩溃（Segmentation Fault）**。
 * 现已在函数入口添加了 `size <= 1` 的安全防卫条件，彻底避免了下溢。
 *
 * 时间复杂度: $O(N^2)$ (最坏、最好、平均)
 * 空间复杂度: $O(1)$
 * 
 * @author [TsungHan Ho](https://github.com/dalaoqi)
 */

#include <algorithm>  /// 用于 std::is_sorted, std::swap
#include <cassert>    /// 用于 assert 断言
#include <cstdint>   /// 用于 std::uint32_t
#include <iostream>   /// 用于输入输出
#include <vector>     /// 用于 std::vector

/**
 * @namespace sorting
 * @brief 排序算法命名空间
 */
namespace sorting {
/**
 * @namespace cycle_sort
 * @brief 圈排序算法命名空间
 */
namespace cycle_sort {
/**
 * @brief 圈排序核心算法
 * @tparam T 数组元素模板类型
 * @param in_arr 待排序数组（只读引用）
 * @returns 排序完毕的新数组
 */
template <typename T>
std::vector<T> cycleSort(const std::vector<T> &in_arr) {
    std::vector<T> arr(in_arr);
    
    // 安全防卫：如果大小 <= 1，直接返回原数组，防止下溢崩溃
    if (arr.size() <= 1) {
        return arr;
    }

    for (int cycle_start = 0; cycle_start < static_cast<int>(arr.size()) - 1; cycle_start++) {
        T item = arr[cycle_start];

        // 1. 寻找当前循环起始点的正确插入位置
        int pos = cycle_start;
        for (size_t i = cycle_start + 1; i < arr.size(); i++) {
            if (arr[i] < item) {
                pos++;
            }
        }

        // 如果元素已在其正确的位置，则跳过
        if (pos == cycle_start) {
            continue;
        }

        // 跳过目标位置上可能存在的相同重复元素
        while (item == arr[pos]) {
            pos += 1;
        }
        
        // 交换放到正确位置
        std::swap(item, arr[pos]);

        // 2. 继续解决换出来的 item，直到环重新回到起点闭合
        while (pos != cycle_start) {
            pos = cycle_start;
            for (size_t i = cycle_start + 1; i < arr.size(); i++) {
                if (arr[i] < item) {
                    pos += 1;
                }
            }
            // 处理重复元素
            while (item == arr[pos]) {
                pos += 1;
            }
            std::swap(item, arr[pos]); // 移至正确位置
        }
    }
    return arr;
}
}  // namespace cycle_sort
}  // namespace sorting

/**
 * @brief 单元自测用例
 */
static void test() {
    // 测试 1
    std::vector<uint32_t> array1 = {4, 3, 2, 1};
    std::cout << "Test 1... ";
    std::vector<uint32_t> arr1 = sorting::cycle_sort::cycleSort(array1);
    assert(std::is_sorted(std::begin(arr1), std::end(arr1)));
    std::cout << "passed" << std::endl;

    // 测试 2: 浮点数与负数
    std::vector<double> array2 = {4.3, -6.5, -7.4, 0, 2.7, 1.8};
    std::cout << "Test 2... ";
    std::vector<double> arr2 = sorting::cycle_sort::cycleSort(array2);
    assert(std::is_sorted(std::begin(arr2), std::end(arr2)));
    std::cout << "passed" << std::endl;

    // 测试 3: 全部相同的数据
    std::vector<uint32_t> array3 = {3, 3, 3, 3};
    std::cout << "Test 3... ";
    std::vector<uint32_t> arr3 = sorting::cycle_sort::cycleSort(array3);
    assert(std::is_sorted(std::begin(arr3), std::end(arr3)));
    std::cout << "passed" << std::endl;

    // 测试 4: 已排序数组
    std::vector<uint32_t> array4 = {3, 4, 6, 8, 9, 14};
    std::cout << "Test 4... ";
    std::vector<uint32_t> arr4 = sorting::cycle_sort::cycleSort(array4);
    assert(std::is_sorted(std::begin(arr4), std::end(arr4)));
    std::cout << "passed" << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    test();  // 运行自测
    return 0;
}
