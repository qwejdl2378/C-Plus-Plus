/**
 * @file
 * @brief Find the lowest common ancestor (LCA) of two vertices in a rooted tree using binary lifting (基于倍增法求树上最近公共祖先 LCA 算法实现)
 *
 * @details
 * 最近公共祖先（Lowest Common Ancestor, LCA）：在一个有根树中，两个节点 u 和 v 的最近公共祖先是距离根节点最远且同时为 u 和 v 祖先的节点。
 *
 * ### 倍增算法（Binary Lifting）原理
 * 1. **预处理**：计算出每个节点的 $2^i$ 级祖先。
 *    - 用二元数组 `up[u][i]` 表示节点 `u` 的第 $2^i$ 级祖先。
 *    - 状态转移方程：`up[u][i] = up[up[u][i-1]][i-1]`，即：`u` 的 $2^i$ 级祖先等于 `u` 的 $2^{i-1}$ 级祖先的第 $2^{i-1}$ 级祖先。
 *    - 根节点 `root` 的所有倍增父节点均指向它自身，从而避免了溢出，并天然形成了封顶。
 * 2. **查询**：
 *    - 先将深度较深的点（假设为 `u`）向上提升，直到与另一个点 `v` 处于同一深度级别。这可以通过将两者的深度差 `level_diff` 分解为二进制的 2 的幂之和来实现。
 *    - 如果提升后两点相同，则该点即为 LCA，直接返回。
 *    - 若不相同，从大到小尝试将两点同时向上提升 $2^i$ 步。如果在某一步它们提升后的祖先不同（`up[u][i] != up[v][i]`），则执行提升：`u = up[u][i], v = up[v][i]`。
 *    - 循环结束后，`u` 和 `v` 的直接双亲 `up[u][0]` 即为所求的 LCA。
 *
 * 时间复杂度: 预处理 $O(N \log N)$，单次查询 $O(\log N)$
 * 空间复杂度: $O(N \log N)$
 * 
 * @author [vakhokoto](https://github.com/vakhokoto)
 * @author [Krishna Vedala](https://github.com/kvedala)
 */

#include <cassert>
#include <iostream>
#include <queue>
#include <utility>
#include <vector>

/**
 * @namespace graph
 * @brief 图算法命名空间
 */
namespace graph {
/**
 * @class Graph
 * @brief 邻接表形式表示的无向图类（顶点编号 0 到 N-1）
 */
class Graph {
 public:
    /**
     * @brief 根据边列表构造图的邻接表
     * @param N 顶点总数
     * @param undirected_edges 无向边集合列表
     */
    Graph(size_t N, const std::vector<std::pair<int, int> > &undirected_edges) {
        neighbors.resize(N);
        for (auto &edge : undirected_edges) {
            neighbors[edge.first].push_back(edge.second);
            neighbors[edge.second].push_back(edge.first);
        }
    }

    /**
     * @brief 获取顶点数
     */
    int number_of_vertices() const { return neighbors.size(); }

    std::vector<std::vector<int> > neighbors; ///< 邻接表
};

/**
 * @class RootedTree
 * @brief 有根树表示类，派生自 Graph
 */
class RootedTree : public Graph {
 public:
    /**
     * @brief 构造函数，并利用 BFS 初始化每个顶点的直接父节点及深度级别
     * @param undirected_edges 无向边集
     * @param root_ 树的根节点索引
     */
    RootedTree(const std::vector<std::pair<int, int> > &undirected_edges,
               int root_)
        : Graph(undirected_edges.size() + 1, undirected_edges), root(root_) {
        populate_parents();
    }

    std::vector<int> parent; ///< 存放直接父节点 parent[u]
    std::vector<int> level;  ///< 存放节点深度级 level[u]
    int root;                ///< 树的根节点 ID

 protected:
    /**
     * @brief BFS 辅助函数：从根节点出发计算所有节点的深度及父节点关系
     */
    void populate_parents() {
        parent = std::vector<int>(number_of_vertices(), -1); // -1 表示尚未访问
        level = std::vector<int>(number_of_vertices());
        
        parent[root] = root; // 根节点的父节点设为自身，便于倍增计算
        level[root] = 0;
        
        std::queue<int> queue_of_vertices;
        queue_of_vertices.push(root);
        
        while (!queue_of_vertices.empty()) {
            int vertex = queue_of_vertices.front();
            queue_of_vertices.pop();
            for (int neighbor : neighbors[vertex]) {
                if (parent[neighbor] == -1) {
                    parent[neighbor] = vertex;
                    level[neighbor] = level[vertex] + 1;
                    queue_of_vertices.push(neighbor);
                }
            }
        }
    }
};

/**
 * @class LowestCommonAncestor
 * @brief 倍增 LCA 核心计算类
 */
class LowestCommonAncestor {
 public:
    /**
     * @brief 构造函数：接收有根树，并预计算倍增表
     * @param tree_ 待分析的有根树
     */
    explicit LowestCommonAncestor(const RootedTree &tree_) : tree(tree_) {
        populate_up();
    }

    /**
     * @brief 查询节点 u 和 v 的最近公共祖先
     * @param u 第一个节点
     * @param v 第二个节点
     * @returns u 和 v 的 LCA 节点索引
     */
    int lowest_common_ancestor(int u, int v) const {
        // 1. 确保 u 的深度不比 v 浅，必要时交换两者
        if (tree.level[v] > tree.level[u]) {
            std::swap(u, v);
        }

        // 2. 利用倍增跳跃，将 u 提升到与 v 相同的深度级别
        int level_diff = tree.level[u] - tree.level[v];
        for (int i = 0; (1 << i) <= level_diff; ++i) {
            if (level_diff & (1 << i)) {
                u = up[u][i];
            }
        }
        assert(tree.level[u] == tree.level[v]);

        // 如果重合，说明 LCA 即为 v (或 u)
        if (u == v) {
            return u;
        }

        // 3. 同时向上倍增跳跃，只要两者祖先不同就往上跳
        for (int i = static_cast<int>(up[u].size()) - 1; i >= 0; --i) {
            if (up[u][i] != up[v][i]) {
                u = up[u][i];
                v = up[v][i];
            }
        }

        // 循环终止后，u 和 v 的直接父节点就是它们的最近公共祖先
        assert(up[u][0] == up[v][0]);
        return up[u][0];
    }

    const RootedTree &tree;             ///< 对应有根树引用
    std::vector<std::vector<int> > up;  ///< 倍增表数组：up[vertex][i] 代表 vertex 的 2^i 级祖先

 protected:
    /**
     * @brief 动态递推填充倍增表 up
     */
    void populate_up() {
        up.resize(tree.number_of_vertices());
        // 初始第 2^0 = 1 级祖先即为直接 parent
        for (int vertex = 0; vertex < tree.number_of_vertices(); ++vertex) {
            up[vertex].push_back(tree.parent[vertex]);
        }
        
        // 递推计算 2^level 级祖先
        for (int level = 0; (1 << level) < tree.number_of_vertices(); ++level) {
            for (int vertex = 0; vertex < tree.number_of_vertices(); ++vertex) {
                // 2^(level+1) 级祖先 = 2^level 级祖先的 2^level 级祖先
                up[vertex].push_back(up[up[vertex][level]][level]);
            }
        }
    }
};

}  // namespace graph

/**
 * @brief 单元自测用例
 */
static void tests() {
    /**
     *             _  3  _
     *          /     |     \
     *        1       6       4
     *      / |     /   \       \
     *    7   5   2       8       0
     *            |
     *            9
     */
    std::vector<std::pair<int, int> > edges = {
        {7, 1}, {1, 5}, {1, 3}, {3, 6}, {6, 2}, {2, 9}, {6, 8}, {4, 3}, {0, 4}};
    graph::RootedTree t(edges, 3);
    graph::LowestCommonAncestor lca(t);
    
    assert(lca.lowest_common_ancestor(7, 4) == 3);
    assert(lca.lowest_common_ancestor(9, 6) == 6);
    assert(lca.lowest_common_ancestor(0, 0) == 0);
    assert(lca.lowest_common_ancestor(8, 2) == 6);
}

/**
 * @brief 主函数
 */
int main() {
    tests(); // 运行自测
    std::cout << "All tests passed successfully!" << std::endl;
    return 0;
}
