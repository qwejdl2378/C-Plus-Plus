/**
 * @file
 * @brief Program to find the [Longest Palindromic Subsequence](https://www.geeksforgeeks.org/longest-palindromic-subsequence-dp-12/) of a string (最长回文子序列求解算法实现)
 *
 * @details
 * 回文（Palindrome）指正读反读都完全相同的字符串（例如 "radar"）。
 * 子序列（Subsequence）是通过删除源串中零个或多个字符且不改变剩余字符相对顺序所得到的新串。
 *
 * ### 算法原理
 * 1. 一个字符串 a 的最长回文子序列（LPS），等价于**字符串 a 与其反转字符串 b 之间的最长公共子序列（LCS）**。
 * 2. 构造反转字符串：`b = string(a.rbegin(), a.rend())`。
 * 3. 使用自底向上的动态规划求解 a 和 b 的 LCS 长度，并将最优解路径保存在 `res` 表中。
 * 4. 从右下角 `res[m][m]` 开始反向回溯，构造出具体的 LCS 字符串（即 LPS）。
 *
 * 时间复杂度: O(N^2)
 * 空间复杂度: O(N^2)
 * 
 * @author [Anjali Jha](https://github.com/anjali1903)
 */

#include <cassert>  /// 用于 assert 断言
#include <string>   /// 用于 std::string
#include <vector>   /// 用于 std::vector

/**
 * @namespace dynamic_programming
 * @brief 动态规划算法命名空间
 */
namespace dynamic_programming {
/**
 * @brief 求字符串的最长回文子序列并返回具体的子序列内容
 * @param a 输入的字符串
 * @returns 对应最长回文子序列的字符串
 */
std::string lps(const std::string& a) {
    const auto b = std::string(a.rbegin(), a.rend()); // 构造反转字符串
    const auto m = a.length();
    using ind_type = std::string::size_type;
    std::vector<std::vector<ind_type> > res(m + 1, std::vector<ind_type>(m + 1));

    // 自底向上计算 LCS 长度存入 res 表中
    for (ind_type i = 0; i <= m; i++) {
        for (ind_type j = 0; j <= m; j++) {
            if (i == 0 || j == 0) {
                res[i][j] = 0;
            } else if (a[i - 1] == b[j - 1]) {
                res[i][j] = res[i - 1][j - 1] + 1;
            } else {
                res[i][j] = std::max(res[i - 1][j], res[i][j - 1]);
            }
        }
    }
    
    // 获取最长子序列长度
    auto idx = res[m][m];
    std::string ans(idx, '\0'); // 创建对应长度的空字符集
    ind_type i = m, j = m;

    // 从右下角开始反向回溯构造 LCS
    while (i > 0 && j > 0) {
        // 字符相同，说明是 LCS 的一部分
        if (a[i - 1] == b[j - 1]) {
            ans[idx - 1] = a[i - 1];
            i--;
            j--;
            idx--;
        }
        // 字符不同，向数值较大的子状态方向回溯
        else if (res[i - 1][j] > res[i][j - 1]) {
            i--;
        } else {
            j--;
        }
    }

    return ans;
}
}  // namespace dynamic_programming

/**
 * @brief 单元自测用例
 */
static void test() {
    assert(dynamic_programming::lps("radar") == "radar");
    assert(dynamic_programming::lps("abbcbaa") == "abcba");
    assert(dynamic_programming::lps("bbbab") == "bbbb");
    assert(dynamic_programming::lps("") == "");
}

/**
 * @brief 主函数
 */
int main() {
    test();  // 运行自测
    return 0;
}
