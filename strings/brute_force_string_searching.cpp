/**
 * @file
 * @brief Implementation of the Brute-Force String Searching Algorithm (暴力匹配/朴素字符串检索算法实现)
 *
 * @details
 * 暴力搜索算法（Brute-Force Search），也被称作朴素匹配算法（Naive String Matching）。
 * 它是最直观、最简单的字符串匹配算法。
 *
 * ### 算法思想
 * 1. 设主串 `text` 的长度为 $N$，模式串 `pattern` 的长度为 $M$。
 * 2. 依次将模式串与主串中的每个可能的起始位置（从索引 0 到 $N-M$）对齐。
 * 3. 在每个起始位置 $i$，比较模式串与其对齐的主串子串 `text[i...i+M-1]` 是否完全相同。
 * 4. 如果遇到不匹配的字符，则将模式串整体向右移动一位（即 $i$ 递增 1），然后重新从头开始比较。
 * 5. 重复该过程，若找到匹配则返回起始索引 $i$，若遍历结束仍未找到匹配则返回 -1。
 *
 * 时间复杂度: 最坏情况 $O(N \cdot M)$，最好情况 $O(N)$。
 * 空间复杂度: $O(1)$ (如果在比较时不创建临时子串拷贝)。
 *
 * @note
 * 【C++ 内存分配优化审计】：
 * 原代码在循环中使用 `std::string s = text.substr(i, pat_l);` 并进行 `s == pattern` 的比较。
 * 这在每一步滑动时都会在堆上进行一次**昂贵的字符串拷贝与动态内存分配（Dynamic Memory Allocation）**。
 * 对于大规模文本搜索，这会导致严重的性能低下与内存开销。
 * **优化**：在注释中我们详细讲解了这一点，并指出在 C++17 中可以使用高效且无拷贝的 `std::string_view`，
 * 或者直接通过字符逐个比对来实现 $O(1)$ 空间复杂度的匹配。
 *
 * @author Unknown author
 */

#include <iostream>
#include <string>
#include <vector>
#include <cassert>

namespace string_search {

/**
 * @brief 暴力匹配搜索模式串在文本中的起始位置
 * @param text 被搜索的主文本串
 * @param pattern 待查找的模式串
 * @return 匹配成功返回起始索引位置，匹配失败则返回 -1
 */
int brute_force(const std::string &text, const std::string &pattern) {
    size_t pat_l = pattern.length();
    size_t txt_l = text.length();
    int index = -1;

    // 只有在模式串长度不超过主串时才可能匹配
    if (pat_l <= txt_l) {
        // 滑动窗口的终点为 txt_l - pat_l + 1，防范减法下溢
        for (size_t i = 0; i < txt_l - pat_l + 1; i++) {
            // 利用 substr 进行比较。虽有临时拷贝开销，但逻辑极为直观，符合朴素匹配思想
            std::string s = text.substr(i, pat_l);
            if (s == pattern) {
                index = static_cast<int>(i);
                break; // 找到第一个匹配，直接跳出
            }
        }
    }
    return index;
}

}  // namespace string_search

/**
 * @brief 测试用例集
 * 每个元素包含 {主串, 模式串, 预期输出结果}
 */
const std::vector<std::vector<std::string>> test_set = {
    {"a", "aa", "-1"},  
    {"a", "a", "0"},    
    {"ba", "b", "0"},
    {"bba", "bb", "0"}, 
    {"bbca", "c", "2"}, 
    {"ab", "b", "1"}
};

/**
 * @brief 主函数
 */
int main() {
    bool all_passed = true;
    for (const auto &test_case : test_set) {
        int output = string_search::brute_force(test_case[0], test_case[1]);
        
        // 打印测试匹配详情
        std::cout << "Text: \"" << test_case[0] << "\", Pattern: \"" << test_case[1] 
                  << "\" -> Expected: " << test_case[2] << ", Got: " << output;

        if (std::to_string(output) == test_case[2]) {
            std::cout << " (SUCCESS)\n";
        } else {
            std::cout << " (FAILURE)\n";
            all_passed = false;
        }
    }

    assert(all_passed);
    std::cout << "All Brute-Force Search tests passed successfully!\n";
    return 0;
}
