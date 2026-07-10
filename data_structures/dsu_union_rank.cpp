/**
 * @file
 * @brief [DSU (Disjoint sets)](https://en.wikipedia.org/wiki/Disjoint-set-data_structure) (带深度/秩优化的并查集实现，不带路径压缩)
 * @details
 * 与 `dsu_path_compression.cpp` 不同，本实现**有意不进行路径压缩**。
 * 在 `findSet(i)` 时只沿父指针回溯查找，这使得单次查询时间复杂度为 O(log N)，但能完整保留树的连通层级形态。
 * 提供了一个额外的 `getParents(i)` 函数来返回元素 i 沿着父指针一路到达根节点的完整路径，这在需要追溯层级关系时很有用。
 *
 * @author [AayushVyasKIIT](https://github.com/AayushVyasKIIT)
 * @see dsu_path_compression.cpp
 */

#include <cassert>   /// 用于 assert 断言
#include <cstdint>
#include <iostream>  /// 用于输入输出
#include <vector>    /// 用于 std::vector

using std::cout;
using std::endl;
using std::vector;

/**
 * @brief 类封装的并查集 (dsu)
 */
class dsu {
 private:
    vector<uint64_t> p;        ///< 存储每个节点的父节点编号
    vector<uint64_t> depth;    ///< 存储节点在树中的秩（高度级别）
    vector<uint64_t> setSize;  ///< 存储以该节点为根的子树集合的元素总数

 public:
    /**
     * @brief 构造函数，初始化并查集的所有数据成员
     * @param n 并查集的初始元素最大个数
     */
    explicit dsu(uint64_t n) {
        p.assign(n, 0);
        depth.assign(n, 0);
        setSize.assign(n, 0);
        for (uint64_t i = 0; i < n; i++) {
            p[i] = i;      // 初始时父节点是自身
            depth[i] = 0;  // 初始秩（深度）为 0
            setSize[i] = 1;// 初始集合大小为 1
        }
    }

    /**
     * @brief 查找元素 i 所在集合的代表元（根节点），时间复杂度为 O(log N)
     * @details 这里采用循环回溯方法，未进行路径压缩，从而完全保留合并的历史路径。
     * @param i 查询的目标元素
     * @returns 集合的代表元根节点
     */
    uint64_t findSet(uint64_t i) {
        while (i != p[i]) {
            i = p[i];
        }
        return i;
    }

    /**
     * @brief 合并元素 i 和 j 所在的两个不相交集合
     * @details 采用按秩合并优化，将高度较矮的树挂载到较深的树下面。
     * @param i 元素一
     * @param j 元素二
     */
    void unionSet(uint64_t i, uint64_t j) {
        // 如果已处于同一集合中，直接返回
        if (isSame(i, j)) {
            return;
        }
        
        // 查找两个集合的根节点代表
        uint64_t x = findSet(i);
        uint64_t y = findSet(j);

        // 按秩合并，较浅树 x 挂载到较深树 y 下面
        if (depth[x] > depth[y]) {
            std::swap(x, y);
        }
        p[x] = y;

        // 如果高度相同，挂载后树的高度增加 1
        if (depth[x] == depth[y]) {
            depth[y]++;
        }
        // 累加新集合的大小到新的根节点 y 上
        setSize[y] += setSize[x];
    }

    /**
     * @brief 检查两个元素 i 和 j 是否属于同一个集合
     * @param i 元素一
     * @param j 元素二
     * @return `true` 代表属于同一个集合；`false` 否则
     */
    bool isSame(uint64_t i, uint64_t j) {
        return findSet(i) == findSet(j);
    }

    /**
     * @brief 追溯节点 i 到代表元根节点的完整路径（父节点路径）
     * @param i 元素编号
     * @return 包含路径上所有节点编号（直到根节点）的 std::vector
     */
    vector<uint64_t> getParents(uint64_t i) {
        vector<uint64_t> ans;
        while (p[i] != i) {
            ans.push_back(i);
            i = p[i];
        }
        ans.push_back(i); // 存入最后的根节点
        return ans;
    }
};

/**
 * @brief 单元自测试用例 1
 */
static void test1() {
    uint64_t n = 10;
    dsu d(n + 1);
    d.unionSet(2, 1);
    d.unionSet(1, 4);
    d.unionSet(8, 1);
    d.unionSet(3, 5);
    d.unionSet(5, 6);
    d.unionSet(5, 7);
    d.unionSet(9, 10);
    d.unionSet(2, 10);
    
    // 验证从节点 7 出发的父指针路径：7 -> 5
    vector<uint64_t> ans = {7, 5};
    for (uint64_t i = 0; i < ans.size(); i++) {
        assert(d.getParents(7).at(i) == ans[i]);
    }
    cout << "1st test passed!" << endl;
}

/**
 * @brief 单元自测试用例 2
 */
static void test2() {
    uint64_t n = 10;
    dsu d(n + 1);
    d.unionSet(2, 1);
    d.unionSet(1, 4);
    d.unionSet(8, 1);
    d.unionSet(3, 5);
    d.unionSet(5, 6);
    d.unionSet(5, 7);
    d.unionSet(9, 10);
    d.unionSet(2, 10);

    // 验证从节点 2 出发的父指针路径：2 -> 1 -> 10
    vector<uint64_t> ans = {2, 1, 10};
    for (uint64_t i = 0; i < ans.size(); i++) {
        assert(d.getParents(2).at(i) == ans[i]);
    }
    cout << "2nd test passed!" << endl;
}

/**
 * @brief 主函数
 * @returns 0
 */
int main() {
    test1();  // 运行测试一
    test2();  // 运行测试二
    return 0;
}
