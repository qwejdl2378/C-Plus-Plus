/**
 * @file
 * @brief [DSU (Disjoint sets)](https://en.wikipedia.org/wiki/Disjoint-set-data_structure) (带路径压缩和深度优化的并查集实现)
 * @details
 * 并查集是一种高效的、管理不相交集合的数据结构。
 * 本实现采用面向对象编程（OOP）风格，利用路径压缩和按深度（高度）合并的双重启发式优化。
 * 同时，除了维护集合树的根节点之外，还额外维护了集合的其他属性信息（如集合大小、集合内最小值和最大值）。
 *
 * @author [AayushVyasKIIT](https://github.com/AayushVyasKIIT)
 * @see dsu_union_rank.cpp
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
    vector<uint64_t> p;           ///< 存储每个节点的父节点编号
    vector<uint64_t> depth;       ///< 存储节点在树中的秩（高度级别）
    vector<uint64_t> setSize;     ///< 存储以该节点为根的子树集合的元素总数
    vector<uint64_t> maxElement;  ///< 存储该集合内的最大元素值
    vector<uint64_t> minElement;  ///< 存储该集合内的最小元素值

 public:
    /**
     * @brief 构造函数，初始化并查集的所有数据成员
     * @param n 并查集的初始元素最大个数
     */
    explicit dsu(uint64_t n) {
        p.assign(n, 0);
        /// 初始化时，每个节点的父节点都是自身
        for (uint64_t i = 0; i < n; i++) {
            p[i] = i;
        }
        /// 初始化时，每个树的高度都为 0，最大值和最小值都为自身
        depth.assign(n, 0);
        maxElement.assign(n, 0);
        minElement.assign(n, 0);
        for (uint64_t i = 0; i < n; i++) {
            depth[i] = 0;
            maxElement[i] = i;
            minElement[i] = i;
        }
        setSize.assign(n, 0);
        /// 初始化时，每个独立集合的节点大小都为 1
        for (uint64_t i = 0; i < n; i++) {
            setSize[i] = 1;
        }
    }

    /**
     * @brief 查找元素 i 所在集合的代表元（根节点），单次查询的均摊时间复杂度为 O(1)
     * @details 采用路径压缩，在回溯时将沿途所有节点直接挂载到根节点下。
     * @param i 查询的目标元素
     * @returns 集合的代表元根节点
     */
    uint64_t findSet(uint64_t i) {
        if (p[i] == i) {
            return i;
        }
        return (p[i] = findSet(p[i])); // 路径压缩
    }

    /**
     * @brief 合并元素 i 和 j 所在的两个不相交集合
     * @details 采用按秩合并优化，防止树高退化；同时更新集合的大小、最小值、最大值信息。
     * @param i 元素一
     * @param j 元素二
     */
    void UnionSet(uint64_t i, uint64_t j) {
        // 如果已处于同一集合中，直接返回
        if (isSame(i, j)) {
            return;
        }

        // 查找两个集合的根节点代表
        uint64_t x = findSet(i);
        uint64_t y = findSet(j);

        // 总是将较矮的树挂载到较深的树下面。如果 x 树比 y 树高，交换它们，使 x 始终为较矮树的根
        if (depth[x] > depth[y]) {
            std::swap(x, y);
        }
        p[x] = y; // 将 x 的父指针设为 y

        // 如果高度相同，挂载后树的高度增加 1
        if (depth[x] == depth[y]) {
            depth[y]++;
        }
        
        // 累加新集合的大小到新的根节点 y 上
        setSize[y] += setSize[x];
        // 更新新的根节点 y 上的集合最大值与最小值信息
        maxElement[y] = std::max(maxElement[x], maxElement[y]);
        minElement[y] = std::min(minElement[x], minElement[y]);
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
     * @brief 获取元素 i 所在集合的完整统计信息
     * @param i 元素
     * @return 包含 {最小值, 最大值, 集合大小} 的 std::vector
     */
    vector<uint64_t> get(uint64_t i) {
        vector<uint64_t> ans;
        ans.push_back(get_min(i));
        ans.push_back(get_max(i));
        ans.push_back(size(i));
        return ans;
    }

    /**
     * @brief 获取元素 i 所在集合的大小
     * @param i 元素
     * @return 集合大小
     */
    uint64_t size(uint64_t i) { return setSize[findSet(i)]; }

    /**
     * @brief 获取元素 i 所在集合的最大元素值
     * @param i 元素
     * @return 最大值
     */
    uint64_t get_max(uint64_t i) { return maxElement[findSet(i)]; }

    /**
     * @brief 获取元素 i 所在集合的最小元素值
     * @param i 元素
     * @return 最小值
     */
    uint64_t get_min(uint64_t i) { return minElement[findSet(i)]; }
};

/**
 * @brief 单元自测试用例 1
 */
static void test1() {
    uint64_t n = 10;
    dsu d(n + 1);
    
    d.UnionSet(1, 2);
    d.UnionSet(1, 4);
    vector<uint64_t> ans = {1, 4, 3}; // 集合 {1, 2, 4}，最小值为1，最大值为4，大小为3
    for (uint64_t i = 0; i < ans.size(); i++) {
        assert(d.get(4).at(i) == ans[i]);
    }
    cout << "1st test passed!" << endl;
}

/**
 * @brief 单元自测试用例 2
 */
static void test2() {
    uint64_t n = 10;
    dsu d(n + 1);
    
    d.UnionSet(3, 5);
    d.UnionSet(5, 6);
    d.UnionSet(5, 7);
    vector<uint64_t> ans = {3, 7, 4}; // 集合 {3, 5, 6, 7}，最小值为3，最大值为7，大小为4
    for (uint64_t i = 0; i < ans.size(); i++) {
        assert(d.get(3).at(i) == ans[i]);
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
