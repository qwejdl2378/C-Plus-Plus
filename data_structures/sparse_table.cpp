/**
 * @file
 * @brief Implementation of [Sparse Table](https://brilliant.org/wiki/sparse-table/) for `min()` function. (稀疏表区间最小值RMQ算法实现)
 * @author [Mann Patel](https://github.com/manncodes)
 * @details
 * 稀疏表（Sparse Table）是一种用于回答静态区间查询（Range Queries）的数据结构。
 * 它可以在 O(n log n) 的预处理时间内建表，并在 O(1) 的时间内回答任何“幂等性”（Duplicate Invariant）函数
 * （例如 min、max、gcd、lcm）的区间查询。对于普通的区间和等非幂等查询则需要 O(log n) 时间。
 * 该数据结构唯一的缺点是只适用于不可变数组（静态数组）。一旦数组中的元素发生变化，就必须重新建表。
 *
 * @todo make stress tests.
 *
 * @warning
 * 本稀疏表是专为 `min()`（区间最小值的索引）定制的。只需稍作修改即可改为支持 `gcd()`、`lcm()` 或 `max()` 等。
 */

#include <array>     /// 用于 std::array
#include <cassert>   /// 用于 assert 断言
#include <cstdint>
#include <iostream>  /// 用于输入输出

/**
 * @namespace data_structures
 * @brief 数据结构命名空间
 */
namespace data_structures {

/**
 * @namespace sparse_table
 * @brief 稀疏表算法实现命名空间
 */
namespace sparse_table {

constexpr uint32_t N = 12345;  ///< 数组的最大上限容量限制
constexpr uint8_t M = 14;      ///< 树的高度上限：ceil(log2(N))，即 2^14 > N

/**
 * @brief 稀疏表结构体定义
 */
struct Sparse_table {
    size_t n = 0;  ///< 实际输入数组的大小

    /** @warning 确保静态容量常量 N 不小于 n。如果需要，手动调大 N */

    std::array<int64_t, N> A = {};  ///< 存放原始数值的输入数组
    std::array<std::array<int64_t, N>, M>
        ST{};  ///< 稀疏表 ST 二维数组，存储区间极值的**索引值**
    std::array<int64_t, N> LOG = {};  ///< 预计算的 floor(log2(i)) 对数查找表，用于 O(1) 查询

    /**
     * @brief 构建稀疏表
     * @details
     * 1. 预计算 LOG 数组以供 O(1) 时间的对数查询。
     * 2. 初始化第一层 ST[0][i] = i (长度为 2^0 = 1 的区间，极值索引就是自身)。
     * 3. 动态规划状态转移：区间长度翻倍，合并两个长度为 2^(j-1) 的子区间的极值。
     * 
     * @complexity: O(n * log(n))
     */
    void buildST() {
        LOG[0] = -1; // 0 没有对数值，边界处理

        for (size_t i = 0; i < n; ++i) {
            ST[0][i] = static_cast<int64_t>(i); // 第一层初始化为其自身的索引位置 i
            LOG[i + 1] = LOG[i] + !(i & (i + 1));  ///< 线性递推预计算 log2(i+1)
        }

        // 双重循环填表：外层是区间长度幂次 j，内层是起始位置 i
        for (size_t j = 1; static_cast<size_t>(1 << j) <= n; ++j) {
            for (size_t i = 0; static_cast<size_t>(i + (1 << j)) <= n; ++i) {
                // 将长度为 2^j 的区间拆分为左右两半：[i, i + 2^(j-1) - 1] 和 [i + 2^(j-1), i + 2^j - 1]
                int64_t x = ST[j - 1][i];  ///< 左半部分的极值对应的索引
                int64_t y =
                    ST[j - 1][i + (1 << (j - 1))];  ///< 右半部分的极值对应的索引

                // 比较两半区间的元素大小，把较小值所处的索引记录到 ST[j][i]
                ST[j][i] = (A[x] <= A[y] ? x : y);
            }
        }
    }

    /**
     * @brief 在 O(1) 时间内查询区间 [l, r] （包括左右端点）的极值元素索引
     * @param l 区间左端点（从 0 开始且包含）
     * @param r 区间右端点（包含）
     * @return 区间内最小值元素在原始数组 A 中的索引值
     * @complexity: O(1)
     */
    int64_t query(int64_t l, int64_t r) {
        int64_t g = LOG[r - l + 1];  ///< 能够覆盖区间一半以上的最大 2 的幂次 g
        int64_t x = ST[g][l];  ///< 从左端点 l 向右延伸 2^g 长度区间的极值索引
        int64_t y =
            ST[g][r - (1 << g) + 1];  ///< 从右端点 r 向左延伸 2^g 长度区间的极值索引

        // 合并并返回这两个重叠区间的最小值索引
        return (A[x] <= A[y] ? x : y);
    }
};
}  // namespace sparse_table
}  // namespace data_structures

/**
 * @brief 自测用例集
 */
static void test() {
    // 构造测试数组 [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
    std::array<int64_t, 10> testcase = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    size_t testcase_size = sizeof(testcase) / sizeof(testcase[0]);

    data_structures::sparse_table::Sparse_table st{};

    std::copy(std::begin(testcase), std::end(testcase), std::begin(st.A));
    st.n = testcase_size;

    st.buildST();  ///< 建表预处理

    // 查询区间 [l, r]，由于存储的是最小值对应的索引位置，做断言比对
    assert(st.query(1, 9) == 1);  ///< 区间 [1, 9] 的最小值是 A[1] = 2，其索引为 1
    assert(st.query(2, 6) == 2);  ///< 区间 [2, 6] 的最小值是 A[2] = 3，其索引为 2
    assert(st.query(3, 8) == 3);  ///< 区间 [3, 8] 的最小值是 A[3] = 4，其索引为 3

    std::cout << "Self-test implementations passed!" << std::endl;
}

/**
 * @brief 主函数
 * @returns 0
 */
int main() {
    test();  // 运行单元测试
    return 0;
}
