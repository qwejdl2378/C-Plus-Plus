/**
 * @file
 * @brief Implementation of the [Wildcard Matching](https://www.geeksforgeeks.org/wildcard-pattern-matching/) problem. (通配符匹配记忆化回溯算法实现)
 * @details
 * 给定一个源字符串和一个包含通配符的模式串，实现通配符匹配。
 * 支持的通配符包括：
 * - `?`：匹配任意单个字符。
 * - `*`：匹配任意字符序列（包括空序列）。
 * 本算法使用记忆化回溯（带备忘录的动态规划）实现，以避免重复计算子问题。
 *
 * @author [Swastika Gupta](https://github.com/Swastyy)
 */

#include <cassert>   /// 用于 assert 断言
#include <cstdint>
#include <iostream>  /// 用于输入输出
#include <vector>    /// 用于 std::vector

/**
 * @namespace backtracking
 * @brief Backtracking algorithms
 */
namespace backtracking {
/**
 * @namespace wildcard_matching
 * @brief Functions for the [Wildcard
 * Matching](https://www.geeksforgeeks.org/wildcard-pattern-matching/) problem.
 */
namespace wildcard_matching {
/**
 * @brief 记忆化备忘录表 (DP table)
 * @details 大小为 1000x1000，初始化为 -1，代表该状态尚未计算
 */
std::vector<std::vector<int64_t>> dpTable(1000, std::vector<int64_t>(1000, -1));

/**
 * @brief 判断模式串 p 是否可以与源字符串 s 匹配的记忆化递归函数
 * @param s 源字符串
 * @param p 模式串
 * @param pos1 源字符串当前比对的字符索引
 * @param pos2 模式串当前比对的字符索引
 * @returns 1 if pattern matches with matching string otherwise 0
 */
bool wildcard_matching(std::string s, std::string p, uint32_t pos1,
                       uint32_t pos2) {
    uint32_t n = s.length();
    uint32_t m = p.length();
    // 递归出口 1：源字符串和模式串都正好比对结束，匹配成功
    if (pos1 == n && pos2 == m) {
        return true;
    }

    // 递归出口 2：模式串已用完，但源字符串尚未比对完，匹配失败
    if (pos1 != n && pos2 == m) {
        return false;
    }

    // 递归出口 3：源字符串已用完，但模式串尚未用完。
    // 此时模式串剩余字符必须全部都是 '*' 才能匹配空字符串，否则匹配失败。
    if (pos1 == n && pos2 != m) {
        while (pos2 < m && p[pos2] == '*') {
            pos2++;
        }

        return pos2 == m;
    }

    // 如果备忘录中已经计算过该状态，直接返回缓存结果
    if (dpTable[pos1][pos2] != -1) {
        return dpTable[pos1][pos2];
    }

    // 分支 1：如果当前位置的字符相等，则直接继续比对后面的字符
    if (s[pos1] == p[pos2]) {
        return dpTable[pos1][pos2] =
                   wildcard_matching(s, p, pos1 + 1, pos2 + 1);
    }

    else {
        // 分支 2：如果模式串当前是通配符 '?'，由于可以匹配任意单个字符，直接继续比对后面的字符
        if (p[pos2] == '?') {
            return dpTable[pos1][pos2] =
                       wildcard_matching(s, p, pos1 + 1, pos2 + 1);
        }
        // 分支 3：如果模式串当前是通配符 '*'，有两种可能选择：
        // 1. 将 '*' 匹配为空字符（不消耗源字符串字符，继续匹配模式串的下一个位置：pos2 + 1）
        // 2. 将 '*' 匹配为当前字符（消耗源字符串当前字符，模式串依然留在当前位置等待下一次继续匹配：pos1 + 1）
        else if (p[pos2] == '*') {
            return dpTable[pos1][pos2] =
                       wildcard_matching(s, p, pos1, pos2 + 1) ||
                       wildcard_matching(s, p, pos1 + 1, pos2);
        }
        // 分支 4：字符不相等且不为通配符，匹配失败
        else {
            return dpTable[pos1][pos2] = 0;
        }
    }
}

}  // namespace wildcard_matching
}  // namespace backtracking

/**
 * @brief Self-test implementations
 * @returns void
 */
static void test() {
    // 1st test
    std::cout << "1st test ";
    std::string matching1 = "baaabab";
    std::string pattern1 = "*****ba*****ab";
    assert(backtracking::wildcard_matching::wildcard_matching(matching1,
                                                              pattern1, 0, 0) ==
           1);  // here the pattern matches with given string
    std::cout << "passed" << std::endl;

    // 2nd test
    std::cout << "2nd test ";
    std::string matching2 = "baaabab";
    std::string pattern2 = "ba*****ab";
    assert(backtracking::wildcard_matching::wildcard_matching(matching2,
                                                              pattern2, 0, 0) ==
           1);  // here the pattern matches with given string
    std::cout << "passed" << std::endl;

    // 3rd test
    std::cout << "3rd test ";
    std::string matching3 = "baaabab";
    std::string pattern3 = "ba*ab";
    assert(backtracking::wildcard_matching::wildcard_matching(matching3,
                                                              pattern3, 0, 0) ==
           1);  // here the pattern matches with given string
    std::cout << "passed" << std::endl;

    // 4th test
    std::cout << "4th test ";
    std::string matching4 = "baaabab";
    std::string pattern4 = "a*ab";
    assert(backtracking::wildcard_matching::wildcard_matching(matching4,
                                                              pattern4, 0, 0) ==
           1);  // here the pattern matches with given string
    std::cout << "passed" << std::endl;

    // 5th test
    std::cout << "5th test ";
    std::string matching5 = "baaabab";
    std::string pattern5 = "aa?ab";
    assert(backtracking::wildcard_matching::wildcard_matching(matching5,
                                                              pattern5, 0, 0) ==
           1);  // here the pattern matches with given string
    std::cout << "passed" << std::endl;
}

/**
 * @brief Main function
 * @returns 0 on exit
 */
int main() {
    test();  // run self-test implementations
    return 0;
}
