/**
 * @file 
 * @brief Implementation of [Hopcroft–Karp](https://en.wikipedia.org/wiki/Hopcroft%E2%80%93Karp_algorithm) maximum bipartite matching algorithm (霍普克罗夫特-卡普最大二分匹配算法实现)
 *
 * @details 
 * 霍普克罗夫特-卡普（Hopcroft-Karp）算法用于求解二分图的最大基数匹配（Maximum Cardinality Matching）。
 * 其核心是不断寻找**增广路径（Augmenting Path）**，并在发现增广路径时，将匹配边与非匹配边进行反转，从而使匹配数增加 1。
 * 
 * ### 算法过程
 * 1. 使用 BFS（`bfs()`）建立分层图，并寻找是否存在到达未匹配虚拟节点 `NIL` 的最短交替增广路径。若无法到达 `NIL`，说明已不存在增广路径，算法结束。
 * 2. 使用 DFS（`dfs()`）在分层图的指导下，沿着最短距离寻找多条不相交的增广路径进行增广，更新匹配关系。
 *
 * @note
 * 【测试用例顶点索引纠正】：
 * 算法规定 0 号顶点作为“未匹配虚拟节点 NIL”。因此真实二分图的左右两部顶点集编号均必须从 1 开始（1-indexed）。
 * 原版代码中的测试用例（Test 1 和 Test 3）由于混入了 0 号节点，导致部分边和匹配逻辑失效。
 * 现已全部修正为符合 1-indexed 规范的正确二分图测试用例：
 * - Test 1: 节点数 m=3, n=5。输入边：1-2, 2-5。最大匹配为 2。
 * - Test 2: 节点数 m=4, n=4。输入边：1-1, 1-3, 2-3, 3-4, 4-3, 4-2。最大匹配为 4。
 * - Test 3: 节点数 m=6, n=6。输入边：1-2, 2-5, 2-6, 6-1。最大匹配为 3。
 *
 * 时间复杂度: O(E * sqrt(V))
 * 空间复杂度: O(V + E)
 * 
 * @author [Krishna Pal Deora](https://github.com/Krishnapal4050)
 */

#include <iostream>
#include <cstdlib> 
#include <queue>
#include <list>
#include <climits>
#include <memory>
#include <cassert>

/**
 * @namespace graph 
 * @brief 图算法命名空间
 */
namespace graph { 

/**
 * @class HKGraph
 * @brief 用于 Hopcroft-Karp 算法的二分图类
 */
class HKGraph
{
    int m{};  ///< 二分图左侧的顶点数量
    int n{};  ///< 二分图右侧的顶点数量
    const int NIL{0}; ///< 虚拟未匹配节点 ID 0
    const int INF{INT_MAX};

    std::vector<std::list<int> >adj;  ///< 邻接表，存储左侧顶点连接的右侧顶点

    std::vector<int> pair_u; ///< pair_u[u] 存储左侧 u 匹配的右侧顶点
    std::vector<int> pair_v; ///< pair_v[v] 存储右侧 v 匹配的左侧顶点
    std::vector<int> dist;   ///< dist[u] 存储分层图中左侧顶点到源点的距离

public:
    HKGraph();		       // 默认构造函数
    HKGraph(int m, int n);     // 构造函数
    void addEdge(int u, int v); // 添加边
    
    bool bfs(); // BFS 寻找增广路径
    bool dfs(int u); // DFS 沿着增广路径更新匹配
	
    int hopcroftKarpAlgorithm();  // 求解最大匹配的核心入口
};

/**
 * @brief 执行 Hopcroft-Karp 算法计算最大匹配大小
 * @returns 最大匹配的大小
 */
int HKGraph::hopcroftKarpAlgorithm()
{
    // 初始化匹配表为 NIL (0)
    pair_u = std::vector<int>(m + 1, NIL); 
    pair_v = std::vector<int>(n + 1, NIL); 
    dist = std::vector<int>(m + 1);

    int result = 0;

    // 只要存在增广路径，就持续进行增广
    while (bfs())
    {
        for (int u = 1; u <= m; u++) {
            // 如果 u 未匹配且能够通过 DFS 找到增广路，匹配数加 1
            if (pair_u[u] == NIL && dfs(u)) {
                result++;
            }
        }
    }
    return result;
}

/**
 * @brief 广度优先搜索建立分层图并判断是否有增广路
 * @returns 若存在至少一条增广路返回 true
 */
bool HKGraph::bfs()
{
    std::queue<int> q;

    // 初始化第一层（所有未匹配的左侧节点距离设为 0 并入队）
    for (int u = 1; u <= m; u++)
    {
        if (pair_u[u] == NIL) {
            dist[u] = 0;
            q.push(u);
        } else {
            dist[u] = INF;
        }
    }

    dist[NIL] = INF; // 虚拟节点的距离初始化为无穷大

    while (!q.empty())
    {
        int u = q.front();
        q.pop();

        // 仅当距离小于已找到的最短增广路长度时，继续探索
        if (dist[u] < dist[NIL])
        {
            for (auto it = adj[u].begin(); it != adj[u].end(); ++it)
            {
                int v = *it;
                // 如果右侧节点 v 匹配的左侧节点 pair_v[v] 尚未在当前层中被访问过
                if (dist[pair_v[v]] == INF)
                {
                    dist[pair_v[v]] = dist[u] + 1; // 更新距离并入队
                    q.push(pair_v[v]);
                }
            }
        }
    }

    // 如果虚拟未匹配节点 NIL 可达，说明找到了至少一条增广路径
    return (dist[NIL] != INF);
}

/**
 * @brief 深度优先搜索，尝试沿着 BFS 划分的层进行增广
 * @param u 当前左侧节点
 * @returns 成功增广返回 true
 */
bool HKGraph::dfs(int u)
{
    if (u != NIL)
    {
        for (auto it = adj[u].begin(); it != adj[u].end(); ++it)
        {
            int v = *it;
            // 严格按照 BFS 的分层距离进行深搜
            if (dist[pair_v[v]] == dist[u] + 1)
            {
                // 如果右侧 v 之前未匹配，或者 v 的配对节点可以转移到其他节点（成功找到增广路）
                if (dfs(pair_v[v]) == true)
                {   
                    pair_v[v] = u; // 更新配对
                    pair_u[u] = v;
                    return true;
                }
            }
        }
        dist[u] = INF; // 该节点无法作为增广路的起点，设距离为无穷大防止重复尝试
        return false;
    }
    return true; // 递归到达 NIL 节点，成功找到增广路
}

HKGraph::HKGraph() = default;

/**
 * @brief 构造函数
 * @param m 左侧顶点数量
 * @param n 右侧顶点数量
 */
HKGraph::HKGraph(int m, int n) {
    this->m = m;
    this->n = n;
    adj = std::vector<std::list<int> >(m + 1);
}

/**
 * @brief 添加从左侧 u 到右侧 v 的单向匹配候选边
 */
void HKGraph::addEdge(int u, int v)
{
    adj[u].push_back(v);
}

} // namespace graph

using graph::HKGraph;

/**
 * @brief 单元自测用例（已修正并校正为 1-indexed 顶点编号）
 */
void tests() {
    // 测试 1：m=3, n=5。输入边：1-2, 2-5。最大匹配为 2
    int v1a = 3, v1b = 5;
    HKGraph g1(v1a, v1b); 
    g1.addEdge(1, 2);
    g1.addEdge(2, 5);

    int expected_res1 = 2;
    int res1 = g1.hopcroftKarpAlgorithm();
    assert(res1 == expected_res1);
	
    // 测试 2：m=4, n=4。最大匹配为 4
    int v2a = 4, v2b = 4;
    HKGraph g2(v2a, v2b); 
    g2.addEdge(1, 1);
    g2.addEdge(1, 3);
    g2.addEdge(2, 3);
    g2.addEdge(3, 4);
    g2.addEdge(4, 3);
    g2.addEdge(4, 2);

    int expected_res2 = 4;
    int res2 = g2.hopcroftKarpAlgorithm();
    assert(res2 == expected_res2);
	
    // 测试 3：m=6, n=6。输入边：1-2, 2-5, 2-6, 6-1。最大匹配为 3
    int v3a = 6, v3b = 6;
    HKGraph g3(v3a, v3b); 
    g3.addEdge(1, 2);
    g3.addEdge(2, 5);
    g3.addEdge(2, 6);
    g3.addEdge(6, 1);

    int expected_res3 = 3;
    int res3 = g3.hopcroftKarpAlgorithm();
    assert(res3 == expected_res3);
}

/**
 * @brief 主函数
 */
int main()
{
    tests();  // 执行自测

    int v1 = 0, v2 = 0, e = 0;
    std::cin >> v1 >> v2 >> e;
    HKGraph g(v1, v2);  
    int u = 0, v = 0;
    for (int i = 0; i < e; ++i)
    {
        std::cin >> u >> v;
        g.addEdge(u, v);
    }
  
    int res = g.hopcroftKarpAlgorithm();
    std::cout << "Maximum matching is " << res << "\n";
    return 0;
}
