/**
 * @file
 * @brief Implementation of [Minimum Edit Distance](https://en.wikipedia.org/wiki/Edit_distance) using Dynamic Programming (基于动态规划的最小编辑距离算法实现)
 *
 * @details
 * 给定两个字符串 str1 和 str2，计算将 str1 转换为 str2 所需的最小操作次数。
 * 支持的三种操作（每种代价均为 1）：
 * 1. 插入（Insert）
 * 2. 删除（Remove）
 * 3. 替换（Replace）
 *
 * ### 动态规划递推关系（自底向上）
 * 设 `dp[i][j]` 表示 str1 的前 i 个字符到 str2 的前 j 个字符的最小编辑距离：
 * - 基础状态：
 *   - `dp[0][j] = j`（第一串为空，只能插入所有第二串字符）
 *   - `dp[i][0] = i`（第二串为空，只能删除所有第一串字符）
 * - 转移方程：
 *   - 若 `str1[i-1] == str2[j-1]`，则 `dp[i][j] = dp[i-1][j-1]`（字符相同，直接继承前驱状态）。
 *   - 否则，取三者最小值加 1：
 *     `dp[i][j] = 1 + min(dp[i][j-1], dp[i-1][j], dp[i-1][j-1])`
 *
 * @note
 * 【拷贝开销建议】：
 * 在第 92 行中，`editDistDP` 的参数设计依然将 `std::string` 作为按值传递（Pass-by-value）。
 * 在处理大型字符串时，每次调用都会复制整个字符串，带来不必要的时间和内存开销。
 * 建议在工程开发中修改为常引用传递：`const std::string&`。
 *
 * 时间复杂度: O(m * n)
 * 空间复杂度: O(m * n)
 *
 * @author [Nirjas Jakilim](github.com/nirzak)
 */

#include <cassert>   /// 用于 assert 断言
#include <cstdint>   /// 用于 std::uint64_t
#include <iostream>  /// 用于输入输出
#include <vector>    /// 用于 std::vector

/**
 * @namespace dynamic_programming
 * @brief 动态规划算法命名空间
 */
namespace dynamic_programming {
/**
 * @namespace minimum_edit_distance
 * @brief 最小编辑距离算法命名空间
 */
namespace minimum_edit_distance {
/**
 * @brief 获取三个数中的极小值
 */
uint64_t min(uint64_t x, uint64_t y, uint64_t z) {
    if (x <= y && x <= z) {
        return x;
    }
    if (y <= x && y <= z) {
        return y;
    } else {
        return z;
    }
}

/**
 * @brief 动态规划计算最小编辑距离
 * @param str1 源字符串（按值传递存在拷贝开销）
 * @param str2 目标字符串
 * @param m 字符串 str1 的长度
 * @param n 字符串 str2 的长度
 * @returns 最小编辑步骤数（标量代价和）
 */
uint64_t editDistDP(std::string str1, std::string str2, uint64_t m,
                    uint64_t n) {
    // 建立 (m+1) x (n+1) 的 DP 矩阵
    std::vector<std::vector<uint64_t>> dp(
        m + 1,
        std::vector<uint64_t>(n + 1));

    // 自底向上循环填表
    for (uint64_t i = 0; i <= m; i++) {
        for (uint64_t j = 0; j <= n; j++) {
            // 如果 str1 为空
            if (i == 0) {
                dp[i][j] = j;
            }
            // 如果 str2 为空
            else if (j == 0) {
                dp[i][j] = i;
            }
            // 末尾字符相同，不需要增加代价
            else if (str1[i - 1] == str2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1];
            }
            // 末尾字符不同，取三种编辑操作中最省开销的值加 1
            else {
                dp[i][j] = 1 + min(dp[i][j - 1],       // 插入
                                   dp[i - 1][j],       // 删除
                                   dp[i - 1][j - 1]);  // 替换
            }
        }
    }

    return dp[m][n];
}
}  // namespace minimum_edit_distance
}  // namespace dynamic_programming

/**
 * @brief 单元自测用例
 */
static void test() {
    // 测试 1
    std::string str1 = "INTENTION";
    std::string str2 = "EXECUTION";
    uint64_t expected_output1 = 5;
    uint64_t output1 = dynamic_programming::minimum_edit_distance::editDistDP(
        str1, str2, str1.length(), str2.length());
    assert(output1 == expected_output1);
    std::cout << "Minimum Number of Operations Required: " << output1
              << std::endl;

    // 测试 2
    std::string str3 = "SATURDAY";
    std::string str4 = "SUNDAY";
    uint64_t expected_output2 = 3;
    uint64_t output2 = dynamic_programming::minimum_edit_distance::editDistDP(
        str3, str4, str3.length(), str4.length());
    assert(output2 == expected_output2);
    std::cout << "Minimum Number of Operations Required: " << output2
              << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    test();  // 运行自测
    return 0;
}
