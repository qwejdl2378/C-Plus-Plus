/**
 * @file
 * @brief Implementation of the [Longest Increasing Subsequence](https://en.wikipedia.org/wiki/Longest_increasing_subsequence) algorithm (最长递增子序列算法实现)
 *
 * @details
 * 最长递增子序列（LIS, Longest Increasing Subsequence）：
 * 在给定的序列中，找出其元素按从低到高顺序排列的、且尽可能长的子序列。
 * 子序列中的元素在原序列中不一定是连续的。
 *
 * ### 动态规划递推原理
 * 设 `lis[i]` 表示以元素 `a[i]` 结尾的 LIS 长度：
 * - 基础状态：`lis[i] = 1` （各单元素自身长度为 1）
 *   对于任意前面的元素 `j < i`：
 *   若 `a[i] > a[j]`，则 `lis[i] = max(lis[i], lis[j] + 1)`。
 * 最终整个数组的 LIS 长度为整个 `lis` 数组中的最大值。
 *
 * 时间复杂度: O(N^2)
 * 空间复杂度: O(N)
 *
 * @author [Krishna Vedala](https://github.com/kvedala)
 * @author [David Leal](https://github.com/Panquesito7)
 */

#include <cassert>   /// 用于 assert 断言
#include <climits>   /// 用于 std::max
#include <cstdint>   /// 用于 std::uint64_t 和 std::uint32_t
#include <iostream>  /// 用于输入输出
#include <vector>    /// 用于 std::vector

/**
 * @namespace dynamic_programming
 * @brief 动态规划算法命名空间
 */
namespace dynamic_programming {
/**
 * @brief 计算给定数组的 LIS 长度
 * @param a 包含数字的 vector 数组
 * @param n 数组的大小
 * @returns 数组 a 的最长递增子序列的长度
 */
uint64_t LIS(const std::vector<uint64_t> &a, const uint32_t &n) {
    std::vector<int> lis(n);
    for (int i = 0; i < n; ++i) {
        lis[i] = 1; // 初始化每个位置的 LIS 长度为 1
    }
    
    // 双层循环：外层遍历结尾位置 i，内层寻找能够拼接的前驱位置 j
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < i; ++j) {
            if (a[i] > a[j] && lis[i] < lis[j] + 1) {
                lis[i] = lis[j] + 1; // 满足递增条件，更新 LIS 长度
            }
        }
    }
    
    // 找出所有结尾情况下的最大 LIS 长度
    int res = 0;
    for (int i = 0; i < n; ++i) {
        res = std::max(res, lis[i]);
    }
    return res;
}
}  // namespace dynamic_programming

/**
 * @brief 单元自测用例
 */
static void test() {
    std::vector<uint64_t> a = {15, 21, 2, 3, 4, 5, 8, 4, 1, 1};
    uint32_t n = a.size();

    uint32_t result = dynamic_programming::LIS(a, n);
    // 预期的最长递增子序列为 {2, 3, 4, 5, 8}，长度为 5
    assert(result == 5);

    std::cout << "Self-test implementations passed!" << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    uint32_t n = 0;

    std::cout << "Enter size of array: ";
    std::cin >> n;

    std::vector<uint64_t> a(n);

    std::cout << "Enter array elements: ";
    for (int i = 0; i < n; ++i) {
        std::cin >> a[i];
    }

    std::cout << "\nThe result is: " << dynamic_programming::LIS(a, n)
              << std::endl;
    
    test();  // 运行自测
    return 0;
}
