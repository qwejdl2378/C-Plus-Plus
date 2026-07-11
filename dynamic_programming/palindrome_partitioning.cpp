/**
 * @file
 * @brief Implementation of [Palindrome Partitioning II](https://leetcode.com/problems/palindrome-partitioning-ii/) (回文最少分割问题算法实现)
 *
 * @details
 * 给定一个字符串，求将其分割成若干个子串，且每个子串都是回文串所需的最少分割次数（最少剪切数）。
 *
 * ### 本文件中的动态规划实现分析（区间 DP，时间复杂度为 $O(N^3)$）
 * 本代码采用了区间 DP 的思路，使用两个二维查找表：
 * 1. `is_palindrome[i][j]`：布尔表，记录 `str[i...j]` 是否为回文串。
 * 2. `cuts[i][j]`：记录 `str[i...j]` 成为回文分块所需的最小分割次数。
 *
 * #### 状态转移方程
 * - 如果 `is_palindrome[i][j]` 为真，则 `cuts[i][j] = 0`（无需分割）。
 * - 否则，枚举所有可能的分割点 `k`（从 `i` 到 `j-1`），取左右两半的分割数加 1 的极小值：
 *   `cuts[i][j] = min_{i <= k < j} (cuts[i][k] + cuts[k+1][j] + 1)`
 *
 * @note
 * 【时间复杂度与优化建议】：
 * 1. 本代码实现的区间 DP 包含了三层嵌套循环（长度 `len`、起点 `start_index`、分割点 `partition`），因此**时间复杂度为 $O(N^3)$**。
 * 2. 实际上，可以通过引入 1D DP 数组 `cuts[i]`（表示前缀 `str[0...i]` 的最小分割数）将**时间复杂度优化到 $O(N^2)$**：
 *    `cuts[i] = min_{0 <= j <= i} (cuts[j-1] + 1)`，其中 `str[j...i]` 是回文串。
 *    利用 `is_palindrome` 二维表，我们可以在 $O(N^2)$ 内完成所有计算，无需在内层遍历所有分割点。
 *
 * @author [Sujay Kaushik] (https://github.com/sujaykaushik008)
 */

#include <algorithm>  // 用于 std::min
#include <cassert>    // 用于 assert 断言
#include <climits>    // 用于 INT_MAX
#include <iostream>   // 用于输入输出
#include <vector>     // 用于 std::vector

/**
 * @namespace dynamic_programming
 * @brief 动态规划算法命名空间
 */
namespace dynamic_programming {
/**
 * @namespace palindrome_partitioning
 * @brief 回文分割相关算法命名空间
 */
namespace palindrome_partitioning {
/**
 * @brief 使用区间 DP 计算最少回文切割次数
 * @param str 输入的字符串
 * @returns 达到全回文子串所需的最少切分数
 */
int pal_part(const std::string &str) {
    int n = str.size();

    // cuts[i][j] 代表 str[i..j] 达到全回文划分的最少剪切数
    std::vector<std::vector<int> > cuts(n, std::vector<int>(n, 0));

    // is_palindrome[i][j] 代表子串 str[i..j] 是否是回文
    std::vector<std::vector<bool> > is_palindrome(n,
                                                  std::vector<bool>(n, false));

    // 初始化：单个字符本身就是回文，剪切数为 0
    for (int i = 0; i < n; i++) {
        is_palindrome[i][i] = true;
        cuts[i][i] = 0;
    }

    // 枚举区间长度 len
    for (int len = 2; len <= n; len++) {
        // 枚举起点 start_index
        for (int start_index = 0; start_index < n - len + 1; start_index++) {
            int end_index = start_index + len - 1;

            // 根据左右字符及子问题判断是否是回文
            if (len == 2) {
                is_palindrome[start_index][end_index] =
                    (str[start_index] == str[end_index]);
            } else {
                is_palindrome[start_index][end_index] =
                    (str[start_index] == str[end_index]) &&
                    is_palindrome[start_index + 1][end_index - 1];
            }

            // 如果整个区间就是回文，剪切数为 0
            if (is_palindrome[start_index][end_index]) {
                cuts[start_index][end_index] = 0;
            } else {
                cuts[start_index][end_index] = INT_MAX;
                // 警告：这里循环枚举了所有分割点，使得总复杂度达到了 O(N^3)
                for (int partition = start_index; partition <= end_index - 1;
                     partition++) {
                    cuts[start_index][end_index] =
                        std::min(cuts[start_index][end_index],
                                 cuts[start_index][partition] +
                                     cuts[partition + 1][end_index] + 1);
                }
            }
        }
    }

    return cuts[0][n - 1];
}
}  // namespace palindrome_partitioning
}  // namespace dynamic_programming

/**
 * @brief 单元自测用例
 */
static void test() {
    std::vector<std::string> custom_input{"nitik", "ababbbabbababa", "abdc"};
    std::vector<int> calculated_output(3);

    for (int i = 0; i < 3; i++) {
        calculated_output[i] =
            dynamic_programming::palindrome_partitioning::pal_part(custom_input[i]);
    }

    // 预期分割次数结果：
    // "nitik" -> 2 ("n | iti | k")
    // "ababbbabbababa" -> 3 ("aba | b | bbabb | ababa")
    // "abdc" -> 3 ("a | b | d | c")
    std::vector<int> expected_output{2, 3, 3};

    for (int i = 0; i < 3; i++) {
        assert(expected_output[i] == calculated_output[i]);
    }

    std::cout << "All tests passed successfully!\n";
}

/**
 * @brief 主函数
 */
int main() {
    test();  // 运行自测
    return 0;
}
