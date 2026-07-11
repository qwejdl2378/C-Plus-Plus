/**
 * @file
 * @brief Implementation of [Fenwick Tree / Binary Indexed Tree (BIT)](https://en.wikipedia.org/wiki/Fenwick_tree) (树状数组/芬威克树算法实现)
 *
 * @details
 * 树状数组（Fenwick Tree / Binary Indexed Tree, BIT）是一种支持单点修改和区间查询的高效数据结构。
 *
 * ### 核心特性与复杂度
 * - **单点更新 (Update)**：在 $O(\log N)$ 时间内更新某个元素的值。
 * - **区间查询 (Query)**：在 $O(\log N)$ 时间内计算前缀和或区间和。
 * - **空间复杂度**：$O(N)$ (仅需一个与原数组等大的辅助数组)。
 *
 * ### 树状关系原理（位运算）
 * - 核心机制利用 `x & (-x)`（即 LSB，最低非零比特位）来定位父子节点关系。
 * - 每一个节点 `bit[i]` 存储了原数组中 `[i - LSB(i) + 1, i]` 这一段区间的元素和。
 * - **向后更新 (Add/Update)**：若索引为 `i` 的位置发生变化，需要将当前变化值累加到所有包含它的区间节点上，操作为 `i += i & -i`。
 * - **向前查询 (Prefix Sum)**：计算 `[1, i]` 的前缀和时，向前累加相应区间的和，操作为 `i -= i & -i`。
 *
 * @note
 * 【C++ 类型截断设计缺陷与模板重构审计与修复】：
 * 1. **数值截断设计缺陷**：原实现中，类内部的 `bit` 数组被固定死为 `std::vector<int>`。
 *    即使构造函数接受了 `double` 或 `float` 类型的 `std::vector<T>`，在进行 `update` 累加时也会被隐式截断成 `int`，导致精度丢失。
 *    **修复**：使用 C++ 模板类 `template <typename T = int>` 对 `fenwick_tree` 进行整体模板化，
 *    使 `bit` 的存储类型与输入数据完全匹配。得益于 C++17 的类模板参数推导（CTAD），原自测代码无需任何修改即可完美运行并自动推导为 `int`。
 * 2. **有符号整型负数溢出隐患**：原有的偏移计算为 `x & (-x)`，若 `x` 为有符号 `int`，对 `INT_MIN` 取负在 C++ 中属于未定义行为（溢出）。
 *    **修复**：使用无符号类型 `size_t` 进行索引，防范溢出风险。
 *
 * @author [Mateusz Grzegorzek](https://github.com/mateusz-grzegorzek)
 * @author [David Leal](https://github.com/Panquesito7)
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
 * @brief 模板化树状数组类
 * @tparam T 数据存储的数值类型（默认为 int，可实例化为 float, double, int64_t 等）
 */
template <typename T = int>
class fenwick_tree {
 private:
    size_t n = 0;            ///< 树状数组所代表的原数组大小
    std::vector<T> bit{};    ///< 存储二进制索引树的数据容器

    /**
     * @brief 计算 index 的最低非零位（Lowest Significant Bit, LSB）
     * @param x 索引位置
     * @return 偏移步长 (size_t)
     */
    inline size_t offset(size_t x) const { 
        return x & (-x); // 使用无符号整型求负，在 C++ 标准中是完全良定义的溢出求补操作
    }

 public:
    /**
     * @brief 基于给定初始数组构造并初始化树状数组
     * @param arr 初始化原数组引用
     */
    explicit fenwick_tree(const std::vector<T>& arr) : n(arr.size()) {
        bit.assign(n + 1, T(0)); // 树状数组下标从 1 开始，故开辟 n+1 大小
        for (size_t i = 0; i < n; ++i) {
            update(i, arr[i]);
        }
    }

    /**
     * @brief 指定容量构造一个全 0 的树状数组
     * @param size 数组容量
     */
    explicit fenwick_tree(size_t size) : n(size) { 
        bit.assign(n + 1, T(0)); 
    }

    /**
     * @brief 单点值更新（在原数组索引 id 处累加值 val，并更新树状数组相关受影响节点）
     * @param id 原数组中的索引位置（从 0 开始）
     * @param val 累加的增量值
     */
    void update(size_t id, T val) {
        id++; // 映射到树状数组的 1-indexed 下标
        while (id <= n) {
            bit[id] += val;
            id += offset(id); // 沿更新链向后调整父节点
        }
    }

    /**
     * @brief 查询原数组区间 [0, id] 的前缀和
     * @param id 前缀区间终点索引（从 0 开始）
     * @return 前缀累加和值
     */
    T sum(size_t id) const {
        id++; // 映射到树状数组的 1-indexed 下标
        T res = T(0);
        while (id > 0) {
            res += bit[id];
            id -= offset(id); // 沿查询链向前合并子区间
        }
        return res;
    }

    /**
     * @brief 区间求和（计算闭区间 [l, r] 内的元素和，1D 离线查询）
     * @param l 左端点索引
     * @param r 右端点索引
     * @return 区间元素累加和
     */
    T sum_range(size_t l, size_t r) const { 
        if (l > r) {
            return T(0);
        }
        if (l == 0) {
            return sum(r);
        }
        return sum(r) - sum(l - 1); 
    }
};

}  // namespace range_queries

/**
 * @brief 单元自测用例
 */
static void tests() {
    // 测试 1：整型树状数组
    std::vector<int> arr = {1, 2, 3, 4, 5};
    range_queries::fenwick_tree<int> ft_int(arr);

    assert(ft_int.sum_range(0, 0) == 1);
    assert(ft_int.sum_range(0, 1) == 3);
    assert(ft_int.sum_range(0, 2) == 6);
    assert(ft_int.sum_range(0, 3) == 10);
    assert(ft_int.sum_range(0, 4) == 15);
    assert(ft_int.sum_range(1, 3) == 9);

    ft_int.update(0, 6); // 原数组第一个元素 1 累加 6 变为 7
    assert(ft_int.sum_range(0, 4) == 21);

    // 测试 2：浮点型树状数组（验证模板修复的浮点数累加正确性）
    std::vector<double> arr_double = {1.5, 2.5, 3.5};
    range_queries::fenwick_tree<double> ft_double(arr_double);
    assert(ft_double.sum_range(0, 2) == 7.5);
    ft_double.update(1, 0.5); // 2.5 累加 0.5 变为 3.0
    assert(ft_double.sum_range(0, 2) == 8.0);

    std::cout << "All Fenwick Tree tests passed!" << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    tests(); // 运行自测
    return 0;
}
