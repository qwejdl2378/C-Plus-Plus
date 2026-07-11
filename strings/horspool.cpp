/**
 * @file
 * @brief Implementation of the [Boyer-Moore-Horspool String Search Algorithm](https://en.wikipedia.org/wiki/Boyer%E2%80%93Moore%E2%80%93Horspool_algorithm) (Horspool 字符串检索算法实现)
 *
 * @details
 * Boyer-Moore-Horspool 算法是 Boyer-Moore 算法的一个简化版本。
 * 它的核心简化在于：只使用**坏字符规则**来计算滑动距离，并且计算滑动距离时仅参考当前窗口中**主串与模式串最右侧字符对齐处**的那个主串字符，
 * 而不考虑实际是在哪个具体位置发生匹配失败的。这也免去了维护好后缀表和复杂坏字符表的开销。
 *
 * ### 算法思想
 * 1. 预处理：构建模式串的转移表（Shift Table）。
 *    - 对于模式串 `prototype` 的前 $M-1$ 个字符，字符 `c` 对应的滑动距离为 $M - 1 - i$（其中 $i$ 是该字符最右侧出现的索引）。
 *    - 模式串最后一个字符若不在前面出现过，其默认滑动距离为整个模式串长度 $M$。
 *    - 其他所有未在模式串中出现的字符，其默认滑动距离均为 $M$。
 * 2. 匹配过程：
 *    - 将模式串与文本串 `text` 对齐，从右向左逐字符比对。
 *    - 无论在哪里发生匹配失败，直接参考**当前窗口最右侧对齐处**的文本字符，根据转移表查找并向右滑动相应的距离。
 *    - 重复该过程直到找到模式串（返回 `true`）或文本串扫描完毕（返回 `false`）。
 *
 * 时间复杂度: 最好情况 $O(N/M)$，最坏情况 $O(N \cdot M)$。
 * 空间复杂度: $O(|\Sigma|)$，其中 $|\Sigma|$ 为模式串中不同字符的个数。
 *
 * @note
 * 【有符号与无符号数隐式强转警告与安全性修复】：
 * 1. **空串下溢与隐式强转漏洞**：如果传入的 `prototype` 为空串，
 *    代码执行 `int i = static_cast<int>(prototype.size() - 1);` 会导致 `prototype.size() - 1` 溢出为极大正数，
 *    而在强转为 `int` 后变为 `-1`。
 *    在随后的 `while (i < text.size())` 中，由于 `text.size()` 是无符号的 `size_t`，
 *    有符号的 `-1` 会被隐式提升为最大无符号数。这导致比较条件变为 `18446744073709551615 < 0` （假），算法直接返回 `false`，虽没崩溃，但隐藏了严重的隐式转换逻辑漏洞。
 *    **修复**：在函数入口处添加防御性拦截：
 *    `if (prototype.empty() || text.empty() || prototype.size() > text.size()) return false;`
 *
 * @author [Harry Kontakis](https://github.com/ckontakis)
 */

#include <iostream>
#include <unordered_map>
#include <cassert>
#include <string>

/**
 * @namespace strings
 * @brief 字符串处理算法命名空间
 */
namespace strings {
/**
 * @namespace horspool
 * @brief Horspool 检索算法相关命名空间
 */
namespace horspool {

/**
 * @brief 预处理构建 Horspool 算法的滑动转移表
 * @param prototype 模式串（子串）
 * @return 字符到滑动距离的哈希表
 */
std::unordered_map<char, int> findShiftTable(const std::string &prototype) {
    std::unordered_map<char, int> shiftTable;

    int m = static_cast<int>(prototype.size());
    // 遍历模式串的所有字符，计算每个字符发生坏字符时的右移步长
    for (int i = 0; i < m; i++) {
        char ch = prototype[i];
        if (i != m - 1) {
            // 前 M-1 个字符：右移步长为 M - 1 - i
            shiftTable[ch] = m - 1 - i;
        } else {
            // 最后一个字符：如果在前面没有出现过，则右移步长为整个模式串长度 M
            if (shiftTable.find(ch) == shiftTable.end()) {
                shiftTable[ch] = m;
            }
        }
    }
    return shiftTable;
}

/**
 * @brief 使用 Horspool 算法检索主串中是否包含指定的模式串
 * @param text 被检索的主文本串
 * @param prototype 待查找的模式串（子串）
 * @return true 包含子串
 * @return false 不包含子串
 */
bool horspool(const std::string &text, const std::string &prototype) {
    // 核心修复：空输入与越界长度过滤，根除有符号/无符号隐式类型提升下的逻辑漏洞
    if (prototype.empty() || text.empty() || prototype.size() > text.size()) {
        return false;
    }

    // 初始化滑动转移表
    std::unordered_map<char, int> shiftTable = findShiftTable(prototype);

    int n = static_cast<int>(text.size());
    int m = static_cast<int>(prototype.size());
    int i = m - 1; // i 代表当前窗口中最右侧对齐处的主串字符索引

    while (i < n) {
        int j = i;
        int k = 0;
        bool match = true;

        // 从模式串的尾部向头部（从右往左）进行字符比对
        for (int z = m - 1; z >= 0 && match; z--) {
            if (text[j] == prototype[z]) {
                k++;
                j--;
            } else {
                match = false; // 发生不匹配，退出比对循环
            }
        }

        // 如果全部匹配成功（匹配字符数等于模式串长度）
        if (k == m) {
            return true;
        } else {
            // 发生坏字符：不管在何处失配，一律根据“当前窗口最右侧位置 i”对应的字符决定滑动步长
            char rightmost_char = text[i];
            if (shiftTable.find(rightmost_char) != shiftTable.end()) {
                i += shiftTable[rightmost_char];
            } else {
                i += m; // 若最右侧字符不存在于模式串中，可以直接安全向右跨越整个模式串长度 M
            }
        }
    }
    return false;
}
} // namespace horspool
} // namespace strings

/**
 * @brief 单元自测用例
 */
static void test() {
    assert(strings::horspool::horspool("Hello World", "World") == true);
    assert(strings::horspool::horspool("Hello World", " World") == true);
    assert(strings::horspool::horspool("Hello World", "ello") == true);
    assert(strings::horspool::horspool("Hello World", "rld") == true);
    assert(strings::horspool::horspool("Hello", "Helo") == false);
    assert(strings::horspool::horspool("c++_algorithms", "c++_algorithms") == true);
    assert(strings::horspool::horspool("c++_algorithms", "c++_") == true);
    assert(strings::horspool::horspool("Hello", "Hello World") == false);
    assert(strings::horspool::horspool("c++_algorithms", "") == false);
    assert(strings::horspool::horspool("c++", "c") == true);
    assert(strings::horspool::horspool("3458934793", "4793") == true);
    assert(strings::horspool::horspool("3458934793", "123") == false);
    
    // 边界情况：空主串
    assert(strings::horspool::horspool("", "c++") == false);

    std::cout << "All Horspool tests passed successfully!" << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    test(); // 运行测试
    return 0;
}
