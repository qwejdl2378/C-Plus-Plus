/**
 * @file
 * @brief Implementation of the [Trapped Rainwater Problem](https://www.geeksforgeeks.org/trapping-rain-water/) (接雨水问题前缀/后缀高程差算法实现)
 *
 * @details
 * 给定代表每个柱子高度的非负整数数组，计算下雨之后这个地形能接多少雨水。
 *
 * ### 核心解题思路（双指针高程差/前缀后缀最大值）
 * 1. 对于第 `i` 个柱子，它能接的水量取决于它左右两侧的柱子最大高度的最小值：
 *    `water[i] = min(leftMax[i], rightMax[i]) - heights[i]`
 * 2. `leftMax[i]`：从左侧数第 0 到第 i 个柱子的最大高度。可以通过一次从左往右的扫描动态递推求出。
 * 3. `rightMax[i]`：从右侧数第 i 到第 n-1 个柱子的最大高度。可以通过一次从右往左的扫描动态递推求出。
 * 4. 最后累加各个位置的蓄水量即为总水量。
 *
 * @note
 * 【无符号递减循环的编写技巧】：
 * 在第 42-44 行反向计算 `rightMax` 的循环中：
 *   `for (std::size_t i = n - 2; i < n; --i)`
 * 1. `i` 的类型是无符号整型 `std::size_t`。
 * 2. 当 `i` 递减到 `0` 并执行 `--i` 后，无符号数溢出回绕，值将变为无符号的最大值 `18446744073709551615`。
 * 3. 此时，条件 `i < n` 不再成立（因为最大值显然大于 `n`），循环终止。这是无符号类型在反向递减循环中非常经典的用法，既避免了带符号整型下标警告，又能确保正确处理索引 0。
 *
 * 时间复杂度: O(N)
 * 空间复杂度: O(N)
 *
 * @author [SOZEL](https://github.com/TruongNhanNguyen)
 */

#include <algorithm>  /// 用于 std::min 和 std::max
#include <cassert>    /// 用于 assert 断言
#include <cstddef>    /// 用于 std::size_t
#include <cstdint>    /// 用于 std::uint32_t
#include <vector>     /// 用于 std::vector

/**
 * @namespace dynamic_programming
 * @brief 动态规划算法命名空间
 */
namespace dynamic_programming {
/**
 * @brief 计算总接雨水量
 * @param heights 表示柱子高度的 vector
 * @returns 总接水量
 */
uint32_t trappedRainwater(const std::vector<uint32_t>& heights) {
    std::size_t n = heights.size();
    if (n <= 2)
        return 0;  // 柱子少于 3 根，无法蓄水

    std::vector<uint32_t> leftMax(n), rightMax(n);

    // 1. 从左往右递推每个位置左侧的最大高度
    leftMax[0] = heights[0];
    for (std::size_t i = 1; i < n; ++i) {
        leftMax[i] = std::max(leftMax[i - 1], heights[i]);
    }

    // 2. 从右往左递推每个位置右侧的最大高度
    rightMax[n - 1] = heights[n - 1];
    for (std::size_t i = n - 2; i < n; --i) {
        rightMax[i] = std::max(rightMax[i + 1], heights[i]);
    }

    // 3. 累加各个位置的蓄水量
    uint32_t trappedWater = 0;
    for (std::size_t i = 0; i < n; ++i) {
        trappedWater +=
            std::max(0u, std::min(leftMax[i], rightMax[i]) - heights[i]);
    }

    return trappedWater;
}

}  // namespace dynamic_programming

/**
 * @brief 单元自测用例
 */
static void test() {
    std::vector<uint32_t> test_basic = {0, 1, 0, 2, 1, 0, 1, 3, 2, 1, 2, 1};
    assert(dynamic_programming::trappedRainwater(test_basic) == 6);

    std::vector<uint32_t> test_peak_under_water = {3, 0, 2, 0, 4};
    assert(dynamic_programming::trappedRainwater(test_peak_under_water) == 7);

    std::vector<uint32_t> test_bucket = {5, 1, 5};
    assert(dynamic_programming::trappedRainwater(test_bucket) == 4);

    std::vector<uint32_t> test_skewed_bucket = {4, 1, 5};
    assert(dynamic_programming::trappedRainwater(test_skewed_bucket) == 3);

    std::vector<uint32_t> test_empty = {};
    assert(dynamic_programming::trappedRainwater(test_empty) == 0);

    std::vector<uint32_t> test_flat = {0, 0, 0, 0, 0};
    assert(dynamic_programming::trappedRainwater(test_flat) == 0);

    std::vector<uint32_t> test_no_trapped_water = {1, 1, 2, 4, 0, 0, 0};
    assert(dynamic_programming::trappedRainwater(test_no_trapped_water) == 0);

    std::vector<uint32_t> test_single_elevation = {5};
    assert(dynamic_programming::trappedRainwater(test_single_elevation) == 0);

    std::vector<uint32_t> test_two_point_elevation = {5, 1};
    assert(dynamic_programming::trappedRainwater(test_two_point_elevation) == 0);

    std::vector<uint32_t> test_large_elevation_map_difference = {5, 1, 6, 1,
                                                                 7, 1, 8};
    assert(dynamic_programming::trappedRainwater(
               test_large_elevation_map_difference) == 15);
}

/**
 * @brief 主函数
 */
int main() {
    test();  // 运行自测
    return 0;
}
