/**
 * @file
 * @brief Implementation of the Unbounded 0/1 Knapsack Problem (完全背包问题/无界背包算法实现)
 *
 * @details
 * 完全背包问题（Unbounded Knapsack Problem）：
 * 与 0-1 背包问题每个物品仅限选择一次不同，完全背包允许每个物品选择任意多次。
 * 目标是在不超过背包总重 limit `W` 的前提下，使背包中装入物品的总价值最大。
 *
 * ### 动态规划状态转移（记忆化递归自顶向下）
 * 设 `KnapSackFilling(i, W)` 表示在前 `i` 种物品中进行选择，剩余容量为 `W` 时的最大价值：
 * - 边界条件：当 `i == 0` 时，只能选第 0 种物品，尽可能多地装入：`return (W / wt[0]) * val[0]`。
 * - 状态转移：对于第 `i` 种物品，有两种决策：
 *   1. 不放入背包（`nottake`）：转为子问题 `KnapSackFilling(i - 1, W)`
 *   2. 放入背包至少一个（`take`）：如果 `W >= wt[i]`，累加当前物品价值 `val[i]`，由于物品数量无限，**索引仍保持为 i**，继续决策：
 *      `take = val[i] + KnapSackFilling(i, W - wt[i])`
 *   最后取两决策的较大值缓存并返回。
 *
 * @note
 * 【类型转换与截断风险说明】：
 * 在第 58 行和第 93 行中，函数返回类型被硬编码为了 `std::uint16_t`。
 * `std::uint16_t` 的最大取值范围仅为 65535。如果背包容量较大或者物品总价值较高，
 * 计算结果很容易溢出并发生截断。然而，内部 `dp` 矩阵以及局部决策变量 `take`, `nottake` 都是使用标准 `int` 存储的。
 * 为了规避高价值输入下的溢出风险，应当统一将返回值修改为 `int` 或 `int64_t` / `uint32_t`。
 *
 * 时间复杂度: O(N * W)
 * 空间复杂度: O(N * W)
 *
 * @author [Sanskruti Yeole](https://github.com/yeolesanskruti)
 * @see dynamic_programming/0_1_knapsack.cpp
 */

#include <cassert>   // 用于 assert 断言
#include <cstdint>   // 用于固定宽度整型类型如 std::uint16_t
#include <iostream>  // 用于标准输入输出
#include <vector>    // 用于 std::vector

/**
 * @namespace dynamic_programming
 * @brief 动态规划算法命名空间
 */
namespace dynamic_programming {
/**
 * @namespace unbounded_knapsack
 * @brief 完全背包问题算法相关命名空间
 */
namespace unbounded_knapsack {
/**
 * @brief 记忆化递归填充背包
 * @param i 当前考虑的物品索引
 * @param W 背包剩余可用容量
 * @param val 各物品价值数组
 * @param wt 各物品重量数组
 * @param dp 记忆化二维矩阵表
 * @returns 对应状态下的最大价值（注意 uint16_t 截断风险）
 */
std::uint16_t KnapSackFilling(std::uint16_t i, std::uint16_t W,
                              const std::vector<std::uint16_t>& val,
                              const std::vector<std::uint16_t>& wt,
                              std::vector<std::vector<int>>& dp) {
    // 边界条件：仅剩第 0 种物品，装满为止
    if (i == 0) {
        if (wt[0] <= W) {
            return (W / wt[0]) * val[0];
        } else {
            return 0;
        }
    }
    // 命中缓存则直接返回
    if (dp[i][W] != -1)
        return dp[i][W];

    // 决策 1：不选择当前物品，转到前一个物品种类
    int nottake = KnapSackFilling(i - 1, W, val, wt, dp);
    
    // 决策 2：选择当前物品（可以重复选择，因此索引仍为 i）
    int take = 0;
    if (W >= wt[i]) {
        take = val[i] + KnapSackFilling(i, W - wt[i], val, wt, dp);
    }
    
    // 保存并返回两者的最大值
    return dp[i][W] = std::max(take, nottake);
}

/**
 * @brief 完全背包问题封装入口
 * @param N 物品种类数
 * @param W 背包最大承重上限
 * @param val 各物品价值数组
 * @param wt 各物品重量数组
 * @returns 最大价值总和
 */
std::uint16_t unboundedKnapsack(std::uint16_t N, std::uint16_t W,
                                const std::vector<std::uint16_t>& val,
                                const std::vector<std::uint16_t>& wt) {
    if (N == 0)
        return 0; // 没有物品，价值为 0
        
    // 初始化记忆化二维数组，填充 -1 代表未计算
    std::vector<std::vector<int>> dp(N, std::vector<int>(W + 1, -1));
    return KnapSackFilling(N - 1, W, val, wt, dp);
}

}  // namespace unbounded_knapsack
}  // namespace dynamic_programming

/**
 * @brief 单元自测用例
 */
static void tests() {
    // 测试 1
    std::uint16_t N1 = 4;
    std::vector<std::uint16_t> wt1 = {1, 3, 4, 5};
    std::vector<std::uint16_t> val1 = {6, 1, 7, 7};
    std::uint16_t W1 = 8;
    // 预期最大价值为 48 (选择 8 个重量为 1, 价值为 6 的物品)
    assert(dynamic_programming::unbounded_knapsack::unboundedKnapsack(N1, W1, val1, wt1) == 48);
    std::cout << "Maximum Knapsack value "
              << dynamic_programming::unbounded_knapsack::unboundedKnapsack(N1, W1, val1, wt1)
              << std::endl;

    // 测试 2
    std::uint16_t N2 = 3;
    std::vector<std::uint16_t> wt2 = {10, 20, 30};
    std::vector<std::uint16_t> val2 = {60, 100, 120};
    std::uint16_t W2 = 5; // 背包容量不够装载任何最轻的物品
    assert(dynamic_programming::unbounded_knapsack::unboundedKnapsack(N2, W2, val2, wt2) == 0);
    std::cout << "Maximum Knapsack value "
              << dynamic_programming::unbounded_knapsack::unboundedKnapsack(N2, W2, val2, wt2)
              << std::endl;

    // 测试 3
    std::uint16_t N3 = 3;
    std::vector<std::uint16_t> wt3 = {2, 4, 6};
    std::vector<std::uint16_t> val3 = {5, 11, 13};
    std::uint16_t W3 = 27;
    // 预期最大价值为 27
    assert(dynamic_programming::unbounded_knapsack::unboundedKnapsack(N3, W3, val3, wt3) == 27);
    std::cout << "Maximum Knapsack value "
              << dynamic_programming::unbounded_knapsack::unboundedKnapsack(N3, W3, val3, wt3)
              << std::endl;

    // 测试 4
    std::uint16_t N4 = 0;
    std::vector<std::uint16_t> wt4 = {};
    std::vector<std::uint16_t> val4 = {};
    std::uint16_t W4 = 10;
    assert(dynamic_programming::unbounded_knapsack::unboundedKnapsack(N4, W4, val4, wt4) == 0);
    std::cout << "Maximum Knapsack value for empty arrays: "
              << dynamic_programming::unbounded_knapsack::unboundedKnapsack(N4, W4, val4, wt4)
              << std::endl;

    std::cout << "All test cases passed!" << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    tests();  // 运行自测
    return 0;
}
