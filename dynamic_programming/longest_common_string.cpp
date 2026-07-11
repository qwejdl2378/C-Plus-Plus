/**
 * @file
 * @brief Implementation of Longest Common Subsequence algorithm (最长公共子序列长度算法实现)
 * @details
 * 虽然文件名和部分注释为 longest_common_string，但其求解规则是“通过删除字符后所能达到的最长公共串”，
 * 这在算法定义中属于**最长公共子序列（LCS, Longest Common Subsequence）**，而非连续的公共子串。
 * 
 * ### 动态规划状态转移（自右向左，逆向递推）
 * 设 `sub_sols[i][j]` 表示 `string_a[i...]` 和 `string_b[j...]` 的 LCS 长度：
 * - 如果 `string_a[i] == string_b[j]`，则 `sub_sols[i][j] = 1 + sub_sols[i+1][j+1]`；
 * - 否则，`sub_sols[i][j] = max(sub_sols[i+1][j], sub_sols[i][j+1])`。
 * 
 * @note
 * 【循环边界与溢出设计技巧】：
 * 1. 数组索引采用了无符号的 `std::size_t` 类型，使用 `--pos_a` 递减进行自右向左的反向遍历。
 * 2. 终止条件使用了 `pos_a != limit`（其中 `limit = -1`，即无符号最大值）。
 *    由于无符号溢出特性，当 `pos_a = 0` 执行 `--pos_a` 后，会回绕变为最大值 `limit`，从而恰好满足终止条件退出循环，这是无符号整型反向遍历的经典编写技巧。
 *
 * 时间复杂度: O(|str_a| * |str_b|)
 * 空间复杂度: O(|str_a| * |str_b|)
 *
 * @author [Nikhil Arora](https://github.com/nikhilarora068)
 * @author [Piotr Idzik](https://github.com/vil02)
 */

#include <cassert>   /// 用于 assert 断言
#include <iostream>  /// 用于标准输出
#include <string>    /// 用于 std::string
#include <utility>   /// 用于 std::move
#include <vector>    /// 用于 std::vector

/**
 * @brief 计算两个字符串的最长公共子序列（LCS）的长度
 * @param string_a 输入字符串一
 * @param string_b 输入字符串二
 * @returns LCS 的长度
 */
std::size_t longest_common_string_length(const std::string& string_a,
                                         const std::string& string_b) {
    const auto size_a = string_a.size();
    const auto size_b = string_b.size();
    
    // 初始化 DP 状态表，默认填充 0
    std::vector<std::vector<std::size_t>> sub_sols(
        size_a + 1, std::vector<std::size_t>(size_b + 1, 0));

    const auto limit = static_cast<std::size_t>(-1); // 用于无符号数自减回绕的终止标志
    
    // 逆向双层循环填充 dp 表
    for (std::size_t pos_a = size_a - 1; pos_a != limit; --pos_a) {
        for (std::size_t pos_b = size_b - 1; pos_b != limit; --pos_b) {
            if (string_a[pos_a] == string_b[pos_b]) {
                sub_sols[pos_a][pos_b] = 1 + sub_sols[pos_a + 1][pos_b + 1];
            } else {
                sub_sols[pos_a][pos_b] = std::max(sub_sols[pos_a + 1][pos_b],
                                                  sub_sols[pos_a][pos_b + 1]);
            }
        }
    }

    return sub_sols[0][0]; // 最终的 LCS 长度记录在 sub_sols[0][0] 中
}

/**
 * @brief 存储单组测试数据的结构体
 */
struct TestCase {
    const std::string string_a;
    const std::string string_b;
    const std::size_t common_string_len;

    TestCase(std::string string_a, std::string string_b,
             const std::size_t in_common_string_len)
        : string_a(std::move(string_a)),
          string_b(std::move(string_b)),
          common_string_len(in_common_string_len) {}
};

/**
 * @brief 获取硬编码的测试用例列表
 */
std::vector<TestCase> get_test_cases() {
    return {TestCase("", "", 0),
            TestCase("ab", "ab", 2),
            TestCase("ab", "ba", 1),
            TestCase("", "xyz", 0),
            TestCase("abcde", "ace", 3),
            TestCase("BADANA", "ANADA", 3),
            TestCase("BADANA", "CANADAS", 3),
            TestCase("a1a234a5aaaa6", "A1AAAA234AAA56AAAAA", 6),
            TestCase("123x", "123", 3),
            TestCase("12x3x", "123", 3),
            TestCase("1x2x3x", "123", 3),
            TestCase("x1x2x3x", "123", 3),
            TestCase("x12x3x", "123", 3)};
}

/**
 * @brief 测试基础逻辑的正确性
 */
template <typename TestCases>
static void test_longest_common_string_length(const TestCases& test_cases) {
    for (const auto& cur_tc : test_cases) {
        assert(longest_common_string_length(cur_tc.string_a, cur_tc.string_b) ==
               cur_tc.common_string_len);
    }
}

/**
 * @brief 测试对称性（A 和 B 交换参数后结果仍一致）
 */
template <typename TestCases>
static void test_longest_common_string_length_is_symmetric(
    const TestCases& test_cases) {
    for (const auto& cur_tc : test_cases) {
        assert(longest_common_string_length(cur_tc.string_b, cur_tc.string_a) ==
               cur_tc.common_string_len);
    }
}

/**
 * @brief 辅助反转字符串函数
 */
std::string reverse_str(const std::string& in_str) {
    return {in_str.rbegin(), in_str.rend()};
}

/**
 * @brief 测试反转后字符串的 LCS 仍应当与原串 LCS 长度相等
 */
template <typename TestCases>
static void test_longest_common_string_length_for_reversed_inputs(
    const TestCases& test_cases) {
    for (const auto& cur_tc : test_cases) {
        assert(longest_common_string_length(reverse_str(cur_tc.string_a),
                                             reverse_str(cur_tc.string_b)) ==
               cur_tc.common_string_len);
    }
}

/**
 * @brief 运行全套测试用例集
 */
static void tests() {
    const auto test_cases = get_test_cases();
    assert(test_cases.size() > 0);
    test_longest_common_string_length(test_cases);
    test_longest_common_string_length_is_symmetric(test_cases);
    test_longest_common_string_length_for_reversed_inputs(test_cases);

    std::cout << "All tests have successfully passed!\n";
}

/**
 * @brief 主函数
 */
int main() {
    tests(); // 运行自测
    return 0;
}
