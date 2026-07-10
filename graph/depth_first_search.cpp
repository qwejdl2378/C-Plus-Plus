/**
 *
 * \file
 * \brief [Depth First Search Algorithm (Depth First Search)](https://en.wikipedia.org/wiki/Depth-first_search) (深度优先搜索 DFS 算法实现)
 *
 * \author [Ayaan Khan](http://github.com/ayaankhan98)
 *
 * \details
 * 深度优先搜索 (DFS) 是一种经典的图遍历算法。
 * 时间复杂度为 $O(|V| + |E|)$，其中 V 是图中顶点的个数，E 是图中边的条数。
 *
 * DFS 的典型应用场景包括：
 * 1. 寻找连通分量 (Connected Components)
 * 2. 拓扑排序 (Topological Sorting)
 * 3. 寻找割点 (Articulation Points) 和割边/桥 (Bridges)
 * 4. 强连通分量 (SCC - Strongly Connected Components) 检测（如 Tarjan 算法）
 * 5. 解决走迷宫或约束求解问题（回溯法）
 *
 * <h4>工作原理</h4>
 * 1. 初始时，将图中所有的顶点均标记为未访问 (unvisited)。
 * 2. 选择一个起始顶点，开始递归探索。
 * 3. 在探索一个顶点的过程中，首先将其标记为已访问 (visited)，
 *    然后依次探索与该顶点邻接的所有未访问的顶点（通过递归调用）。
 *
 */

#include <algorithm>
#include <iostream>
#include <vector>

/**
 * \namespace graph
 * \brief 图算法命名空间
 */
namespace graph {
/**
 * \brief 在邻接表中添加一条无向边 (连接顶点 u 和 v)
 *
 * @param adj 图的邻接表表示
 * @param u 起始顶点编号 (从 1 开始)
 * @param v 结束顶点编号 (从 1 开始)
 */
void addEdge(std::vector<std::vector<size_t>> *adj, size_t u, size_t v) {
    /*
     * 因为是无向图，所以我们需要双向添加：
     * 将 v-1 添加到 u-1 的邻接表中，
     * 同时将 u-1 添加到 v-1 的邻接表中。
     */
    (*adj)[u - 1].push_back(v - 1);
    (*adj)[v - 1].push_back(u - 1);
}

/**
 * \brief 递归探索顶点的辅助函数 (DFS 的核心)
 * 遍历并访问当前探索节点的所有未访问邻接节点。
 *
 * @param adj 图的邻接表
 * @param v 当前正在探索的顶点索引 (从 0 开始)
 * @param visited 记录各节点访问状态的布尔数组指针
 */
void explore(const std::vector<std::vector<size_t>> &adj, size_t v,
             std::vector<bool> *visited) {
    // 打印当前遍历到的节点 (转回从 1 开始的外部表示)
    std::cout << v + 1 << " ";
    
    // 标记当前顶点为已访问
    (*visited)[v] = true;
    
    // 遍历当前节点的所有邻接节点
    for (auto x : adj[v]) {
        // 如果邻接节点 x 还没有被访问过，则递归进行探索
        if (!(*visited)[x]) {
            explore(adj, x, visited);
        }
    }
}

/**
 * \brief 初始化并启动深度优先搜索 (DFS)
 *
 * @param adj 图的邻接表
 * @param start DFS 探索的起始顶点 (从 0 开始的索引)
 */
void depth_first_search(const std::vector<std::vector<size_t>> &adj,
                         size_t start) {
    size_t vertices = adj.size();

    // 初始化 visited 向量，初始所有顶点均为未访问 (false)
    std::vector<bool> visited(vertices, false);
    
    // 从起点开始进行探索
    explore(adj, start, &visited);
}
}  // namespace graph

/** 主函数，包含用户交互式输入 */
int main() {
    size_t vertices = 0, edges = 0;
    std::cout << "Enter the Vertices : ";
    std::cin >> vertices;
    std::cout << "Enter the Edges : ";
    std::cin >> edges;

    /// 创建图的邻接表表示
    std::vector<std::vector<size_t>> adj(vertices, std::vector<size_t>());

    /// 循环输入每条边的连接情况
    std::cout << "Enter the vertices which have edges between them (e.g., u v) : "
              << std::endl;
    while (edges--) {
        size_t u = 0, v = 0;
        std::cin >> u >> v;
        graph::addEdge(&adj, u, v);
    }

    /// 执行深度优先搜索，这里默认从索引为 2 (即编号为 3) 的节点开始遍历
    std::cout << "DFS Traversal starting from vertex 3: ";
    graph::depth_first_search(adj, 2);

    std::cout << std::endl;
    return 0;
}
