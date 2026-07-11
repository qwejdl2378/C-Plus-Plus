/**
 * @file
 * @brief Implementation of the [Knuth-Morris-Pratt (KMP) Algorithm](https://en.wikipedia.org/wiki/Knuth%E2%80%93Morris%E2%80%93Pratt_algorithm) (KMP 字符串模式匹配算法实现)
 *
 * @details
 * KMP 算法是一种著名的、在 $O(N + M)$ 时间复杂度内完成字符串检索的高效算法。
 * 它的核心优势在于：**在发生失配时，能够利用先前已经匹配过的信息，避免回溯主串指针，尽可能多地向右滑动模式串**。
 *
 * ### 核心实现原理
 * 1. **前缀函数/失配数组 (Failure Array / Next / LPS Table)**：
 *    - 预处理模式串，计算得到失配表 `failure`。
 *    - `failure[i]` 表示子串 `pattern[0...i-1]` 中，最长的同时也是真前缀的真后缀的长度。
 *    - 发生失配时，主串指针 `j` 不动，模式串指针根据失配表直接滑行到 `failure[k]` 位置，重新对齐比较。
 * 2. **匹配扫描**：
 *    - 一趟扫描主串，逐步比对主串字符 `text[j]` 与模式串字符 `pattern[k]`。
 *    - 成功比对到模式串末尾时，返回起始匹配索引。
 *
 * 时间复杂度: $O(N + M)$ (预处理 $O(M)$，检索 $O(N)$)
 * 空间复杂度: $O(M)$ (用于存放失配数组)
 *
 * @note
 * 【C++ 有符号与无符号整型比较警告审计与修复】：
 * 1. **有符号/无符号类型警告与溢出隐患**：原代码在 `getFailureArray` 函数中定义循环变量为 `int i = 0`，
 *    但与之比较的 `pattern_length` 却是无符号的 `size_t` 类型。
 *    这会引起编译器的有符号/无符号数比较警告（Signed/Unsigned Comparison Warning），且在超大模式串长度超过 `INT_MAX` 时会导致溢出陷入死循环。
 *    **修复**：将 `int i` 修正为与 `pattern_length` 类型完全对应的 `size_t i`。
 *
 * @author [Yancey](https://github.com/Yancey2023)
 */

#include <cassert>   /// 用于 assert 断言
#include <iostream>  /// 用于标准输出
#include <string>    /// 用于 std::string
#include <vector>    /// 用于 std::vector

/**
 * @namespace string_search
 * @brief 字符串检索算法命名空间
 */
namespace string_search {

/**
 * @brief 预处理模式串，生成失配跳转表（Failure Function）
 * @param pattern 待查找的目标模式串
 * @return 模式串各索引失配后的跳转值数组
 */
std::vector<size_t> getFailureArray(const std::string &pattern) {
    size_t pattern_length = pattern.size();
    std::vector<size_t> failure(pattern_length + 1);
    
    // failure[0] 初始化为特殊标记 npos (-1) 标识没有前缀可跳转
    failure[0] = std::string::npos;
    size_t j = std::string::npos; // 记录最长公共前后缀匹配的候选前缀位置

    // 核心修复：使用 size_t 代替 int，根除 signed/unsigned 警告和超长模式串下的死循环隐患
    for (size_t i = 0; i < pattern_length; i++) {
        // 如果失配且有回跳路径，则通过跳转表进行模式串前缀回退
        while (j != std::string::npos && pattern[j] != pattern[i]) {
            j = failure[j];
        }
        
        // 匹配成功或回退到头，推导下一位的失配跳转位置
        failure[i + 1] = ++j;
    }
    return failure;
}

/**
 * @brief 使用 KMP 算法检索模式串在主串中首次出现的位置
 * @param pattern 待查找的目标模式串
 * @param text 检索的源主文本串
 * @return 匹配成功的首个字符的 0-indexed 索引，若不存在则返回 std::string::npos
 */
size_t kmp(const std::string &pattern, const std::string &text) {
    // 边界防卫：空模式串视为在主串起始位置 0 处直接成功匹配
    if (pattern.empty()) {
        return 0;
    }

    // 预处理模式串，生成失配查找表
    std::vector<size_t> failure = getFailureArray(pattern);
    size_t text_length = text.size();
    size_t pattern_length = pattern.size();
    size_t k = 0; // k 跟踪模式串的匹配字符位置

    // j 依次扫描主串，主串指针完全不需要回溯
    for (size_t j = 0; j < text_length; j++) {
        // 字符失配，通过跳转表快速更新匹配指针 k
        while (k != std::string::npos && pattern[k] != text[j]) {
            k = failure[k];
        }
        
        // 比对成功，k 指针前移。若 k 溢出为 0 说明是回退到头后的新开端
        if (++k == pattern_length) {
            // 完整匹配整个模式串，返回主串对应的起始匹配索引位置
            return j - k + 1;
        }
    }
    
    return std::string::npos; // 检索结束，未找到匹配项
}
}  // namespace string_search

/**
 * @brief 单元自测用例
 */
static void tests() {
    using string_search::kmp;
    
    // 预期不匹配，返回 npos
    assert(kmp("abc1abc12l", "alskfjaldsabc1abc1abc12k2") == std::string::npos);
    // 预期在索引 1 处匹配
    assert(kmp("bca", "abcabc") == 1);
    // 预期匹配，返回 5
    assert(kmp("World", "helloWorld") == 5);
    // 预期匹配，返回 7
    assert(kmp("c++", "his_is_c++") == 7);
    // 预期匹配，返回 0
    assert(kmp("happy", "happy_coding") == 0);
    // 空模式串，返回 0
    assert(kmp("", "pattern is empty") == 0);

    std::cout << "All KMP algorithm tests successfully passed!" << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    tests(); // 运行测试用例
    return 0;
}
