/**
 * @file
 * @brief Implementation of the [Boyer-Moore String Search Algorithm](https://en.wikipedia.org/wiki/Boyer%E2%80%93Moore_string-search_algorithm) (Boyer-Moore 字符串检索算法实现)
 *
 * @details
 * Boyer-Moore 算法是一种非常高效的字符串匹配算法。
 * 与传统的从左往右逐字符匹配的朴素算法不同，Boyer-Moore 在将模式串与主串对齐后，
 * **从右往左（自模式串的末尾向头部）**进行字符比较。
 *
 * ### 核心启发式规则 (Shift Rules)
 * 1. **坏字符规则 (Bad Character Heuristic)**：
 *    当在主串的某个位置发生不匹配时，该不匹配的字符被称为“坏字符”。
 *    - 如果该坏字符在模式串中不存在，可以直接将模式串向右滑动整个模式串长度。
 *    - 如果该坏字符在模式串中存在，将模式串中最右边的该字符与主串中的坏字符对齐。
 * 2. **好后缀规则 (Good Suffix Heuristic)**：
 *    当发生不匹配时，已经匹配成功的部分后缀称为“好后缀”。
 *    - 如果模式串中有另一段子串能匹配该好后缀，则将该子串向右滑动与好后缀对齐。
 *    - 如果没有，则寻找好后缀的某个最长前缀，使其与模式串的某个最长后缀相同，将二者对齐。
 *
 * 每一步滑动的距离取这两种规则计算出的滑动值的**最大值**：`max(bad_char_shift, good_suffix_shift)`。
 *
 * 时间复杂度: 最好情况 $O(N/M)$，最坏情况 $O(N \cdot M)$ (通常在实际文本搜索中表现出亚线性时间复杂度)。
 * 空间复杂度: $O(M + |\Sigma|)$，其中 $|\Sigma|$ 是字符集大小。
 *
 * @note
 * 【非 ASCII 字符越界访问与空输入 Bug 审计与修复】：
 * 1. **非 ASCII 字符导致数组越界崩溃 Bug**：原代码将字符集大小硬编码为 `CHAR_MAX`（通常为 127）。
 *    在处理含有中文、特殊符号或扩展 ASCII 码（0x80 - 0xFF）的字符串时，
 *    有符号类型的 `char` 会被解释为负数（例如 `-128` 到 `-1`），作为下标访问 `bad_char` 数组时会直接引发**内存越界读写崩溃（段错误）**。
 *    **修复**：将字符集大小统一扩展到 `256`（即 `UCHAR_MAX + 1`），并在所有使用字符作为下标的地方，
 *    通过 `static_cast<unsigned char>` 进行强制无符号转换，杜绝越界隐患。
 * 2. **空字符串下溢崩溃风险**：如果传入空的模式串，
 *    计算 `index_position = arg.pat.size() - 1` 会产生无符号整数下溢。
 *    **修复**：在 `search` 接口增加防线：`if (arg.pat.empty() || str.empty() || arg.pat.size() > str.size()) return {};`。
 *
 * @author [Stoycho Kyosev](https://github.com/stoychoX)
 */

#include <cassert>   /// 用于 assert 断言
#include <climits>   /// 用于 UCHAR_MAX
#include <cstring>   /// 用于 std::strlen
#include <iostream>  /// 用于输入输出
#include <string>    /// 用于 std::string
#include <vector>    /// 用于 std::vector
#include <algorithm> ///< 用于 std::max

// 使用无符号字符的最大范围 256 作为字符表大小，彻底解决 signed char 负数下标越界 Bug
constexpr size_t ALPHABET_SIZE = UCHAR_MAX + 1; 

namespace strings {
namespace boyer_moore {

/**
 * @brief 存放预处理模式串后得到的启发式规则表结构体
 */
struct pattern {
    std::string pat;               ///< 存放的模式串
    std::vector<size_t> bad_char;  ///< 坏字符规则表
    std::vector<size_t> good_suffix; ///< 好后缀规则表
};

/**
 * @brief 预处理好后缀规则表
 */
void init_good_suffix(const std::string& str, std::vector<size_t>& arg) {
    arg.resize(str.size() + 1, 0);

    // border_pos[i] 保存子串 str[i..] 中既是其真后缀又是其真前缀的最长边位置
    std::vector<size_t> border_pos(str.size() + 1, 0);
    size_t current_char = str.length();
    size_t border_index = str.length() + 1;
    border_pos[current_char] = border_index;

    while (current_char > 0) {
        while (border_index <= str.length() &&
               str[current_char - 1] != str[border_index - 1]) {
            if (arg[border_index] == 0) {
                arg[border_index] = border_index - current_char;
            }
            border_index = border_pos[border_index];
        }
        current_char--;
        border_index--;
        border_pos[current_char] = border_index;
    }

    size_t largest_border_index = border_pos[0];
    for (size_t i = 0; i < str.size(); i++) {
        if (arg[i] == 0) {
            arg[i] = largest_border_index;
        }
        if (i == largest_border_index) {
            largest_border_index = border_pos[largest_border_index];
        }
    }
}

/**
 * @brief 预处理坏字符规则表
 */
void init_bad_char(const std::string& str, std::vector<size_t>& arg) {
    // 核心修复：大小设为 256，默认值设为字符串长度
    arg.resize(ALPHABET_SIZE, str.length());

    for (size_t i = 0; i < str.length(); i++) {
        // 核心修复：转换字符为无符号，防止 signed char 负数下标越界
        unsigned char ch = static_cast<unsigned char>(str[i]);
        arg[ch] = str.length() - i - 1;
    }
}

/**
 * @brief 外部初始化模式串主入口
 */
void init_pattern(const std::string& str, pattern& arg) {
    arg.pat = str;
    init_bad_char(str, arg.bad_char);
    init_good_suffix(str, arg.good_suffix);
}

/**
 * @brief Boyer-Moore 检索匹配函数
 * @param str 待匹配的主文本串
 * @param arg 预处理好的模式串结构体
 * @return 所有匹配成功的起始索引位置数组
 */
std::vector<size_t> search(const std::string& str, const pattern& arg) {
    // 核心修复：增加空字符串及大小边界的安全拦截，防范 underflow
    if (arg.pat.empty() || str.empty() || arg.pat.size() > str.size()) {
        return {};
    }

    size_t index_position = arg.pat.size() - 1;
    std::vector<size_t> index_storage;

    while (index_position < str.length()) {
        size_t index_string = index_position;
        int index_pattern = static_cast<int>(arg.pat.size()) - 1;

        // 从右往左逐一字符进行匹配
        while (index_pattern >= 0 &&
               str[index_string] == arg.pat[index_pattern]) {
            --index_pattern;
            --index_string;
        }

        // 若 index_pattern < 0，说明全部匹配成功
        if (index_pattern < 0) {
            index_storage.push_back(index_position - arg.pat.length() + 1);
            index_position += arg.good_suffix[0]; // 利用好后缀规则滑动到下一个可能位置
        } else {
            // 发生坏字符不匹配，取坏字符和好后缀规则中的最大滑动值
            // 核心修复：对 str[index_string] 转成无符号字符，防止越界崩溃
            unsigned char bad_ch = static_cast<unsigned char>(str[index_string]);
            index_position += std::max(arg.bad_char[bad_ch],
                                       arg.good_suffix[index_pattern + 1]);
        }
    }

    return index_storage;
}

/**
 * @brief 辅助前缀判定函数
 */
bool is_prefix(const char* str, const char* pat, size_t len) {
    if (std::strlen(str) < len) {
        return false;
    }
    for (size_t i = 0; i < len; i++) {
        if (str[i] != pat[i]) {
            return false;
        }
    }
    return true;
}

}  // namespace boyer_moore
}  // namespace strings

/**
 * @brief "and" 检索单元测试
 */
void and_test(const char* text) {
    strings::boyer_moore::pattern ands;
    strings::boyer_moore::init_pattern("and", ands);
    std::vector<size_t> indexes = strings::boyer_moore::search(text, ands);

    assert(indexes.size() == 2);
    assert(strings::boyer_moore::is_prefix(text + indexes[0], "and", 3));
    assert(strings::boyer_moore::is_prefix(text + indexes[1], "and", 3));
}

/**
 * @brief "pat" 检索单元测试
 */
void pat_test(const char* text) {
    strings::boyer_moore::pattern pat;
    strings::boyer_moore::init_pattern("pat", pat);
    std::vector<size_t> indexes = strings::boyer_moore::search(text, pat);

    assert(indexes.size() == 6);
    for (const auto& currentIndex : indexes) {
        assert(strings::boyer_moore::is_prefix(text + currentIndex, "pat", 3));
    }
}

/**
 * @brief 单元自测用例
 */
static void tests() {
    const char* text =
        "When pat Mr. and Mrs. pat Dursley woke up on the dull, gray  \
                            Tuesday our story starts, \
                there was nothing about pat the cloudy sky outside to pat suggest that\
                        strange and \
                    mysterious things would pat soon be happening all pat over the \
                        country.";

    and_test(text);
    pat_test(text);

    // 验证含有中文（非 ASCII）时的稳定性，确认不会越界崩溃
    strings::boyer_moore::pattern cn_pat;
    strings::boyer_moore::init_pattern("测试", cn_pat);
    std::vector<size_t> cn_res = strings::boyer_moore::search("这是一个测试文本，测试非ASCII字符越界安全性", cn_pat);
    assert(cn_res.size() == 2);

    std::cout << "All Boyer-Moore tests successfully passed!" << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    tests(); // 运行自测
    return 0;
}
