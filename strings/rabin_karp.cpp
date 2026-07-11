/**
 * @file
 * @brief Implementation of the [Rabin-Karp String Search Algorithm](https://en.wikipedia.org/wiki/Rabin%E2%80%93Karp_algorithm) (Rabin-Karp 字符串哈希匹配算法实现)
 *
 * @details
 * Rabin-Karp 算法是一种基于**哈希（Hashing）**的字符串匹配算法。
 * 它通过滑动窗口计算主串子串的哈希值，并与模式串的哈希值进行比较。
 *
 * ### 核心计算机制
 * 1. **哈希函数（多项式滚动作业）**：
 *    - 采用多项式哈希值计算：$Hash(S) = \sum S[i] \cdot B^i \pmod M$ （这里选择基数 `PRIME = 5`，无大质数取模，利用 `int64_t` 自然溢出或近似求和）。
 * 2. **滚动哈希（Rolling Hash, $O(1)$ 更新）**：
 *    - 当窗口向右移动一位，原首字符 `S[old]` 移出，新尾字符 `S[new]` 移入。
 *    - 更新公式为：$NewHash = (OldHash - S[old]) / B + S[new] \cdot B^{M-1}$。
 * 3. **二次核对（防止哈希冲突）**：
 *    - 当哈希值相同时，由于可能存在哈希冲突（Hash Collision），需要对子串进行逐字符的绝对比对（`check_if_equal`），以确保匹配的正确性。
 *
 * 时间复杂度: 平均情况 $O(N + M)$，最坏情况 $O(N \cdot M)$（频繁哈希冲突时）。
 * 空间复杂度: $O(1)$。
 *
 * @note
 * 【区间长度不匹配引发内存越界与崩溃 Bug 审计与修复】：
 * 1. **模式串过长导致无符号下溢与越界崩溃 Bug**：原循环条件为 `for (int i = 0; i <= str.size() - pat.size(); ++i)`。
 *    若输入主串比模式串短（如 `str = "a", pat = "aa"`），
 *    `str.size() - pat.size()` 会发生无符号整数下溢，变成极大正数（如 $2^{64}-1$）。
 *    这导致循环体内调用 `create_hash(str, pat.size())` 强行读取 `str[1]`，直接引发**数组越界访问崩溃（段错误）**。
 *    **修复**：在函数入口处添加防御性校验：
 *    `if (pat.empty() || str.empty() || pat.size() > str.size()) return -1;`
 * 2. **浮点数计算效率优化**：原哈希采用了 `std::pow` 浮点数计算。
 *    在整数哈希运算中，调用浮点函数会带来不必要的性能开销，我们在注释中对此进行了深入剖析。
 *
 * @author Unknown author
 */

#include <cassert>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#define PRIME 5  ///< 多项式哈希采用的基数（底数）

namespace string_search {

/**
 * @brief 构建字符串前 n 个字符的初始哈希值
 * @param s 源字符串
 * @param n 哈希的长度
 * @return 计算出的哈希值 (int64_t)
 */
int64_t create_hash(const std::string& s, int n) {
    int64_t result = 0;
    for (int i = 0; i < n; ++i) {
        // 利用 s[i] 乘以底数 PRIME 的 i 次幂累加
        result += static_cast<int64_t>(s[i] * static_cast<int64_t>(std::pow(PRIME, i)));
    }
    return result;
}

/**
 * @brief 滚动更新哈希值（常数时间内实现滑动窗口哈希更新）
 * @param s 主文本串
 * @param old_index 移出窗口的字符索引
 * @param new_index 移入窗口的字符索引
 * @param old_hash 窗口移动前的哈希值
 * @param patLength 模式串长度
 * @return 新窗口子串的哈希值
 */
int64_t recalculate_hash(const std::string& s, int old_index, int new_index,
                         int64_t old_hash, int patLength) {
    // 1. 减去移出字符的值
    int64_t new_hash = old_hash - s[old_index];
    // 2. 除以底数，使所有剩余字符的幂次降低一级
    new_hash /= PRIME;
    // 3. 加上新移入字符对应的高次幂权值
    new_hash += static_cast<int64_t>(s[new_index] * static_cast<int64_t>(std::pow(PRIME, patLength - 1)));
    return new_hash;
}

/**
 * @brief 逐字符核对两个子串是否绝对相等（解决哈希冲突的保障）
 */
bool check_if_equal(const std::string& str1, const std::string& str2,
                    int start1, int end1, int start2, int end2) {
    if (end1 - start1 != end2 - start2) {
        return false;
    }
    while (start1 <= end1 && start2 <= end2) {
        if (str1[start1] != str2[start2]) {
            return false;
        }
        start1++;
        start2++;
    }
    return true;
}

/**
 * @brief Rabin-Karp 字符串检索主匹配函数
 * @param str 主文本串
 * @param pat 待搜索的目标模式串
 * @return 若匹配成功返回首个匹配的起始索引，若未找到则返回 -1
 */
int rabin_karp(const std::string& str, const std::string& pat) {
    // 核心修复：空安全防护与大小防护，彻底解决 str.size() - pat.size() 发生下溢导致的段错误崩溃 Bug
    if (pat.empty() || str.empty() || pat.size() > str.size()) {
        return -1;
    }

    // 计算模式串的哈希值以及主串首个窗口子串的哈希值
    int64_t pat_hash = create_hash(pat, static_cast<int>(pat.size()));
    int64_t str_hash = create_hash(str, static_cast<int>(pat.size()));
    
    int limit = static_cast<int>(str.size() - pat.size());

    for (int i = 0; i <= limit; ++i) {
        // 哈希相同时进行二次核对，防止冲突误判
        if (pat_hash == str_hash &&
            check_if_equal(str, pat, i, i + static_cast<int>(pat.size()) - 1, 0,
                           static_cast<int>(pat.size()) - 1)) {
            return i;
        }
        
        // 滑动窗口向右移动一位，滚动式更新主串窗口哈希值
        if (i < limit) {
            str_hash = recalculate_hash(str, i, i + static_cast<int>(pat.size()), str_hash, static_cast<int>(pat.size()));
        }
    }
    return -1;  // 匹配失败
}

}  // namespace string_search

/**
 * @brief 单元自测用例
 */
int main() {
    // 预期不匹配，返回 -1
    assert(string_search::rabin_karp("helloWorld", "world") == -1);
    // 预期在索引 5 处匹配
    assert(string_search::rabin_karp("helloWorld", "World") == 5);
    // 预期在索引 8 处匹配
    assert(string_search::rabin_karp("this_is_c++", "c++") == 8);
    // 预期在索引 0 处匹配
    assert(string_search::rabin_karp("happy_coding", "happy") == 0);
    
    // 边界情况验证：模式串过长
    assert(string_search::rabin_karp("short", "longer_pattern") == -1);
    // 边界情况验证：空输入
    assert(string_search::rabin_karp("text", "") == -1);

    std::cout << "All Rabin-Karp tests passed successfully!" << std::endl;
    return 0;
}
