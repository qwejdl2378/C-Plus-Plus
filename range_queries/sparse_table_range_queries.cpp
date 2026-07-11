/**
 * @file
 * @brief Implementation of [Sparse Table](https://en.wikipedia.org/wiki/Range_minimum_query) data structure (稀疏表/ST 表算法实现)
 *
 * @details
 * 稀疏表（Sparse Table，简称 ST 表）是一种用于解决**区间最值查询（Range Minimum/Maximum Query, RMQ）**的高效数据结构。
 *
 * ### 算法原理（倍增与动态规划）
 * 1. **状态定义**：`table[i][j]` 表示区间 `[j, j + 2^i - 1]` 内的极值（如最小值）。区间长度为 $2^i$。
 * 2. **状态转移方程**：
 *    - 当 $i = 0$ 时，区间长度为 1，`table[0][j] = A[j]`。
 *    - 当 $i > 0$ 时，区间可以平分为两个长度为 $2^{i-1}$ 的子区间：
 *      `table[i][j] = min(table[i-1][j], table[i-1][j + 2^(i-1)])`
 * 3. **常数查询（$O(1)$）**：对于任意查询区间 `[L, R]`，设其长度为 $len = R - L + 1$，
 *    计算满足 $2^k \le len$ 的最大整数 $k$ （可以通过预处理对数表 `logs` 获取）。
 *    区间 `[L, R]` 必然能被两个长度为 $2^k$ 的区间 `[L, L + 2^k - 1]` 和 `[R - 2^k + 1, R]` 完全覆盖（即使有重叠也不影响最值结果）。
 *    因此最值为：`min(table[k][L], table[k][R - 2^k + 1])`。
 *
 * ### 复杂度
 * - 预处理时间复杂度: $O(N \log N)$
 * - 单次最值查询时间复杂度: $O(1)$
 * - 空间复杂度: $O(N \log N)$
 *
 * @note
 * 【原版代码的严重越界与边界遗漏 Bug 审计与修复】：
 * 1. **Log 数组越界崩溃 Bug**：原 `computeLogs` 函数中，`logs` 大小设为 `n`。
 *    但在查询区间 `[0, n-1]` 时，区间长度为 `n`，代码会去读取 `logs[n]`，直接引发**数组越界访问崩溃**。
 *    同时如果 `n <= 1`，执行 `logs[1] = 0` 也会引发越界写崩溃。
 *    **修复**：重构 `logs` 数组大小为 `n + 1`，并对 `n == 0` 进行安全防卫。
 * 2. **末尾元素遗漏（边界计算错误） Bug**：在 `buildTable` 中，内层循环边界写为 `j + curLen < n`。
 *    当 `curLen = 1` 时，循环在 `j < n - 1` 时截止，**原数组最后一个元素 A[n-1] 没被填入第一层 ST 表**！
 *    这导致在查询任何包含最后一个元素的区间时，返回错误的数据。
 *    **修复**：修正内层循环边界为正确的 `j + curLen <= n`。
 * 3. **硬编码行数安全隐患**：原代码硬编码 `table` 行数为 `20`。如果数组元素多于 $2^{20}$，就会发生行越界崩溃。
 *    **修复**：根据实际数据大小，动态分配 `logs[n] + 1` 作为行数。
 *
 * @author Unknown author
 */

#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>

/**
 * @namespace range_queries
 * @brief 区间查询算法命名空间
 */
namespace range_queries {
/**
 * @namespace sparse_table
 * @brief 稀疏表相关算法命名空间
 */
namespace sparse_table {

/**
 * @brief 预计算对数表（用于快速计算 log2）
 * @tparam T 元素类型
 * @param A 原始输入数组
 * @return 长度为 N+1 的对数表，logs[i] 存储 floor(log2(i))
 */
template <typename T>
std::vector<int> computeLogs(const std::vector<T>& A) {
    int n = A.size();
    if (n == 0) {
        return {};
    }
    // 核心修复：分配 n + 1 大小，彻底解决 logs[n] 越界读崩溃问题
    std::vector<int> logs(n + 1, 0);
    logs[1] = 0;
    for (int i = 2; i <= n; i++) {
        logs[i] = logs[i / 2] + 1;
    }
    return logs;
}

/**
 * @brief 动态构建二维稀疏表
 * @tparam T 元素类型
 * @param A 原输入数组引用
 * @param logs 预计算的对数表
 * @return 二维稀疏表
 */
template <typename T>
std::vector<std::vector<T>> buildTable(const std::vector<T>& A,
                                       const std::vector<int>& logs) {
    int n = A.size();
    if (n == 0) {
        return {};
    }
    
    // 核心修复：动态计算行数 logs[n] + 1，防范超大数组下 20 行不够用导致的越界崩溃
    int max_power = logs[n] + 1;
    std::vector<std::vector<T>> table(max_power, std::vector<T>(n, 0));

    // 外层循环：控制区间长度为 2^i
    for (int i = 0; i < max_power; i++) {
        int curLen = 1 << i;
        // 内层循环：扫描区间起点 j
        // 核心修复：更正边界为 j + curLen <= n，包含最后一个元素，杜绝数据漏算 Bug
        for (int j = 0; j + curLen <= n; j++) {
            if (curLen == 1) {
                table[i][j] = A[j]; // 基础情况
            } else {
                // 由左右两半小区间极值汇总
                table[i][j] = std::min(table[i - 1][j], table[i - 1][j + curLen / 2]);
            }
        }
    }
    return table;
}

/**
 * @brief 区间最值查询 (RMQ, O(1))
 * @tparam T 元素类型
 * @param beg 查询区间起点（0-indexed, 闭区间）
 * @param end 查询区间终点（0-indexed, 闭区间）
 * @param logs 对数表
 * @param table 预先构建的稀疏表
 * @return 区间内元素的极小值
 */
template <typename T>
T getMinimum(int beg, int end, const std::vector<int>& logs,
             const std::vector<std::vector<T>>& table) {
    assert(beg <= end);
    int len = end - beg + 1;
    int p = logs[len]; // 找到满足 2^p <= len 的最大幂次
    int pLen = 1 << p;
    // 取两段重叠区间的极小值合并
    return std::min(table[p][beg], table[p][end - pLen + 1]);
}
}  // namespace sparse_table
}  // namespace range_queries

/**
 * @brief 单元自测用例
 */
static void test() {
    std::vector<int> A{1, 2, 0, 3, 9};
    std::vector<int> logs = range_queries::sparse_table::computeLogs(A);
    std::vector<std::vector<int>> table =
        range_queries::sparse_table::buildTable(A, logs);

    // 区间 [0, 0] 最小值应为 1
    assert(range_queries::sparse_table::getMinimum(0, 0, logs, table) == 1);
    // 区间 [0, 4] 包含所有元素，最小值应为 0
    assert(range_queries::sparse_table::getMinimum(0, 4, logs, table) == 0);
    // 区间 [2, 4] 包含元素 {0, 3, 9}，最值为 0
    assert(range_queries::sparse_table::getMinimum(2, 4, logs, table) == 0);
    // 区间 [3, 4] 包含元素 {3, 9}，最值应为 3（验证边界修复正确性）
    assert(range_queries::sparse_table::getMinimum(3, 4, logs, table) == 3);

    std::cout << "All Sparse Table tests passed successfully!" << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    test(); // 运行测试
    return 0;
}
