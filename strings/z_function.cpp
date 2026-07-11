/**
 * @file
 * @brief Implementation of the [Z-Algorithm / Z-Function](https://cp-algorithms.com/string/z-function.html) for String Matching (Z 算法/Z 函数字符串检索实现)
 *
 * @details
 * Z 函数（Z-Function）对一个长度为 $N$ 的字符串 $S$，计算得到一个大小为 $N$ 的数组 `z`。
 * 其中 `z[i]` 表示以 $S[i]$ 开头的子串与 $S$ 的最长公共前缀（LCP, Longest Common Prefix）的长度。
 *
 * ### 算法原理（滑动匹配窗口 Z-Box）
 * 1. 维护一个区间 $[l, r]$（称为 Z-Box），表示当前已知的、与前缀匹配且右端点最靠右的子区间。
 * 2. 遍历到位置 `i` 时：
 *    - 如果 $i \le r$，利用先前计算的结果，`z[i]` 的初始值可以参考 `z[i - l]`，并被限制在当前边界内：`z[i] = min(r - i + 1, z[i - l])`。
 *    - 在此初始值基础上，继续向外扩展比对，直至失配。
 *    - 若更新后的右端点 $i + z[i] - 1 > r$，则将 Z-Box 的边界更新为：`l = i`，`r = i + z[i] - 1`。
 * 3. 匹配检索：
 *    - 将模式串 `pattern`、特殊字符 `#` 和文本串 `text` 拼接：`S = pattern + '#' + text`。
 *    - 计算整个拼接串的 Z 函数。
 *    - 在主串部分（即拼接串索引大于 `pattern_length` 的部分），若 `z[j] == pattern_length`，说明找到了一个完整匹配。
 *
 * 时间复杂度: $O(N + M)$
 * Space Complexity: $O(N + M)$
 *
 * @note
 * 【Z-Box 区间起点未更新导致退化为 $O(N^2)$ 的严重 Bug 审计与修复】：
 * 1. **优化完全失效（退化为暴力匹配）Bug**：原代码在更新边界时，仅写了：
 *    `if (i + z[i] - 1 > r) { r = i + z[i] - 1; }`
 *    **完全遗漏了更新 Z-Box 的起点位置 `l = i;`**！
 *    这导致 `l` 在整个循环运行期间始终保持为初始值 `0`。
 *    此时，计算 `z[i - l]` 实际上就是在计算 `z[i]` 本身（其初始为 0），使得 `z[i] = std::min(r - i + 1, z[i - l])` 永远为 `0`。
 *    整个算法退化为每次都必须从头重新比对字符的 $O(N^2)$ 暴力搜索，极易在超长文本中触发超时（TLE）。
 *    **修复**：在满足更新条件时，同时更新 `l` 与 `r`：
 *    `l = i; r = i + z[i] - 1;`
 *
 * @author [Ritika Gupta](https://github.com/RitikaGupta8734)
 */

#include <cstdint>   /// 用于 std::uint64_t
#include <iostream>  /// 用于标准输出
#include <string>    /// 用于 std::string
#include <vector>    /// 用于 std::vector
#include <cassert>   /// 用于 assert 断言
#include <algorithm> ///< 用于 std::min

namespace strings {
namespace z_function {

/**
 * @brief 计算给定字符串的 Z 函数数组
 * @param pattern 输入字符串
 * @return Z 数组
 */
std::vector<uint64_t> Z_function(const std::string &pattern) {
    uint64_t pattern_length = pattern.size();
    std::vector<uint64_t> z(pattern_length, 0);

    for (uint64_t i = 1, l = 0, r = 0; i < pattern_length; i++) {
        // 如果当前索引在 Z-box 右边界内，进行状态转移复用
        if (i <= r) {
            z[i] = std::min(r - i + 1, z[i - l]);
        }
        
        // 尝试向外暴力匹配扩展
        while (i + z[i] < pattern_length &&
               pattern[z[i]] == pattern[i + z[i]]) {
            z[i]++;
        }
        
        // 如果扩展后的右端点超过了当前的 Z-box 边界，更新 Z-box 的左右边界 [l, r]
        if (i + z[i] - 1 > r) {
            l = i;            // 核心修复：添加遗漏的 l = i，恢复 O(N) 线性时间复杂度
            r = i + z[i] - 1;
        }
    }
    return z;
}

/**
 * @brief 使用 Z 函数寻找模式串在文本中的所有匹配起始索引
 * @param pattern 待查找的模式串（不应包含字符 '#'）
 * @param text 被检索的文本串
 * @return 匹配成功的起始索引数组
 */
std::vector<uint64_t> find_pat_in_text(const std::string &pattern,
                                       const std::string &text) {
    uint64_t text_length = text.size(), pattern_length = pattern.size();
    if (pattern_length == 0) {
        // 如果模式串为空，返回主串的所有可能索引位置
        std::vector<uint64_t> matching_indexes(text_length);
        for (uint64_t i = 0; i < text_length; i++) {
            matching_indexes[i] = i;
        }
        return matching_indexes;
    }

    // 拼接字符串，以特殊字符 '#' 隔开
    std::vector<uint64_t> z = Z_function(pattern + '#' + text);
    std::vector<uint64_t> matching_indexes;

    // 扫描拼接串后部主串对应的 Z 值
    for (uint64_t i = 0; i < text_length; i++) {
        // 如果匹配前缀的长度等于模式串长度
        if (z[i + pattern_length + 1] == pattern_length) {
            matching_indexes.push_back(i);
        }
    }
    return matching_indexes;
}

}  // namespace z_function
}  // namespace strings

/**
 * @brief 单元自测用例
 */
static void test() {
    using namespace strings::z_function;

    // 正常匹配测试
    std::string text1 = "alskfjaldsabc1abc1abcbksbcdnsdabcabc";
    std::string pattern1 = "abc";
    std::vector<uint64_t> matching_indexes1 = find_pat_in_text(pattern1, text1);
    assert((matching_indexes1 == std::vector<uint64_t>{10, 14, 18, 30, 33}));

    // 匹配失败测试
    std::string text2 = "greengrass";
    std::string pattern2 = "abc";
    std::vector<uint64_t> matching_indexes2 = find_pat_in_text(pattern2, text2);
    assert((matching_indexes2 == std::vector<uint64_t>{}));

    // 空文本匹配测试
    std::string text3 = "";
    std::string pattern3 = "abc";
    std::vector<uint64_t> matching_indexes3 = find_pat_in_text(pattern3, text3);
    assert((matching_indexes3 == std::vector<uint64_t>{}));

    // 空模式串匹配测试
    std::string text4 = "redsand";
    std::string pattern4 = "";
    std::vector<uint64_t> matching_indexes4 = find_pat_in_text(pattern4, text4);
    assert((matching_indexes4 == std::vector<uint64_t>{0, 1, 2, 3, 4, 5, 6}));

    std::cout << "All Z-Function tests successfully passed!" << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    test();  // 运行自测
    return 0;
}
