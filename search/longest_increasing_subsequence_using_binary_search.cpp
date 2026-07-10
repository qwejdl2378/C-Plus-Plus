/**
 * @file
 * @brief Find the length of the Longest Increasing Subsequence (LIS) using Binary Search (基于二分查找的动态规划 LIS 算法)
 * @details
 * 给定一个整数数组 nums，返回其最长严格递增子序列的长度。
 *
 * ### 算法演进与优化
 * 1. 暴力法 (Brute Force)：生成所有可能的子序列，过滤并找出最长的递增子序列。时间复杂度 O(2^n)，呈指数级。
 * 2. 普通动态规划 (DP)：状态转移方程定义 dp[i] 表示以 nums[i] 结尾的最长递增子序列长度。双层循环计算，时间复杂度 O(N^2)。
 * 3. 动态规划 + 二分查找 (Patience Sorting 思想)：维护一个辅助数组 `ans`，其第 i 个元素表示长度为 i+1 的所有递增子序列中最小的末尾值。
 *    对于每一个新读取的值：
 *    - 若大于当前已知的最大末尾值 `ans.back()`，直接追加到末尾，扩展 LIS 的最大长度。
 *    - 否则，通过二分查找（C++ 中的 `std::lower_bound`）定位到 `ans` 中第一个大于或等于该数的位置并替换它。
 *    这种贪心策略使得后续更容易拼接出更长的递增序列，时间复杂度大幅优化至 O(N log N)，空间复杂度为 O(N)。
 *
 * @author [Naman Jain](https://github.com/namanmodi65)
 */

#include <cassert>   /// 用于 assert 断言
#include <iostream>  /// 用于标准输出
#include <vector>    /// 用于 std::vector 容器
#include <algorithm> /// 用于 std::lower_bound 二分查找
#include <cstdint>   /// 用于 std::uint32_t

/**
 * @brief 基于二分查找求最长严格递增子序列的长度
 * @tparam T 数组元素类型
 * @param nums 待求解的输入数组
 * @return 最长严格递增子序列的长度值
 */
template <typename T>
std::uint32_t longest_increasing_subsequence_using_binary_search(std::vector<T>& nums) {
    if (nums.empty()) return 0;

    std::vector<T> ans; // 维护贪心状态的递增子序列辅助数组
    ans.push_back(nums[0]);
    
    for (std::size_t i = 1; i < nums.size(); i++) {
        if (nums[i] > ans.back()) {
            // 如果比当前最长子序列的尾部元素还大，直接追加，最长子序列长度 + 1
            ans.push_back(nums[i]);
        } else {
            // 否则，在 ans 数组中利用二分查找定位到第一个大于或等于 nums[i] 的元素并替换它，
            // 降低相应长度子序列的尾部元素值，使后续有更大概率生成更长的子序列。
            auto idx = std::lower_bound(ans.begin(), ans.end(), nums[i]) - ans.begin();
            ans[idx] = nums[i];
        }
    }
    // 返回最终 ans 数组的长度，即为最长递增子序列的最大长度
    return static_cast<std::uint32_t>(ans.size());
}

/**
 * @brief 单元自测用例
 */
static void tests() {
    std::vector<int> arr = {10, 9, 2, 5, 3, 7, 101, 18};
    assert(longest_increasing_subsequence_using_binary_search(arr) == 4); // LIS: {2, 3, 7, 18} 或 {2, 5, 7, 101}

    std::vector<int> arr2 = {0, 1, 0, 3, 2, 3};
    assert(longest_increasing_subsequence_using_binary_search(arr2) == 4); // LIS: {0, 1, 2, 3}

    std::vector<int> arr3 = {7, 7, 7, 7, 7, 7, 7};
    assert(longest_increasing_subsequence_using_binary_search(arr3) == 1); // 严格单调递增子序列长度为 1

    std::vector<int> arr4 = {-10, -1, -5, 0, 5, 1, 2};
    assert(longest_increasing_subsequence_using_binary_search(arr4) == 5); // LIS: {-10, -5, 0, 1, 2}

    std::vector<double> arr5 = {3.5, 1.2, 2.8, 3.1, 4.0};
    assert(longest_increasing_subsequence_using_binary_search(arr5) == 4);

    std::vector<char> arr6 = {'a', 'b', 'c', 'a', 'd'};
    assert(longest_increasing_subsequence_using_binary_search(arr6) == 4);

    std::vector<int> arr7 = {};
    assert(longest_increasing_subsequence_using_binary_search(arr7) == 0);

    std::cout << "All LIS tests have successfully passed!\n";
}

/**
 * @brief 主函数
 * @returns 0
 */
int main() {
    tests();  // 运行测试
    return 0;
}
