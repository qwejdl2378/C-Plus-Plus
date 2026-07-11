/**
 * @file
 * @brief Implementation of [Duval's Algorithm](https://en.wikipedia.org/wiki/Lyndon_word) for Finding the Minimum Cyclic Shift (Duval 算法求解最小表示/最小循环移位实现)
 *
 * @details
 * Duval 算法通常用于将字符串分解为 Lyndon 字（Lyndon Factorization）。
 * 并且，它可以在 $O(N)$ 时间和 $O(1)$ 辅助空间内，求解一个序列的**最小表示（Lexicographically Smallest Cyclic Shift）**。
 *
 * ###  Lyndon 字与最小表示
 * - **Lyndon 字**：一个字符串，如果它严格小于它的所有非空真右后缀，则称它为 Lyndon 字。
 * - **最小循环移位（最小表示）**：给定一个长度为 $N$ 的序列，寻找一个起始位置，从该位置循环移动序列，使得得到的序列字典序最小。
 * - Duval 算法利用双指针 `j` 和 `k` 在拼接的双倍序列 `s + s`（逻辑上通过模运算实现）上滑动：
 *   - 指针 `i` 是当前考虑的 Lyndon 块的起点。
 *   - 指针 `j` 是当前比较的字符位置。
 *   - 指针 `k` 记录与前缀进行匹配匹配位置的映射。
 *   - 每次不匹配时，可以将起点 `i` 贪心地向后跳跃 `j - k` 的倍数，从而在 $O(N)$ 线性时间内确定最小表示的位置。
 *
 * 时间复杂度: $O(N)$
 * 空间复杂度: $O(1)$ (仅使用常数个辅助变量，无需复制或拼接新数组)
 *
 * @note
 * 【C++ 零大小输入防御审计】：
 * 1. **除以零潜在崩溃防范**：在内层循环中包含表达式 `s[j % n]`。如果输入容器为空，`j % 0` 会引发**除以零的运行时致命错误（Floating Point Exception）**。
 *    但在 Duval 的代码结构中，外层循环条件为 `while (i < n)`。当容器为空时，`n = 0`，`i < n` 立即为假，直接跳过整个循环体，安全返回 `ans = 0`。
 *    因此，此代码结构天然具备防御性。
 *
 * @author [Amine Ghoussaini](https://github.com/aminegh20)
 */

#include <array>    /// 用于 std::array
#include <cassert>  /// 用于 assert 断言
#include <cstddef>  /// 用于 std::size_t
#include <deque>    /// 用于 std::deque
#include <iostream> ///< 用于标准输出
#include <string>   /// 用于 std::string
#include <vector>   /// 用于 std::vector

/**
 * @namespace string
 * @brief 字符串与序列操作算法命名空间
 */
namespace string {

/**
 * @brief 使用 Duval 算法寻找序列的字典序最小循环移位起点
 * @tparam T 可迭代且支持大小比较的序列类型（如 std::string, std::vector 等）
 * @param s 输入的序列
 * @return 0-indexed 的最小循环移位起始索引
 */
template <typename T>
size_t duval(const T& s) {
    size_t n = s.size();
    size_t i = 0, ans = 0;
    
    // 只要起点 i 还没遍历完原数组长度
    while (i < n) {
        ans = i; // 贪心记录当前可能的最佳起点
        size_t j = i + 1; // j 往后扫描
        size_t k = i;     // k 用于和当前段的前缀进行对照
        
        // 模拟逻辑上的双倍长度序列遍历，进行字典序大小比较
        while (j < (n + n) && s[j % n] >= s[k % n]) {
            if (s[k % n] < s[j % n]) {
                k = i; // 若当前位字符变大，说明找到了更好的前缀关系，对照指针重置为起点
            } else {
                k++;   // 字符相同，对照指针与扫描指针同步后移
            }
            j++;
        }
        
        // 跳跃移动：将 i 跳过重复周期节，优化多重相同字符情况下的扫描效率
        while (i <= k) {
            i += j - k;
        }
    }
    
    return ans; // 返回字典序最小的循环移位起点
}

}  // namespace string

/**
 * @brief 单元自测用例
 */
static void test() {
    using namespace string;

    // 测试 1：abcab 的移位为: abcab -> bcaba -> cabab -> ababc -> babca，最小移位是 ababc，对应起点索引为 3
    std::string s1 = "abcab";
    assert(duval(s1) == 3);

    // 测试 2：011100 -> 最小移位为 000111，对应起点索引为 4
    std::string s2 = "011100";
    assert(duval(s2) == 4);

    // 测试 3：整型 vector
    std::vector<int> v = {5, 2, 1, 3, 4};
    assert(duval(v) == 2);

    // 测试 4：整型 std::array
    std::array<int, 5> a = {1, 2, 3, 4, 5};
    assert(duval(a) == 0);

    // 测试 5：双端队列 std::deque
    std::deque<char> d = {'a', 'z', 'c', 'a', 'b'};
    assert(duval(d) == 3);

    // 测试 6：空输入测试（验证安全防御，不触发除以零错误）
    std::string s3;
    assert(duval(s3) == 0);

    // 测试 7：带负数元素的整型 vector
    std::vector<int> v2 = {5, 2, 1, 3, -4};
    assert(duval(v2) == 4);

    std::cout << "All Duval tests passed!" << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    test();  // 运行测试用例
    return 0;
}
