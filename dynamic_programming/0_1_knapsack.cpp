/**
 * @file
 * @brief Implementation of [0-1 Knapsack Problem](https://en.wikipedia.org/wiki/Knapsack_problem) (0-1 背包问题动态规划实现)
 *
 * @details
 * 给定 n 个物品的重量和价值，将这些物品放入容量为 `W` 的背包中，以使得背包中物品的总价值最大。
 * 换句话说，给定两个分别代表物品价值和重量的整型数组 `val[0..n-1]` 和 `wt[0..n-1]`，
 * 以及代表背包最大载重容量的整型值 W，求价值最大的物品子集，使得它们的重量之和小于或等于 W。
 * 在 0-1 背包问题中，物品是不可分割的，每个物品要么完整放入背包，要么不放（即 0-1 属性）。
 *
 * ### 算法思想
 * 使用动态规划（Dynamic Programming）。
 * 设状态 `maxValue[i][j]` 表示：在前 `i` 个物品中选择，且背包容量限制为 `j` 时能获得的最大价值。
 * 状态转移方程：
 * 1. 如果不选第 `i` 个物品（或者其重量大于当前背包限制 `j`）：
 *    `maxValue[i][j] = maxValue[i-1][j]`
 * 2. 如果选择第 `i` 个物品（前提是 `weight[i-1] <= j`）：
 *    `maxValue[i][j] = max(maxValue[i-1][j], value[i-1] + maxValue[i-1][j - weight[i-1]])`
 *
 * @author [Anmol](https://github.com/Anmol3299)
 * @author [Pardeep](https://github.com/Pardeep009)
 */

#include <array>
#include <cassert>
#include <iostream>
#include <vector>

/**
 * @namespace dynamic_programming
 * @brief 动态规划算法命名空间
 */
namespace dynamic_programming {
/**
 * @namespace knapsack
 * @brief 0-1 背包问题相关函数
 */
namespace knapsack {
/**
 * @brief 计算给定背包容量和物品属性下能获得的最大价值
 * @tparam n 物品数量 (数组模板参数)
 * @param capacity 背包的最大承重容量
 * @param weight 物品的重量数组
 * @param value 物品的价值数组
 * @return 能够装入背包的最大总价值
 */
template <size_t n>
int maxKnapsackValue(const int capacity, const std::array<int, n> &weight,
                     const std::array<int, n> &value) {
    // 二维 DP 状态表，行数代表允许考虑的物品个数 (0 到 n)，列数代表当前的容量上限 (0 到 capacity)
    std::vector<std::vector<int> > maxValue(n + 1,
                                            std::vector<int>(capacity + 1, 0));
    
    // 外层循环：遍历允许放入背包的物品个数
    // 内层循环：遍历背包当前的承重限制
    int items = sizeof(weight) / sizeof(weight[0]);
    for (size_t i = 0; i < items + 1; ++i) {
        for (size_t j = 0; j < capacity + 1; ++j) {
            // 边界条件：当物品个数为 0 或背包容量限制为 0 时，最大价值只能为 0
            if (i == 0 || j == 0) {
                maxValue[i][j] = 0;
            } 
            // 如果第 i 个物品（索引为 i-1）的重量小于或等于当前背包容量 j
            else if (weight[i - 1] <= j) {
                // 我们可以选择装入或者不装入该物品：
                
                // 选择装入：当前物品价值 + 扣除该物品重量后剩余容量的最优解
                int profit1 = value[i - 1] + maxValue[i - 1][j - weight[i - 1]];

                // 选择不装入：在相同容量下，仅考虑前 i-1 个物品的最优解
                int profit2 = maxValue[i - 1][j];

                // 状态转移：取两者之中的较大值
                maxValue[i][j] = std::max(profit1, profit2);
            } 
            // 如果当前物品的重量大于当前背包容量限制 j，说明装不下，必须排除
            else {
                maxValue[i][j] = maxValue[i - 1][j];
            }
        }
    }

    // 返回考虑完所有 n 个物品、背包容量为 capacity 时的最大价值
    return maxValue[items][capacity];
}
}  // namespace knapsack
}  // namespace dynamic_programming

/**
 * @brief 自测用例集
 */
static void test() {
    // 测试用例 1
    const int n1 = 3;                             // 3 个物品
    std::array<int, n1> weight1 = {10, 20, 30};   // 物品重量
    std::array<int, n1> value1 = {60, 100, 120};  // 物品价值
    const int capacity1 = 50;                     // 背包最大容量
    const int max_value1 = dynamic_programming::knapsack::maxKnapsackValue(
        capacity1, weight1, value1);
    const int expected_max_value1 = 220; // 预期最大价值：选择物品 1 (10) 和 2 (20)，总价 60+100=160; 或者是选择 2 (20) 和 3 (30)，总价 100+120=220。
    assert(max_value1 == expected_max_value1);
    std::cout << "Maximum Knapsack value with " << n1 << " items is "
              << max_value1 << std::endl;

    // 测试用例 2
    const int n2 = 4;                               // 4 个物品
    std::array<int, n2> weight2 = {24, 10, 10, 7};  // 物品重量
    std::array<int, n2> value2 = {24, 18, 18, 10};  // 物品价值
    const int capacity2 = 25;                       // 背包最大容量
    const int max_value2 = dynamic_programming::knapsack::maxKnapsackValue(
        capacity2, weight2, value2);
    const int expected_max_value2 = 36;  // 预期最大价值：选择物品 2 (10) 和 3 (10)，价值为 18+18=36。
    assert(max_value2 == expected_max_value2);
    std::cout << "Maximum Knapsack value with " << n2 << " items is "
              << max_value2 << std::endl;
}

/**
 * @brief 主函数
 * @returns 0
 */
int main() {
    // 运行自测用例
    test();
    return 0;
}
