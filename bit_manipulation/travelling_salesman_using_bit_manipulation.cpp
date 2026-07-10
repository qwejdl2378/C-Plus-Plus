/**
 * @file
 * @brief Implementation to [Travelling Salesman problem using bit-masking](https://www.geeksforgeeks.org/travelling-salesman-problem-set-1/) (基于状态压缩与位运算的旅行商问题实现)
 *
 * @details
 * 旅行商问题（TSP）：给定一个城市间距离的邻接矩阵，寻找一条能够恰好访问每个城市一次，
 * 并且最后返回出发城市的最短路线（或者说最小总花费）。
 *
 * ### 状态压缩动态规划原理 (Held-Karp 算法)
 * 1. 状态定义：使用一个二进制数（位图掩码 `setOfCities`）代表已经访问过的城市集合。
 *    例如，如果有 4 个城市，`setOfCities = 11` (二进制为 `1011`) 代表城市 0, 1, 3 已被访问。
 * 2. 状态转移：`dp[setOfCities][city]` 表示当前位于 `city` 城市且已访问的城市集合为 `setOfCities` 时，访问完剩余城市并返回起点的最小花费。
 * 3. 递归转换：
 *    - 边界条件：当所有城市都已访问过（即 `setOfCities == (1 << n) - 1`）时，直接返回当前城市回到起点城市 0 的距离 `dist[city][0]`。
 *    - 递推关系：遍历每一个尚未访问的城市 `choice`（即满足 `(setOfCities & (1 << choice)) == 0`），
 *      `subProb = dist[city][choice] + TSP(setOfCities | (1 << choice), choice)`。取其中的最小值作为当前状态的解。
 *
 * 时间复杂度: O(n^2 * 2^n)
 * 空间复杂度: O(n * 2^n)，用于存储 DP 状态。
 * 
 * @author [Utkarsh Yadav](https://github.com/Rytnix)
 */

#include <algorithm>  /// 用于 std::min
#include <cassert>    /// 用于 assert 断言
#include <cstdint>   /// 用于 std::uint32_t 和 std::uint64_t
#include <iostream>   /// 用于输入输出
#include <limits>     /// 用于数值极限
#include <vector>     /// 用于 std::vector

/**
 * @namespace bit_manipulation
 * @brief 位运算算法命名空间
 */
namespace bit_manipulation {
/**
 * @namespace travelling_salesman_using_bit_manipulation
 * @brief 基于位掩码的旅行商问题算法相关命名空间
 */
namespace travelling_salesman_using_bit_manipulation {
/**
 * @brief 记忆化搜索求解旅行商问题最小代价值
 * @param dist 邻接矩阵表示的城市间花费距离
 * @param setOfCities 当前已访问城市的二进制掩码
 * @param city 当前所在的城市编号
 * @param n 城市的总个数
 * @param dp 记忆化状态表 dp[setOfCities][city]
 * @returns 遍历剩余所有节点并返回起点 0 的最小花费
 */
std::uint64_t travelling_salesman_using_bit_manipulation(
    std::vector<std::vector<uint32_t>> dist,
    std::uint64_t setOfCities,
    std::uint64_t city,
    std::uint64_t n,
    std::vector<std::vector<uint32_t>> &dp) 
{
    // 递归边界：所有城市均已访问完
    if (setOfCities == (1 << n) - 1) {
        return dist[city][0];  // 返回当前城市回到初始城市 0 的距离
    }

    // 记忆化剪枝：如果当前状态已被计算过，直接返回
    if (dp[setOfCities][city] != -1) {
        return dp[setOfCities][city];
    }
    
    // 尝试前往所有未访问过的下一个城市，求最小开销
    uint64_t ans = 2147483647; // 初始化为一个较大值
    for (int choice = 0; choice < n; choice++) {
        // 判断第 choice 个城市是否未被访问
        if ((setOfCities & (1 << choice)) == 0) {
            std::uint64_t subProb =
                dist[city][choice] +
                travelling_salesman_using_bit_manipulation(
                    dist, setOfCities | (1 << choice), choice, n, dp);
            
            ans = std::min(ans, subProb); // 取最小值
        }
    }
    // 记录并返回当前状态的最优解
    dp[setOfCities][city] = ans;
    return ans;
}
}  // namespace travelling_salesman_using_bit_manipulation
}  // namespace bit_manipulation

/**
 * @brief 单元自测用例
 */
static void test() {
    // 测试用例 1
    std::vector<std::vector<uint32_t>> dist = {
        {0, 20, 42, 35}, {20, 0, 30, 34}, {42, 30, 0, 12}, {35, 34, 12, 0}};
    uint32_t V = dist.size();
    std::vector<std::vector<uint32_t>> dp(1 << V, std::vector<uint32_t>(V, -1));
    assert(bit_manipulation::travelling_salesman_using_bit_manipulation::
               travelling_salesman_using_bit_manipulation(dist, 1, 0, V, dp) == 97);
    std::cout << "1st test-case: passed!" << "\n";

    // 测试用例 2
    dist = {{0, 5, 10, 15}, {5, 0, 20, 30}, {10, 20, 0, 35}, {15, 30, 35, 0}};
    V = dist.size();
    std::vector<std::vector<uint32_t>> dp1(1 << V, std::vector<uint32_t>(V, -1));
    assert(bit_manipulation::travelling_salesman_using_bit_manipulation::
               travelling_salesman_using_bit_manipulation(dist, 1, 0, V, dp1) == 75);
    std::cout << "2nd test-case: passed!" << "\n";
    
    // 测试用例 3
    dist = {{0, 10, 15, 20}, {10, 0, 35, 25}, {15, 35, 0, 30}, {20, 25, 30, 0}};
    V = dist.size();
    std::vector<std::vector<uint32_t>> dp2(1 << V, std::vector<uint32_t>(V, -1));
    assert(bit_manipulation::travelling_salesman_using_bit_manipulation::
               travelling_salesman_using_bit_manipulation(dist, 1, 0, V, dp2) == 80);

    std::cout << "3rd test-case: passed!" << "\n";
}

/**
 * @brief 主函数
 * @returns 0
 */
int main() {
    test();  // 运行自测
    return 0;
}
