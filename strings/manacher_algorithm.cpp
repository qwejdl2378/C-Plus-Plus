/**
 * @file
 * @brief Implementation of [Manacher's Algorithm](https://en.wikipedia.org/wiki/Longest_palindromic_substring) (马拉车算法/Manacher 算法最长回文子串实现)
 *
 * @details
 * Manacher 算法（俗称“马拉车算法”）是一种可以在 $O(N)$ 线性时间内求解一个字符串的**最长回文子串（Longest Palindromic Substring）**的经典算法。
 *
 * ### 核心设计思想
 * 1. **字符插空（Stuffing）**：
 *    - 经典回文子串有奇数长度（如 "aba"，中心是 'b'）和偶数长度（如 "abba"，中心是 'b' 和 'b' 之间）之分，这导致状态转移非常繁琐。
 *    - 马拉车算法通过在每个字符前后插入一个特殊分隔符（如 `#`），并在首尾放置边界符（如 `@` 和 `&`），
 *      将所有字符串统一转化为**奇数长度的增广字符串**（例如 "aba" -> "@#a#b#a#&"，"abba" -> "@#a#b#b#a#&"）。
 * 2. **回文对称性与动态规划优化**：
 *    - 维护当前已知的“右边界最远”的回文子串，其中心为 `bigger_center`，右端点为 `right`。
 *    - 遍历到位置 `i` 时，如果 `i` 在右边界 `right` 内部，根据对称性，其关于 `bigger_center` 的对称点 `opposite_to_i` 的回文半径值可以直接拿来参考。
 *      `palindrome_max_half_length[i] = min(palindrome_max_half_length[opposite_to_i], right - i)`。
 *    - 从该参考半径开始继续向外暴力匹配，大幅减少了重复比对的次数。
 *
 * 时间复杂度: $O(N)$
 * 空间复杂度: $O(N)$ (用于存储增广后的字符串和回文半径数组)
 *
 * @note
 * 【特殊符号使用约束审计】：
 * 1. 本算法使用 `@`（头部边界符）、`#`（插空分隔符）、`&`（尾部边界符）作为预处理辅助字符。
 *    因此，输入的原始字符串 `prototype` 中不应当含有这三个特殊字符，否则会与辅助字符混淆，导致匹配结果错误。
 * 2. **无符号整型下溢审计**：在计算对称点 `opposite_to_i = 2 * bigger_center - i` 时，
 *    由于采用无符号数 `uint64_t`，必须保证 $2 \cdot \text{bigger\_center} \ge i$。
 *    根据算法性质，`i` 只有在 $i < right$（即 $i$ 在当前回文内部）时才参与对称，
 *    而回文左边界 $\text{bigger\_center} - L \ge 0$，因此对称点必然 $\ge 0$，无符号下溢在数学上被严格杜绝。
 *
 * @author [Riti Kumari](https://github.com/riti2409)
 */

#include <cassert>   /// 用于 assert 断言
#include <cstdint>   /// 用于 std::uint64_t
#include <iostream>  /// 用于标准输入输出
#include <string>    /// 用于 std::string
#include <vector>    /// 用于 std::vector
#include <algorithm> ///< 用于 std::min

namespace strings {
namespace manacher {

/**
 * @brief 使用 Manacher 算法求解最长回文子串
 * @param prototype 输入的原始字符串（不应包含 '@', '#', '&'）
 * @return 找到的最长回文子串
 */
std::string manacher(const std::string &prototype) {
    if (prototype.empty()) {
        return "";
    }

    // 1. 字符插空预处理，将所有偶数长度和奇数长度回文统一转化为奇数长度
    std::string stuffed_string = "";
    for (char str : prototype) {
        stuffed_string += str;
        stuffed_string += "#";
    }
    // 添加首尾哨兵，防止越界判断
    stuffed_string = "@#" + stuffed_string + "&";

    // 记录以 stuffed_string 各个字符为中心的最长回文半长度
    std::vector<uint64_t> palindrome_max_half_length(stuffed_string.size(), 0);

    uint64_t bigger_center = 0; // 当前最右回文的中心点
    uint64_t right = 0;         // 当前最右回文的右边界端点

    for (uint64_t i = 1; i < stuffed_string.size() - 1; i++) {
        // 如果当前点 i 落在右边界内，利用回文对称性快速获取初始半径
        if (i < right) {
            uint64_t opposite_to_i = 2 * bigger_center - i; // 对称点，无溢出风险
            palindrome_max_half_length[i] = std::min(
                palindrome_max_half_length[opposite_to_i], right - i);
        }

        // 尝试以 i 为中心继续向两边扩展比对字符
        while (stuffed_string[i + (palindrome_max_half_length[i] + 1)] ==
               stuffed_string[i - (palindrome_max_half_length[i] + 1)]) {
            palindrome_max_half_length[i]++;
        }

        // 如果新扩展的回文右端点超过了已知的最右端点，更新中心和右边界
        if (i + palindrome_max_half_length[i] > right) {
            bigger_center = i;
            right = i + palindrome_max_half_length[i];
        }
    }

    // 2. 扫描半径数组，寻找全局最大回文半长度及其中心位置
    uint64_t half_length = 0;
    uint64_t center_index = 0;

    for (uint64_t i = 1; i < stuffed_string.size() - 1; i++) {
        if (palindrome_max_half_length[i] > half_length) {
            half_length = palindrome_max_half_length[i];
            center_index = i;
        }
    }

    std::string palindromic_substring = "";

    if (half_length > 0) {
        // 核心性质：在增广字符串中，最长回文子串的起始物理位置 center_index - half_length + 1
        // 必然是一个偶数索引，从而我们在提取时跳过插入的 '#'
        uint64_t start = center_index - half_length + 1;
        uint64_t end = center_index + half_length - 1;
        for (uint64_t index = start; index <= end; index += 2) {
            palindromic_substring += stuffed_string[index];
        }
    } else {
        // 如果最大半长度为 0，说明没有任何大于 1 的回文，直接取首字符作为回文子串
        palindromic_substring = prototype[0];
    }
    return palindromic_substring;
}

}  // namespace manacher
}  // namespace strings

/**
 * @brief 单元自测用例
 */
static void test() {
    assert(strings::manacher::manacher("") == "");
    assert(strings::manacher::manacher("abababc") == "ababa");
    assert(strings::manacher::manacher("cbaabd") == "baab");
    assert(strings::manacher::manacher("DedzefDeD") == "DeD");
    assert(strings::manacher::manacher("XZYYXXYZXX") == "YXXY");
    assert(strings::manacher::manacher("1sm222m10abc") == "m222m");
    assert(strings::manacher::manacher("798989591") == "98989");
    assert(strings::manacher::manacher("xacdedcax") == "xacdedcax");
    assert(strings::manacher::manacher("xaccax") == "xaccax");
    assert(strings::manacher::manacher("a") == "a");
    assert(strings::manacher::manacher("xy") == "x");
    assert(strings::manacher::manacher("abced") == "a");

    std::cout << "All Manacher algorithm tests successfully passed!" << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    test();  // 运行测试用例
    return 0;
}
