/**
 * @file
 * @brief Implementation of the [Graph Connected Components](https://en.wikipedia.org/wiki/Component_(graph_theory)) algorithm (基于 DFS 的连通分量计数算法实现)
 *
 * @details
 * 在无向图中，如果两个顶点之间存在路径，则称这两个顶点是连通的。
 * 连通分量（Connected Component）是无向图中的极大连通子图。在该子图内，任何两个顶点都是相互可达的。
 *
 * ### 算法原理
 * 1. 维护一个布尔数组 `visited` 记录每个节点的访问状态。
 * 2. 遍历图中的每一个节点。若节点尚未被访问，则以此节点为起点，调用深度优先搜索（DFS）的辅助函数 `explore`。
 * 3. `explore` 会递归访问并标记当前连通分量内的所有可达节点。
 * 4. 每次遇到一个未访问过的节点并成功进行一轮完整的 DFS 搜索，说明找到了一个新的连通分量，计数器 `connected_components` 加 1。
 * 
 * 时间复杂度: O(V + E)
 * 空间复杂度: O(V + E)
 *
 * @author [Ayaan Khan](http://github.com/ayaankhan98)
 */

#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

/**
 * @namespace graph
 * @brief 图算法命名空间
 */
namespace graph {
/**
 * @brief 添加无向边，输入参数为 1-indexed 顶点标识
 * @param adj 邻接表指针
 * @param u 起点节点
 * @param v 终点节点
 */
void addEdge(std::vector<std::vector<int>> *adj, int u, int v) {
    (*adj)[u - 1].push_back(v - 1);
    (*adj)[v - 1].push_back(u - 1);
}

/**
 * @brief DFS 辅助探索函数，递归标记当前连通子图的所有节点
 * @param adj 邻接表指针
 * @param u 当前正在探索的节点ID
 * @param visited 节点访问状态表指针
 */
void explore(const std::vector<std::vector<int>> *adj, int u,
             std::vector<bool> *visited) {
    (*visited)[u] = true; // 标记当前节点已访问
    for (auto v : (*adj)[u]) {
        if (!(*visited)[v]) {
            explore(adj, v, visited); // 递归探索相邻未访问的节点
        }
    }
}

/**
 * @brief 计算图中的连通分量总数
 * @param adj 图邻接表指针
 * @returns 连通分量的数量
 */
int getConnectedComponents(const std::vector<std::vector<int>> *adj) {
    int n = adj->size();
    int connected_components = 0;
    std::vector<bool> visited(n, false); // 初始化所有节点未被访问

    for (int i = 0; i < n; i++) {
        if (!visited[i]) {
            explore(adj, i, &visited); // 探索当前连通分量的所有可达节点
            connected_components++;    // 连通分量计数加 1
        }
    }
    return connected_components;
}
}  // namespace graph

/**
 * @brief 单元自测用例
 */
void tests() {
    std::cout << "Running predefined tests..." << std::endl;
    std::cout << "Initiating Test 1..." << std::endl;
    std::vector<std::vector<int>> adj1(9, std::vector<int>());
    graph::addEdge(&adj1, 1, 2);
    graph::addEdge(&adj1, 1, 3);
    graph::addEdge(&adj1, 3, 4);
    graph::addEdge(&adj1, 5, 7);
    graph::addEdge(&adj1, 5, 6);
    graph::addEdge(&adj1, 8, 9);

    // 包含 3 个连通分量：{1,2,3,4}, {5,6,7}, {8,9}
    assert(graph::getConnectedComponents(&adj1) == 3);
    std::cout << "Test 1 Passed..." << std::endl;

    std::cout << "Innitiating Test 2..." << std::endl;
    std::vector<std::vector<int>> adj2(10, std::vector<int>());
    graph::addEdge(&adj2, 1, 2);
    graph::addEdge(&adj2, 1, 3);
    graph::addEdge(&adj2, 1, 4);
    graph::addEdge(&adj2, 2, 3);
    graph::addEdge(&adj2, 3, 4);
    graph::addEdge(&adj2, 4, 8);
    graph::addEdge(&adj2, 4, 10);
    graph::addEdge(&adj2, 8, 10);
    graph::addEdge(&adj2, 8, 9);
    graph::addEdge(&adj2, 5, 7);
    graph::addEdge(&adj2, 5, 6);
    graph::addEdge(&adj2, 6, 7);

    // 包含 2 个连通分量：{1,2,3,4,8,9,10}, {5,6,7}
    assert(graph::getConnectedComponents(&adj2) == 2);
    std::cout << "Test 2 Passed..." << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    tests(); // 运行自测

    int vertices = 0, edges = 0;
    std::cout << "Enter the number of vertices : ";
    std::cin >> vertices;
    std::cout << "Enter the number of edges : ";
    std::cin >> edges;

    std::vector<std::vector<int>> adj(vertices, std::vector<int>());

    int u = 0, v = 0;
    while (edges--) {
        std::cin >> u >> v;
        graph::addEdge(&adj, u, v);
    }

    int cc = graph::getConnectedComponents(&adj);
    std::cout << "Number of Connected Components: " << cc << std::endl;
    return 0;
}
