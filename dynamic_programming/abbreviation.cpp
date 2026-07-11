/**
 * @file
 * @brief Implementation of [Abbreviation](https://www.hackerrank.com/challenges/abbr/problem) (字符串缩写匹配算法实现)
 *
 * @details
 * 给定两个字符串 a 和 b，判断是否可以通过以下操作将 a 转换为 b：
 * 1. 将 a 中的零个或多个小写字母转换为大写字母。
 * 2. 删除 a 中剩余的所有小写字母。
 *
 * ### 动态规划状态转移
 * 设 `dp[i][j]` 表示 a 的前 i 个字符是否能匹配 b 的前 j 个字符：
 * 1. 如果 `a[i-1] == b[j-1]`（完全相同），则 `dp[i][j] = dp[i-1][j-1]`。
 * 2. 如果 `a[i-1]` 是小写字母且转换为大写后等于 `b[j-1]`，则有两种选择：
 *    - 转换为大写进行匹配：`dp[i-1][j-1]`
 *    - 作为小写字母直接删除：`dp[i-1][j]`
 *    只要有一者为真即可：`dp[i][j] = dp[i-1][j-1] || dp[i-1][j]`
 * 3. 其他情况：
 *    - 如果 `a[i-1]` 是大写字母，由于不能删除，必然匹配失败，`dp[i][j] = false`。
 *    - 如果 `a[i-1]` 是小写字母，只能选择删除，`dp[i][j] = dp[i-1][j]`。
 *
 * @note
 * 【重要 Bug 说明与比对】：
 * 在原作者的迭代法 `abbreviation`（第 123-125 行）中，初始化 `memo[i][0] = true` 是有缺陷的！
 * 如果 a = "A"（大写），b = ""（空串），由于大写字母 'A' 无法被删除，正确结果应为 `false`。
 * 但因为初始化时直接将 `memo[i][0]` 全部设为 `true`，导致迭代法会错误地返回 `true`。
 * 而递归法中对大写字母无法删除进行了正确处理，因此递归法会正确返回 `false`。
 * 修正方法：迭代法中 `memo[i][0]` 应当由 `memo[i-1][0] && islower(str[i-1])` 递推决定。
 *
 * 时间复杂度: O(|a| * |b|)
 * 空间复杂度: O(|a| * |b|)
 *
 * @author [Ashish Daulatabad](https://github.com/AshishYUO)
 */

#include <cassert>   /// 用于 assert 断言
#include <cstdint>   /// 用于 std::uint32_t
#include <iostream>  /// 用于输入输出
#include <string>    /// 用于 std::string
#include <vector>    /// 用于 std::vector

/**
 * @namespace dynamic_programming
 * @brief 动态规划算法命名空间
 */
namespace dynamic_programming {
/**
 * @namespace abbreviation
 * @brief 缩写匹配相关命名空间
 */
namespace abbreviation {
/**
 * @brief 记忆化递归动态规划实现
 * @param memo 记忆化解空间表
 * @param visited 状态访问记录表
 * @param str 源字符串 a
 * @param result 目标大写字符串 b
 * @param str_idx 当前处理源串的索引
 * @param result_idx 当前处理目标串的索引
 * @returns `true` 可以成功转换；`false` 无法转换
 */
bool abbreviation_recursion(std::vector<std::vector<bool>> *memo,
                            std::vector<std::vector<bool>> *visited,
                            const std::string &str, const std::string &result,
                            uint32_t str_idx = 0, uint32_t result_idx = 0) {
    bool ans = memo->at(str_idx).at(result_idx);
    if (str_idx == str.size() && result_idx == result.size()) {
        return true;
    } else if (str_idx == str.size() && result_idx != result.size()) {
        return false;
    } else if (!visited->at(str_idx).at(result_idx)) {
        if (str[str_idx] == result[result_idx]) {
            // 当前字符相同，继续比对下一个
            ans = abbreviation_recursion(memo, visited, str, result,
                                         str_idx + 1, result_idx + 1);
        } else if (str[str_idx] - 32 == result[result_idx]) {
            // 当前字符是小写，转大写后与目标相同。分支：匹配该大写字符 或 扔掉该小写字符
            ans = abbreviation_recursion(memo, visited, str, result,
                                         str_idx + 1, result_idx + 1) ||
                  abbreviation_recursion(memo, visited, str, result,
                                         str_idx + 1, result_idx);
        } else {
            // 字符不匹配。如果是大写则转换失败；如果是小写则只能丢弃
            if (str[str_idx] >= 'A' && str[str_idx] <= 'Z') {
                ans = false;
            } else {
                ans = abbreviation_recursion(memo, visited, str, result,
                                             str_idx + 1, result_idx);
            }
        }
    }
    (*memo)[str_idx][result_idx] = ans;
    (*visited)[str_idx][result_idx] = true;
    return (*memo)[str_idx][result_idx];
}

/**
 * @brief 迭代动态规划实现
 * @param str 源字符串 a
 * @param result 目标大写字符串 b
 * @returns `true` 可以成功转换；`false` 无法转换
 */
bool abbreviation(const std::string &str, const std::string &result) {
    std::vector<std::vector<bool>> memo(
        str.size() + 1, std::vector<bool>(result.size() + 1, false));

    // 警告：这里直接赋值 memo[i][0] = true 存在逻辑漏洞
    for (uint32_t i = 0; i <= str.size(); ++i) {
        memo[i][0] = true;
    }
    for (uint32_t i = 1; i <= result.size(); ++i) {
        memo[0][i] = false;
    }
    for (uint32_t i = 1; i <= str.size(); ++i) {
        for (uint32_t j = 1; j <= result.size(); ++j) {
            if (str[i - 1] == result[j - 1]) {
                memo[i][j] = memo[i - 1][j - 1];
            } else if (str[i - 1] - 32 == result[j - 1]) {
                memo[i][j] = (memo[i - 1][j - 1] || memo[i - 1][j]);
            } else {
                if (str[i - 1] >= 'A' && str[i - 1] <= 'Z') {
                    memo[i][j] = false;
                } else {
                    memo[i][j] = memo[i - 1][j];
                }
            }
        }
    }
    return memo.back().back();
}
}  // namespace abbreviation
}  // namespace dynamic_programming

/**
 * @brief 单元自测用例
 */
static void test() {
    std::string s = "daBcd", t = "ABC";
    std::vector<std::vector<bool>> memo(s.size() + 1,
                                         std::vector<bool>(t.size() + 1, false)),
        visited(s.size() + 1, std::vector<bool>(t.size() + 1, false));

    assert(dynamic_programming::abbreviation::abbreviation_recursion(
               &memo, &visited, s, t) == true);
    assert(dynamic_programming::abbreviation::abbreviation(s, t) == true);
    
    s = "XXVVnDEFYgYeMXzWINQYHAQKKOZEYgSRCzLZAmUYGUGILjMDET";
    t = "XXVVDEFYYMXWINQYHAQKKOZEYSRCLZAUYGUGILMDETQVWU";
    memo = std::vector<std::vector<bool>>(
        s.size() + 1, std::vector<bool>(t.size() + 1, false));
    visited = std::vector<std::vector<bool>>(
        s.size() + 1, std::vector<bool>(t.size() + 1, false));

    assert(dynamic_programming::abbreviation::abbreviation_recursion(
               &memo, &visited, s, t) == false);
    assert(dynamic_programming::abbreviation::abbreviation(s, t) == false);

    s = "DRFNLZZVHLPZWIupjwdmqafmgkg";
    t = "DRFNLZZVHLPZWI";
    memo = std::vector<std::vector<bool>>(
        s.size() + 1, std::vector<bool>(t.size() + 1, false));
    visited = std::vector<std::vector<bool>>(
        s.size() + 1, std::vector<bool>(t.size() + 1, false));

    assert(dynamic_programming::abbreviation::abbreviation_recursion(
               &memo, &visited, s, t) == true);
    assert(dynamic_programming::abbreviation::abbreviation(s, t) == true);
}

/**
 * @brief 主函数
 * @returns 0
 */
int main() {
    test();  // 运行自测
    return 0;
}
