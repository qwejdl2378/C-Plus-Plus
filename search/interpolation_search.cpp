/******************************************************************************
 * @file
 * @brief [Interpolation search algorithm](https://en.wikipedia.org/wiki/interpolation_search) (插值查找算法)
 *
 * @details
 * 插值查找类似于我们在电话簿中查找名字的过程。
 * 1. 在每一步中，算法基于查找区间边界处的键值以及待查目标值本身，通过线性插值估计目标元素可能处于的位置。
 * 2. 估计位置公式：pos = low + ((high - low) / (arr[high] - arr[low])) * (key - arr[low])
 * 3. 比较该位置的值与目标值，进而收缩区间。
 * 
 * ### 时间复杂度
 * * 最好情况（分布均匀时）：O(log2(log2 n))
 * * 最差情况（呈指数级非均匀分布）：O(n)
 *
 * @author [Lajat Manekar](https://github.com/Lazeeez)
 * *******************************************************************************/

#include <algorithm>  /// 用于 std::sort
#include <cassert>    /// 用于 assert 断言
#include <cstdint>
#include <iostream>   /// 用于标准输出
#include <vector>     /// 用于 std::vector

/**
 * @namespace search
 * @brief 搜索算法命名空间
 */
namespace search {

/**
 * @namespace interpolation_search
 * @brief 插值查找相关命名空间
 */
namespace interpolation_search {

/**
 * @brief 执行插值查找算法
 * 
 * @note 
 * 【经典整数除法精度 Bug 说明】：
 * 在下方的第 75 行公式中：
 * `((static_cast<uint64_t>(high - low) / (arr[high] - arr[low])) * (number - arr[low]))`
 * 由于先执行了 `(high - low) / (arr[high] - arr[low])` 整数除法，
 * 只要数组值跨度大于索引跨度（这是绝大多数常见情况），该整数除法的结果就会直接截断为 0！
 * 这导致 `pos` 每次计算都为 `low`，使本算法退化成了低效的普通逐个比对。
 * 正确的写法应该先乘后除，或者使用 double 浮点数类型计算：
 * `low + (double)(high - low) / (arr[high] - arr[low]) * (number - arr[low])`
 * 
 * @param arr 已排序的 vector 数组
 * @param number 待查找的目标数字值
 * @returns 目标值对应的索引，找不到则返回 -1 (uint64_t 最大值)
 */
uint64_t interpolationSearch(const std::vector<uint64_t> &arr,
                             uint64_t number) {
    uint64_t size = arr.size();
    uint64_t low = 0, high = (size - 1);

    // 目标值必须在区间边界内
    while (low <= high && number >= arr[low] && number <= arr[high]) {
        if (low == high) {
            if (arr[low] == number) {
                return low;
            }
            return -1;
        }
        
        // 依据线性插值比例估计探测点索引 pos
        uint64_t pos =
            low +
            ((static_cast<uint64_t>(high - low) / (arr[high] - arr[low])) *
             (number - arr[low]));

        if (arr[pos] == number) {
            return pos;  // 成功找到
        }

        if (arr[pos] < number) {
            low = pos + 1;  // 目标值在右半部分，更新 low
        } else {
            high = pos - 1; // 目标值在左半部分，更新 high
        }
    }
    return -1;
}

}  // namespace interpolation_search
}  // namespace search

/**
 * @brief 单元自测用例
 */
static void tests() {
    std::vector<uint64_t> arr = {{10, 12, 13, 16, 18, 19, 20, 21, 1, 2, 3, 4,
                                  22, 23, 24, 33, 35, 42, 47}};
    std::sort(arr.begin(), arr.end()); // 插值查找必须基于有序数组
    uint64_t number = 33;
    uint64_t expected_answer = 15;
    uint64_t derived_answer =
        search::interpolation_search::interpolationSearch(arr, number);
    std::cout << "Testcase: ";
    assert(derived_answer == expected_answer);
    std::cout << "Passed!\n";
}

/**
 * @brief 主函数
 * @returns 0
 */
int main() {
    tests();  // 运行自测
    return 0;
}
