/**
 * @file
 * @brief Implementation of the [House Robber Problem](https://labuladong.gitbook.io/algo-en/i.-dynamic-programming/houserobber) algorithm (打家劫舍问题算法实现)
 * @details
 * 打家劫舍问题：每个房屋都存有特定金额的钱。相邻的房屋装有防盗系统，如果两间相邻的房屋在同一天被闯入，系统会自动报警。
 * 目标是在不触发警报的情况下，计算一天内能窃取的最高金额（即求非相邻元素的最大和）。
 *
 * ### 动态规划状态转移（空间优化后）
 * 设 `dp[i]` 为抢劫前 `i` 间房屋所能获得的最大金额。
 * - 递推关系：`dp[i] = max(dp[i-2] + money[i], dp[i-1])`
 *   - 抢劫当前房屋：收益为 `dp[i-2] + money[i]`（不能抢第 i-1 间）
 *   - 不抢当前房屋：收益为 `dp[i-1]`
 * - 空间优化：因为当前状态只依赖前两个状态，可以使用滚动变量 `value1` (即 `dp[i-2]`) 和 `value2` (即 `dp[i-1]`) 替代一维 DP 数组。
 *
 * 时间复杂度: O(N)
 * 空间复杂度: O(1)
 *
 * @author [Swastika Gupta](https://github.com/Swastyy)
 */

#include <cassert>   /// 用于 assert 断言
#include <climits>   /// 用于 std::max
#include <cstdint>   /// 用于 std::uint32_t
#include <iostream>  /// 用于输入输出
#include <vector>    /// 用于 std::vector

/**
 * @namespace dynamic_programming
 * @brief 动态规划算法命名空间
 */
namespace dynamic_programming {
/**
 * @namespace house_robber
 * @brief 打家劫舍相关算法命名空间
 */
namespace house_robber {
/**
 * @brief 计算不触发相邻警报下的最大抢劫金额
 * @param money 包含各房屋内金额的数组
 * @param n 房屋总间数
 * @returns 能抢到的最高总金额
 */
std::uint32_t houseRobber(const std::vector<uint32_t> &money,
                          const uint32_t &n) {
    if (n == 0) {
        return 0; // 没有房子
    }
    if (n == 1) {
        return money[0]; // 只有一间房子
    }
    if (n == 2) {
        return std::max(money[0], money[1]); // 只有两间房子，选金额大的
    }

    uint32_t max_value = 0;  // 记录遍历到的最大抢劫金额
    uint32_t value1 = money[0]; // dp[i-2]
    uint32_t value2 = std::max(money[0], money[1]); // dp[i-1]

    // 滚动更新变量，将空间复杂度降到 O(1)
    for (uint32_t i = 2; i < n; i++) {
        max_value = std::max(money[i] + value1, value2);
        value1 = value2;
        value2 = max_value;
    }

    return max_value;
}
}  // namespace house_robber
}  // namespace dynamic_programming

/**
 * @brief 单元自测用例
 */
static void test() {
    // 测试 1
    // [1, 2, 3, 1] -> 抢第 1 间 (1) 和第 3 间 (3)，最大收益 4
    std::vector<uint32_t> array1 = {1, 2, 3, 1};
    std::cout << "Test 1... ";
    assert(dynamic_programming::house_robber::houseRobber(array1, array1.size()) == 4);
    std::cout << "passed" << std::endl;

    // 测试 2
    // [6, 7, 1, 3, 8, 2, 4] -> 抢第 1, 3, 5, 7 间 (6 + 1 + 8 + 4)，最大收益 19
    std::vector<uint32_t> array2 = {6, 7, 1, 3, 8, 2, 4};
    std::cout << "Test 2... ";
    assert(dynamic_programming::house_robber::houseRobber(array2, array2.size()) == 19);
    std::cout << "passed" << std::endl;

    // 测试 3
    // [] -> 无房可抢，最大收益 0
    std::vector<uint32_t> array3 = {};
    std::cout << "Test 3... ";
    assert(dynamic_programming::house_robber::houseRobber(array3, array3.size()) == 0);
    std::cout << "passed" << std::endl;

    // 测试 4
    // [2, 7, 9, 3, 1] -> 抢第 1, 3, 5 间 (2 + 9 + 1)，最大收益 12
    std::vector<uint32_t> array4 = {2, 7, 9, 3, 1};
    std::cout << "Test 4... ";
    assert(dynamic_programming::house_robber::houseRobber(array4, array4.size()) == 12);
    std::cout << "passed" << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    test();  // 运行自测
    return 0;
}
