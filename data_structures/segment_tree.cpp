/**
 * @file
 * @brief A data structure to quickly do operations on ranges: the [Segment Tree](https://en.wikipedia.org/wiki/Segment_tree) algorithm implementation
 * @details
 * 线段树 (Segment Tree) 的数据结构实现。
 * 支持单点修改和区间查询（区间求和、区间最值等满足结合律的运算），
 * 两者的时间复杂度均为 O(log N)。
 *
 * @author [Nishant Chatterjee](https://github.com/nishantc1527)
 */

#include <iostream>     /// 用于输入输出操作
#include <vector>       /// 用于 std::vector
#include <algorithm>    /// 用于 std::min 和 std::max
#include <cassert>      /// 用于断言 assert

/*
 * @namespace
 * @brief 数据结构命名空间
 */
namespace data_structures {
/**
 * @brief 线段树类模板
 * @tparam T 存储在线段树中的数据类型
 */
template <class T>
class SegmentTree {
private:
    const T ID = 0;   ///< 结合律运算的恒等元 (零元)，满足 comb(ID, x) = x
    std::vector<T> t; ///< 扁平化数组，用于表示树形结构的节点
    int size = 0;     ///< 线段树管理的原始数组的有效元素个数
private:
    /**
     * @brief 结合律函数，用于合并左右子树的节点值
     * @param x 第一个操作数
     * @param y 第二个操作数
     * @return 左右子树值结合后的结果。在本例中，使用的是加法（求区间和）
     */
    T comb(T x, T y) {
        return x + y;
    }
    /**
     * @brief 计算两个整数的中点
     * @param l 左边界
     * @param r 右边界
     * @return 中间位置的值
     */
    int mid(int l, int r) {
        return l + (r - l) / 2;
    }
    /**
     * @brief 单点更新的辅助递归方法
     * @param i 当前节点的数组索引
     * @param l 当前节点所代表的区间左端点
     * @param r 当前节点所代表的区间右端点
     * @param pos 待更新数据在原数组中的位置
     * @param val 更新后的新值
     */
    void update(int i, int l, int r, int pos, T val) {
        // 递归终止：定位到叶子节点，直接更新叶子节点的值
        if(l == r) t[i] = val;
        else {
            int m = mid(l, r);
            // 判断待更新位置在左半区间还是右半区间，并递归向下更新
            if(pos <= m) update(i * 2, l, m, pos, val);
            else update(i * 2 + 1, m + 1, r, pos, val);
            // 子节点更新完毕后，更新当前节点（区间和）
            t[i] = comb(t[i * 2], t[i * 2 + 1]);
        }
    }
    /**
     * @brief 区间查询的辅助递归方法
     * @param i 当前节点的数组索引
     * @param l 当前节点代表的区间左边界
     * @param r 当前节点代表的区间右边界
     * @param tl 查询区间的左边界
     * @param tr 查询区间的右边界
     * @return 查询区间内所有元素的合并结果（在本例中为区间和）
     */
    T range_comb(int i, int l, int r, int tl, int tr) {
        // 如果当前节点代表的区间恰好与查询区间重合，直接返回该节点值
        if(l == tl && r == tr) return t[i];
        if(tl > tr) return 0; // 非法边界返回零元
        int m = mid(l, r);
        // 递归查询左子树与右子树，并将结果结合起来
        return comb(range_comb(i * 2, l, m, tl, std::min(tr, m)), 
                    range_comb(i * 2 + 1, m + 1, r, std::max(tl, m + 1), tr));
    }
public:
    // 构造函数，开辟 4 * n 大小的空间用于容纳线段树的内部节点
    SegmentTree(int n) : t(n * 4, ID), size(n) {}
    
    /** 
     * @brief 更新数组中指定位置的值（单点修改）
     * @param pos 待修改的位置 (从 1 开始索引)
     * @param val 目标更新值
     */
    void update(int pos, T val) {
        update(1, 1, size, pos, val);
    }
    
    /**
     * @brief 查询区间 [l, r] 的聚合结果（区间查询，此处为区间求和）
     * @param l 区间左端点 (从 1 开始索引)
     * @param r 区间右端点 (从 1 开始索引)
     * @return 区间元素之和
     */
    T range_comb(int l, int r) {
        return range_comb(1, 1, size, l, r);
    }
};
} // namespace data_structures

/**
 * @brief 自测函数，验证线段树的正确性
 */
static void test() {
    // 创建一个管理 5 个元素的线段树
    data_structures::SegmentTree<int> t(5);
    t.update(1, 1);
    t.update(2, 2);
    t.update(3, 3);
    t.update(4, 4);
    t.update(5, 5);
    
    // 验证区间 [1, 3] 的求和应为 1 + 2 + 3 = 6
    assert(t.range_comb(1, 3) == 6); 
    
    // 更新位置 1 的值为 3
    t.update(1, 3);
    // 验证更新后的求和应为 3 + 2 + 3 = 8
    assert(t.range_comb(1, 3) == 8); 

    std::cout << "All tests have successfully passed!\n";
}

/**
 * @brief 主函数
 * @returns 0
 */
int main() {
    test();  // 运行测试用例
    return 0;
}
