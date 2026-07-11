/**
 * @file
 * @brief Implementation of the [Word Break Problem](https://leetcode.com/problems/word-break/) (单词拆分问题算法实现)
 *
 * @details
 * 单词拆分问题：给定一个非空字符串 s 和一个包含非空单词列表的字典 `wordDict`，
 * 判断是否可以将 s 拆分为一个或多个在字典中出现的单词序列。
 *
 * 字典中的单词允许被重复使用多次，且字典中没有重复的单词。
 *
 * ### 动态规划状态转移（记忆化递归自顶向下）
 * 设递归函数 `check(pos)` 表示从字符串 s 的第 `pos` 个索引位置开始，剩余子串是否能被合法地拆分：
 * - 边界条件：若 `pos == s.length()`，说明整串已被成功拆解匹配完毕，返回 `true`。
 * - 缓存机制：`dp[pos]` 存储该位置是否已探过路，`1` 代表成功，`0` 代表失败，`INT_MAX` 代表未计算。
 * - 状态转移：从 `pos` 开始向后枚举所有可能的拆分长度：
 *   若前缀 `s[pos...i]` 在字典中存在，且子问题 `check(i + 1)` 也返回 `true`：
 *   则当前状态为真，记录并返回 `true`。
 *   若枚举完所有前缀后均无解，则记录 `dp[pos] = 0` 并返回 `false`。
 *
 * 时间复杂度: O(N^2 * L)，其中 N 为字符串长度，L 为单词的最大长度。
 * 空间复杂度: O(N)
 *
 * @author [Akshay Anand] (https://github.com/axayjha)
 */

#include <cassert>
#include <climits>
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

/**
 * @namespace dynamic_programming
 * @brief 动态规划算法命名空间
 */
namespace dynamic_programming {
/**
 * @namespace word_break
 * @brief 单词拆分相关算法命名空间
 */
namespace word_break {
/**
 * @brief 判断单词是否在字典中存在
 * @param str 待查询的单词串
 * @param strSet 字典集合
 * @returns 是否存在于字典中
 */
bool exists(const std::string &str,
            const std::unordered_set<std::string> &strSet) {
    return strSet.find(str) != strSet.end();
}

/**
 * @brief 记忆化递归检测拆分可能性
 * @param s 原始字符串
 * @param strSet 字典哈希表（提供 O(1) 查询）
 * @param pos 当前处理的起始位置
 * @param dp 记忆化状态向量指针
 * @returns 能否从当前位置合法拆分到末尾
 */
bool check(const std::string &s, const std::unordered_set<std::string> &strSet,
           int pos, std::vector<int> *dp) {
    // 已经成功完成对整个字符串的拆分
    if (pos == s.length()) {
        return true;
    }

    // 命中缓存则直接返回结果
    if (dp->at(pos) != INT_MAX) {
        return dp->at(pos) == 1;
    }

    std::string wordTillNow = "";  // 用于累积记录当前位置开始的前缀字符串

    // 循环扫描各种前缀长度
    for (int i = pos; i < s.length(); i++) {
        wordTillNow += s[i]; // 追加当前字符

        // 前缀存在且剩余子串可以被合法拆分
        if (exists(wordTillNow, strSet) && check(s, strSet, i + 1, dp)) {
            dp->at(pos) = 1; // 记录为可以成功拆分
            return true;
        }
    }
    
    dp->at(pos) = 0; // 记录该位置无法完成后续拆分
    return false;
}

/**
 * @brief 单词拆分封装入口函数
 * @param s 输入字符串
 * @param wordDict 字典列表
 * @returns 是否能完成拆分
 */
bool wordBreak(const std::string &s, const std::vector<std::string> &wordDict) {
    // 将 vector 单词表转化为无序哈希集合以提供常数级 O(1) 查询效率
    std::unordered_set<std::string> strSet;
    for (const auto &s : wordDict) {
        strSet.insert(s);
    }
    
    // 初始化记忆化向量 dp，大小为字符串长度，值默认为 INT_MAX (代表未处理过)
    std::vector<int> dp(s.length(), INT_MAX);

    // 从 0 号索引位置开始检验
    return check(s, strSet, 0, &dp);
}

}  // namespace word_break
}  // namespace dynamic_programming

/**
 * @brief 单元自测用例
 */
static void test() {
    const std::string s = "applepenapple";
    const std::vector<std::string> wordDict = {"apple", "pen"};

    // 应该返回 true，因为可以分割为 "apple" + "pen" + "apple"
    assert(dynamic_programming::word_break::wordBreak(s, wordDict));

    std::cout << "Test implementation passed!\n";
}

/**
 * @brief 主函数
 */
int main() {
    test();  // 运行自测

    const std::string s = "applepenapple";
    const std::vector<std::string> wordDict = {"apple", "pen"};

    std::cout << dynamic_programming::word_break::wordBreak(s, wordDict)
              << std::endl;
    return 0;
}
