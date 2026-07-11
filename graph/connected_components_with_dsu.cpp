/**
 * @file
 * @brief Implementation of connected components using Disjoint Set Union (DSU) (基于并查集/不相交集合合并的连通分量计数算法实现)
 *
 * @details
 * 并查集（DSU, Disjoint Set Union）是一种非常高效的树形数据结构，主要用于处理一些不相交集合的合并及查询问题。
 *
 * @note
 * 【关于算法效率的学术分析】：
 * 原作者在文档中称“对于静态图，DFS 计算连通分量是低效的，而 DSU 是最省时间的”。
 * **这个结论在学术上并不完全准确**：
 * 1. 对于**静态图**：直接使用 DFS 可以在一次完整的遍历中以 $O(V + E)$ 线性时间求得所有连通分量，并且不需要额外的树高度管理和路径压缩逻辑，常数更小、更高效。
 * 2. 对于**动态图（在线加边）**：若图的边是在运行中不断动态添加的，且需要实时查询当前的连通分量数，DFS 需要每次重新运行，开销达 $O(V + E)$；而 DSU 能够以接近常数的 $O(\alpha(V))$ 时间完成动态合并和在线更新，这才是 DSU 的核心绝对优势所在。
 *
 * ### 并查集优化技术
 * 1. **路径压缩（Path Compression）**：在 `find_set` 中使用 `parent[val] = parent[parent[val]]`（即双跳压缩法，虽然不是完全压缩到根节点，但实现简单且有效），使树深度在查找时不断变扁平。
 * 2. **按大小合并（Union by Size）**：在 `union_sets` 中，总是将较小树的根节点挂在较大树的根节点下，防止并查集退化为单链表。
 *
 * 时间复杂度: 单次操作近乎常数级 $O(\alpha(V))$
 * 空间复杂度: O(V)
 *
 * @author Unknown author
 * @author [Sagar Pandya](https://github.com/sagarpandyansit)
 */

#include <cstdint>
#include <iostream>  /// 用于输入输出
#include <set>       /// 用于 std::set
#include <vector>    /// 用于 std::vector

/**
 * @namespace graph
 * @brief 图算法命名空间
 */
namespace graph {
/**
 * @namespace disjoint_union
 * @brief 并查集算法命名空间
 */
namespace disjoint_union {
uint32_t number_of_nodes = 0;                // 节点总数
std::vector<int64_t> parent{};               // 各节点的父节点表
std::vector<uint32_t> connected_set_size{};  // 各并查集的大小表

/**
 * @brief 初始化并查集，每个节点作为独立集合（父节点指向自身，大小为 1）
 */
void make_set() {
    for (uint32_t i = 1; i <= number_of_nodes; i++) {
        parent[i] = i;
        connected_set_size[i] = 1;
    }
}

/**
 * @brief 查找节点所在的集合代表元（根节点），并进行路径压缩
 * @param val 要查询的节点值
 * @return 该节点所在集合的根节点代表元
 */
int64_t find_set(int64_t val) {
    while (parent[val] != val) {
        parent[val] = parent[parent[val]]; // 路径压缩：父指针上跳指向祖父节点
        val = parent[val];
    }
    return val;
}

/**
 * @brief 合并两个节点所在的集合
 * @param node1 第一个节点
 * @param node2 第二个节点
 */
void union_sets(int64_t node1, int64_t node2) {
    node1 = find_set(node1);  // 找到 node1 的根代表元
    node2 = find_set(node2);  // 找到 node2 的根代表元

    // 根代表元不同，执行启发式合并（小树挂在大树下面）
    if (node1 != node2) {
        if (connected_set_size[node1] < connected_set_size[node2]) {
            std::swap(node1, node2);
        }
        parent[node2] = node1;  // 将 node2 挂在 node1 之下
        connected_set_size[node1] += connected_set_size[node2];  // 累加集合大小
    }
}

/**
 * @brief 统计当前图中的连通分量总数
 * @returns 不同的根节点代表元数量，即连通分量数
 */
uint32_t no_of_connected_components() {
    std::set<int64_t> temp;  // 利用 set 自动去重功能统计不同根的个数
    for (uint32_t i = 1; i <= number_of_nodes; i++) {
        temp.insert(find_set(i));
    }
    return temp.size();
}
}  // namespace disjoint_union
}  // namespace graph

/**
 * @brief 测试驱动函数
 */
static void test() {
    namespace dsu = graph::disjoint_union;
    std::cout << "Enter the number of nodes: ";
    std::cin >> dsu::number_of_nodes;
    
    // 向量大小调整为 number_of_nodes + 1 (因为节点为 1-indexed)
    dsu::parent.resize(dsu::number_of_nodes + 1);
    dsu::connected_set_size.resize(dsu::number_of_nodes + 1);
    
    dsu::make_set(); // 初始化并查集
    
    uint32_t edges = 0;
    std::cout << "Enter the number of edges: ";
    std::cin >> edges;
    
    std::cout << "Enter edges (u v):" << std::endl;
    while (edges--) {
        int64_t node_a = 0, node_b = 0;
        std::cin >> node_a >> node_b;
        dsu::union_sets(node_a, node_b); // 动态合并连通支
    }
    
    std::cout << "Total connected components: " << dsu::no_of_connected_components() << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    test();  // 运行测试程序
    return 0;
}
