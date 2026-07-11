/**
 * @file
 * @brief Implementation of [Topological Sort Algorithm](https://en.wikipedia.org/wiki/Topological_sorting) using DFS (基于 DFS 三色染色的拓扑排序算法实现)
 *
 * @details
 * 拓扑排序（Topological Sorting）是对有向无环图（DAG）的顶点的一种线性排序。
 * 使得对于从顶点 u 到顶点 v 的每一条有向边 (u, v)，u 在排序中都排在 v 的前面。
 *
 * @note
 * 【原版代码的严重逻辑缺陷与静默失败 Bug 审计】：
 * 原版代码存在两处严重的学术及逻辑缺陷：
 * 1. 描述错误：原注释声称本文件使用的是“Kahn 算法（Kahn's Algorithm）”。
 *    但事实上，Kahn 算法是**基于入度和 BFS** 实现的，而本文件采用的是**基于 DFS 和栈**的深度优先拓扑序算法（Tarjan 方法）。
 * 2. 环路检测失效：原版代码仅使用了一个二值 `visited` 数组（已访问为 1，未访问为 0）。
 *    在 DFS 遍历邻居时，如果遇到已被访问过的节点就直接跳过（第 83 行 `if (!visited[neighbour])`）。
 *    这导致当图中存在环路（如 `0->1->2->0`）时，DFS 会因为遇到已访问的 `0` 而直接退栈，
 *    进而将 `2, 1, 0` 全部压入栈中，`ans` 长度依然是 $n$，导致第 114 行的 `ans.size() < n` 恒为假，
 *    根本无法检测到任何环路！更糟糕的是，自测用例 Test 3 里面使用了 `try-catch` 块却未在无异常时强制失败，
 *    这使得“未能成功检测出环路并抛出异常”这一重大缺陷被静默隐藏，测试代码假装通过了！
 * 
 * ### 修复方案：引入三色标记法检测环路
 * - `0`：未访问状态。
 * - `1`：正在访问中（处于当前的递归栈中）。若 DFS 过程中遇到状态为 `1` 的邻居，代表存在返祖边（环路），立即抛出 `std::invalid_argument`。
 * - `2`：已完全访问状态（已出栈）。
 *
 * 时间复杂度: O(V + E)
 * 空间复杂度: O(V + E)
 *
 * @author [Aditya Borate](https://github.com/adi776borate)
 */

#include <algorithm>  // 用于 std::reverse
#include <cassert>    // 用于 assert
#include <iostream>   // 用于标准输出
#include <stack>      // 用于 std::stack
#include <stdexcept>  // 用于异常抛出
#include <vector>     // 用于 std::vector

/**
 * @namespace graph
 * @brief 图算法命名空间
 */
namespace graph {
/**
 * @namespace topological_sort
 * @brief 拓扑排序算法命名空间
 */
namespace topological_sort {
/**
 * @class Graph
 * @brief 用邻接表表示的有向图类
 */
class Graph {
 private:
    int n;                              // 顶点数量
    std::vector<std::vector<int>> adj;  // 邻接表

 public:
    /**
     * @brief 构造函数
     * @param nodes 节点总数
     */
    Graph(int nodes) : n(nodes), adj(nodes) {}

    /**
     * @brief 添加有向边 u -> v
     */
    void addEdge(int u, int v) { adj[u].push_back(v); }

    /**
     * @brief 获取邻接表的只读引用
     */
    const std::vector<std::vector<int>>& getAdjacencyList() const {
        return adj;
    }

    /**
     * @brief 获取顶点数
     */
    int getNumNodes() const { return n; }
};

/**
 * @brief DFS 辅助函数，采用三色法判定环并生成逆拓扑序
 * @param v 当前访问节点
 * @param visited 状态向量（0=未访问，1=正在访问，2=访问完毕）
 * @param graph 邻接表只读引用
 * @param s 结果栈，保存后序遍历逆序
 */
void dfs(int v, std::vector<int>& visited,
         const std::vector<std::vector<int>>& graph, std::stack<int>& s) {
    visited[v] = 1; // 标记为正在访问 (灰色)
    
    for (int neighbour : graph[v]) {
        if (visited[neighbour] == 0) {
            dfs(neighbour, visited, graph, s);
        } 
        // 发现返祖边：如果邻接点处于正在访问状态 (1)，代表有向图中存在环路
        else if (visited[neighbour] == 1) {
            throw std::invalid_argument("cycle detected in graph");
        }
    }
    
    visited[v] = 2; // 标记为访问完毕 (黑色)
    s.push(v);      // 后序遍历：所有子孙节点均处理完后入栈
}

/**
 * @brief 拓扑排序核心主入口
 * @param g 有向图对象
 * @returns 拓扑排序节点序列
 * @throws std::invalid_argument 若图中存在环路
 */
std::vector<int> topologicalSort(const Graph& g) {
    int n = g.getNumNodes();
    const auto& adj = g.getAdjacencyList();
    std::vector<int> visited(n, 0); // 初始全为 0 (未访问)
    std::stack<int> s;

    for (int i = 0; i < n; i++) {
        if (visited[i] == 0) {
            dfs(i, visited, adj, s);
        }
    }

    std::vector<int> ans;
    while (!s.empty()) {
        ans.push_back(s.top());
        s.pop();
    }

    return ans;
}
}  // namespace topological_sort
}  // namespace graph

/**
 * @brief 单元自测用例
 */
static void test() {
    // 测试 1
    std::cout << "Testing for graph 1\n";
    int n_1 = 6;
    graph::topological_sort::Graph graph1(n_1);
    graph1.addEdge(4, 0);
    graph1.addEdge(5, 0);
    graph1.addEdge(5, 2);
    graph1.addEdge(2, 3);
    graph1.addEdge(3, 1);
    graph1.addEdge(4, 1);
    std::vector<int> ans_1 = graph::topological_sort::topologicalSort(graph1);
    std::vector<int> expected_1 = {5, 4, 2, 3, 1, 0};
    std::cout << "Topological Sorting Order: ";
    for (int i : ans_1) {
        std::cout << i << " ";
    }
    std::cout << '\n';
    assert(ans_1 == expected_1);
    std::cout << "Test Passed\n\n";

    // 测试 2
    std::cout << "Testing for graph 2\n";
    int n_2 = 5;
    graph::topological_sort::Graph graph2(n_2);
    graph2.addEdge(0, 1);
    graph2.addEdge(0, 2);
    graph2.addEdge(1, 2);
    graph2.addEdge(2, 3);
    graph2.addEdge(1, 3);
    graph2.addEdge(2, 4);
    std::vector<int> ans_2 = graph::topological_sort::topologicalSort(graph2);
    std::vector<int> expected_2 = {0, 1, 2, 4, 3};
    std::cout << "Topological Sorting Order: ";
    for (int i : ans_2) {
        std::cout << i << " ";
    }
    std::cout << '\n';
    assert(ans_2 == expected_2);
    std::cout << "Test Passed\n\n";

    // 测试 3 - 含有环路的有向图（必须能成功抛出异常并捕获）
    std::cout << "Testing for graph 3\n";
    int n_3 = 3;
    graph::topological_sort::Graph graph3(n_3);
    graph3.addEdge(0, 1);
    graph3.addEdge(1, 2);
    graph3.addEdge(2, 0);
    
    bool caught_exception = false;
    try {
        graph::topological_sort::topologicalSort(graph3);
    } catch (std::invalid_argument& err) {
        assert(std::string(err.what()) == "cycle detected in graph");
        caught_exception = true;
    }
    assert(caught_exception); // 确保确实进入了 catch 块，排除了静默假过 bug
    std::cout << "Test Passed (Cycle successfully detected!)\n";
}

/**
 * @brief 主函数
 */
int main() {
    test();  // 运行测试
    return 0;
}
