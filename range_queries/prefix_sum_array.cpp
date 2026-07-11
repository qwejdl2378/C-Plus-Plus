/**
 * @file
 * @brief Implementation of the [Prefix Sum Array](https://en.wikipedia.org/wiki/Prefix_sum) data structure (前缀和数组算法实现)
 *
 * @details
 * 前缀和数组（Prefix Sum Array）是一种非常基础但应用极其广泛的区间查询优化技术。
 *
 * ### 算法原理
 * 1. 给定一个大小为 $N$ 的原数组 `A`。
 * 2. 构造一个大小为 $N+1$ 的前缀和数组 `PSA`，其中 `PSA[i]` 存储原数组前 `i` 个元素的累加和（`A[0] + A[1] + ... + A[i-1]`），且规定 `PSA[0] = 0`。
 * 3. 对于任意的闭区间查询 `[l, r]`（从 0 开始的索引，且 $l \le r$），其区间元素和可以通过常数时间内的减法运算直接求得：
 *    `Sum(l, r) = PSA[r + 1] - PSA[l]`
 *
 * ### 性能分析
 * - 预处理建树时间复杂度: $O(N)$
 * - 单次区间查询时间复杂度: $O(1)$
 * - 空间复杂度: $O(N)$
 *
 * @note
 * 【原版代码的严重越界与逻辑缺失 Bug 审计与修复】：
 * 1. **首元素遗漏 Bug**：原版建构函数 `build` 的循环中写为：
 *    `for (std::size_t i = 1; i < original_array.size(); ++i) { PSA.push_back(PSA.back() + original_array[i]); }`
 *    这导致原数组的第一个元素 `original_array[0]` **被完全漏掉**，根本没有加入前缀和！
 *    原测试代码由于将首元素设为 `0` 从而巧合避开了求和偏差。
 * 2. **首位查询越界 Bug**：原查询设计为 `PSA[end] - PSA[beg - 1]`。
 *    如果要查询包含起点 0 的区间（如 `[0, r]`），`beg - 1` 就会变为 `-1`，导致 `PSA[-1]` **发生严重的数组越界读取**。
 * 
 * ### 修复方案
 * - 重构前缀和数组为标准的 $N+1$ 尺寸；
 * - 修正 `build` 函数，使 `PSA[i] = PSA[i-1] + original_array[i-1]` 包含所有元素；
 * - 修正 `query` 函数为标准的 `PSA[end + 1] - PSA[beg]`。
 *
 * @author [Paulo Vitor Lima Borges](https://github.com/PauloVLB)
 */

#include <cassert>   /// 用于 assert 断言
#include <iostream>  /// 用于标准输出
#include <vector>    /// 用于 std::vector

/**
 * @namespace range_queries
 * @brief 区间查询算法命名空间
 */
namespace range_queries {
/**
 * @namespace prefix_sum_array
 * @brief 前缀和数组相关命名空间
 */
namespace prefix_sum_array {

// 全局前缀和数组，PSA[i] 存储前 i 个元素之和
std::vector<int64_t> PSA{};

/**
 * @brief 构建前缀和数组
 * @param original_array 原始数组的引用
 */
void build(const std::vector<int64_t>& original_array) {
    PSA.clear();
    PSA.resize(original_array.size() + 1, 0); // 核心修复：大小设为 N+1，PSA[0] = 0
    
    for (std::size_t i = 0; i < original_array.size(); ++i) {
        PSA[i + 1] = PSA[i] + original_array[i]; // 核心修复：将包括 original_array[0] 在内的所有元素累加进去
    }
}

/**
 * @brief 查询区间 [beg, end] 内的元素和 (0-indexed 闭区间)
 * @param beg 区间起始位置
 * @param end 区间结束位置
 * @returns 区间元素之和
 */
int64_t query(size_t beg, size_t end) {
    // 核心修复：防御非法区间与越界
    if (beg > end || end >= PSA.size() - 1) {
        return 0;
    }
    return PSA[end + 1] - PSA[beg]; // 核心修复：使用标准公式，安全防范 beg - 1 下标越界
}

}  // namespace prefix_sum_array
}  // namespace range_queries

/**
 * @brief 单元自测用例
 */
static void test() {
    // 首元素修改为 10（验证修复首元素被漏掉的 Bug 效果）
    std::vector<int64_t> values{10, 123, 0,  2,  -2, 5,
                                24, 0,   23, -1, -1};  

    range_queries::prefix_sum_array::build(values);

    // 测试 1：包含首端点 0 的区间查询 [0, 10]，预期和为 10 + 173 = 183
    assert(range_queries::prefix_sum_array::query(0, 10) == 183);
    
    // 测试 2：查询 [1, 10]，预期和为 173
    assert(range_queries::prefix_sum_array::query(1, 10) == 173);

    // 测试 3：查询区间 [4, 6] (-2 + 5 + 24 = 27)
    assert(range_queries::prefix_sum_array::query(4, 6) == 27);

    // 测试 4：查询区间 [5, 9] (5 + 24 + 0 + 23 - 1 = 51)
    assert(range_queries::prefix_sum_array::query(5, 9) == 51);

    std::cout << "All Prefix Sum Array tests passed successfully!" << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    test();  // 运行自测
    return 0;
}
