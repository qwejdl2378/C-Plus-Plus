/**
 * @file
 * @brief Implementation of [Radix sort](https://en.wikipedia.org/wiki/Radix_sort) using counting sort subroutine (基于计数排序的基数排序优化实现)
 *
 * @details
 * 相比于原版 `radix_sort.cpp`，本版本（radix_sort2）使用了标准的基于偏移量前缀和的**稳定计数排序（Counting Sort）**作为子程序。
 * 避免了原版多次全扫描的不必要开销。
 *
 * ### 性能分析
 * - 时间复杂度: $O(d \cdot (N + B))$，其中 d 为数字的位数，B 为基数（十进制下为 10）。
 * - 空间复杂度: $O(N + B)$。
 *
 * @note
 * 【原版代码的严重数据溢出与崩溃 Bug 审计与修复】：
 * 1. **整数类型截断/溢出 Bug**：在第 51 行中，子程序的参数 `cur_digit` 被声明为 `uint16_t`（最大范围 65535）。
 *    而在 `radix` 的数位权重循环中，`i` 会随着乘以 10 迅速超过 65535（如 100000, 1000000）。
 *    这会导致传入的权重发生**整型截断溢出（Integer Wrap-around）**。一旦数字大于 65535，排序逻辑将彻底失效。
 *    现已将 `cur_digit` 改为与元素类型匹配的 `uint64_t`。
 * 2. **空数组解引用崩溃 Bug**：在第 84 行中，代码直接调用 `*max_element(ar.begin(), ar.end())`。
 *    如果输入的 `vector` 为空，对 `ar.end()` 进行解引用会触发**段错误崩溃（Segmentation Fault）**。
 *    现已在函数入口处添加了空数组的安全防护拦截。
 *
 * @author [Suyash Jaiswal](https://github.com/Suyashjaiswal)
 */

#include <algorithm>  /// 用于 std::max_element, std::is_sorted
#include <cassert>    /// 用于 assert 断言
#include <cstdint>   /// 用于 uint64_t, uint32_t 等类型
#include <iostream>   /// 用于标准输出
#include <vector>     /// 用于 std::vector

/**
 * @namespace sorting
 * @brief 排序算法命名空间
 */
namespace sorting {
/**
 * @namespace radix_sort
 * @brief 基数排序算法命名空间
 */
namespace radix_sort {

/**
 * @brief 针对当前数位（权重）的稳定计数排序子程序
 * @param cur_digit 当前比较数位的权重（如 1, 10, 100, 1000 ...）
 * @param ar 待排序数组
 * @returns 针对当前位有序的新数组
 */
std::vector<uint64_t> step_ith(
    uint64_t cur_digit, // 核心修复：类型从 uint16_t 改为 uint64_t，防止高位权重溢出
    const std::vector<uint64_t>& ar) {
    int n = ar.size();
    
    // 1. 统计当前位上各数字的频次（0~9）
    std::vector<uint32_t> position(10, 0);
    for (int i = 0; i < n; ++i) {
        position[(ar[i] / cur_digit) % 10]++;
    }
    
    // 2. 累加前缀和，计算每个数字在输出数组中的起始偏移位置
    int cur = 0;
    for (int i = 0; i < 10; ++i) {
        int a = position[i];
        position[i] = cur;
        cur += a;
    }
    
    // 3. 根据起始位置稳定写回临时数组
    std::vector<uint64_t> temp(n);
    for (int i = 0; i < n; ++i) {
        temp[position[(ar[i] / cur_digit) % 10]] = ar[i];
        position[(ar[i] / cur_digit) % 10]++; // 更新下个相同数字的写回位置
    }
    return temp;
}

/**
 * @brief 基数排序主入口
 * @param ar 待排序的无符号整型数组
 * @returns 排序完毕的新数组
 */
std::vector<uint64_t> radix(const std::vector<uint64_t>& ar) {
    // 核心修复：空安全防线
    if (ar.empty()) {
        return ar;
    }

    // 寻找最大元素
    uint64_t max_ele = *std::max_element(ar.begin(), ar.end());
    std::vector<uint64_t> temp = ar;
    
    // 逐位处理：从个位（1）开始，每次循环乘 10
    // 当权重 i 超过最大元素时，说明所有数字都已完成了最高位的分配，循环终止
    for (uint64_t i = 1; max_ele / i > 0; i *= 10) {
        temp = step_ith(i, temp);
    }
    
    for (uint64_t i : temp) {
        std::cout << i << " ";
    }
    std::cout << "\n";
    return temp;
}
}  // namespace radix_sort
}  // namespace sorting

/**
 * @brief 单元自测用例
 */
static void tests() {
    // 测试 1
    std::vector<uint64_t> ar1 = {432, 234, 143, 332, 123};
    ar1 = sorting::radix_sort::radix(ar1);
    assert(std::is_sorted(ar1.begin(), ar1.end()));

    // 测试 2: 包含大于 65535 的大数（用以验证溢出 Bug 的修复效果）
    std::vector<uint64_t> ar2 = {200000, 3214, 123, 111, 112, 100042,
                                 133, 132,  32,  12,  113};
    ar2 = sorting::radix_sort::radix(ar2);
    assert(std::is_sorted(ar2.begin(), ar2.end()));
    
    std::cout << "All radix sort 2 tests passed!" << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    tests(); // 运行自测
    return 0;
}
