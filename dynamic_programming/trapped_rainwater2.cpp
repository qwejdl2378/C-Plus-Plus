/**
 * @file
 * @brief Space-optimized Implementation of the [Trapped Rainwater Problem](https://www.geeksforgeeks.org/trapping-rain-water/) (接雨水问题双指针 O(1) 空间优化算法实现)
 *
 * @details
 * 本实现是接雨水问题（Trapping Rain Water）的最优解法：双指针扫描。
 * 与 `trapped_rainwater.cpp` 中分配两个 O(N) 空间的前缀/后缀数组不同，
 * 本方法仅使用两个辅助变量和指针即可完成。
 * 
 * ### 核心解题原理（双指针收缩）
 * 1. 维护左指针 `left` 和右指针 `right`，分别初始化为 0 和 n-1。
 * 2. 跟踪左右两侧见过的最大墙高度 `leftMax` 和 `rightMax`。
 * 3. 在每一步比较 `heights[left]` 和 `heights[right]`：
 *    - 如果 `heights[left] < heights[right]`：说明右边必然存在一个比左边更高的墙（至少不低于 `heights[right]`）。
 *      因此，左侧能够蓄水的高度仅由 `leftMax` 决定。若当前高度小于 `leftMax` 则累加蓄水量 `leftMax - heights[left]`，否则更新 `leftMax`；接着向右移动 `left` 指针。
 *    - 反之（左侧墙不低于右侧）：说明左边存在一个不低于右侧的墙，蓄水量仅由 `rightMax` 决定。更新蓄水并向左移动 `right` 指针。
 *
 * 时间复杂度: O(N)
 * 空间复杂度: O(1)
 *
 * @author [kanavgoyal898](https://github.com/kanavgoyal898)
 */

#include <algorithm>  /// 用于 std::min 和 std::max
#include <cassert>    /// 用于 assert 断言
#include <cstddef>    /// 用于 std::size_t
#include <cstdint>    /// 用于 std::uint32_t
#include <vector>     /// 用于 std::vector

/**
 * @namespace dynamic_programming
 * @brief 动态规划与贪心算法命名空间
 */
namespace dynamic_programming {
/**
 * @brief 双指针法计算总接雨水量
 * @param heights 表示柱子高度的 vector
 * @returns 总接水量
 */
uint32_t trappedRainwater(const std::vector<uint32_t>& heights) {
    std::size_t n = heights.size();
    if (n <= 2)
        return 0;  // 柱子数量太少，无法蓄水

    std::size_t left = 0, right = n - 1;
    uint32_t leftMax = 0, rightMax = 0, trappedWater = 0;

    // 左右指针相向移动收缩
    while (left < right) {
        if (heights[left] < heights[right]) {
            // 右侧存在屏障，左侧蓄水量仅由左侧最高点 leftMax 决定
            if (heights[left] >= leftMax)
                leftMax = heights[left];  // 更新左侧最高高度
            else
                trappedWater += leftMax - heights[left];  // 累加局部蓄水量
            ++left;
        } else {
            // 左侧存在屏障，右侧蓄水量仅由右侧最高点 rightMax 决定
            if (heights[right] >= rightMax)
                rightMax = heights[right];  // 更新右侧最高高度
            else
                trappedWater += rightMax - heights[right];  // 累加局部蓄水量
            --right;
        }
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
