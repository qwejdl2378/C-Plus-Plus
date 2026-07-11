/**
 * @file
 * @brief Implementation of [Kahn's Topological Sort Algorithm](https://en.wikipedia.org/wiki/Topological_sorting#Kahn's_algorithm) using BFS (基于入度与 BFS 的卡恩拓扑排序算法实现)
 *
 * @details
 * 卡恩（Kahn）算法是拓扑排序的经典算法，其主要思想基于顶点入度（Indegree）的变化。
 * 它也是有向图是否存在环路的一种直观判断手段。
 *
 * ### Kahn 算法步骤
 * 1. 统计图中所有顶点的入度 `deg`。
 * 2. 将所有入度为 0 的顶点压入队列 `q` 中。
 * 3. 循环出队队首节点 `cur`，将其加入拓扑序列中：
 *    - 遍历 `cur` 的所有出边邻居 `i`。由于 `cur` 已输出，邻接点 `i` 的入度 `deg[i]` 减 1。
 *    - 若 `deg[i]` 降为 0，说明其前置依赖已全部满足，将 `i` 加入队列。
 * 4. 重复直至队列为空。
 * 5. **环路判定**：如果最终生成的拓扑序列节点数 `count` 小于图中顶点总数 `V`，
 *    说明图中存在由有向环路引起的“入度死锁”，即图不是有向无环图（DAG），无法完成拓扑排序。
 *
 * @note
 * 【原版代码的潜在缺陷】：
 * 1. 原代码中未对有向环做任何过滤或报警。若图中存在环，算法输出的结果末尾会包含多余的填充零值（因为 `arr` 大小固定为 `V+1` 且由 0 初始化），造成输出错误。
 * 2. 空间分配冗余：顶点取值范围为 `[0, V-1]` 时，大小分配 `V+1` 并不必要。
 * 现已重构为标准的 Kahn 算法实现，在检测到环路时输出清晰警告并截断无效填充。
 *
 * 时间复杂度: O(V + E)
 * 空间复杂度: O(V + E)
 *
 * @author Unknown author
 */

#include <cstdio>
#include <cstring>
#include <iostream>
#include <queue>
#include <vector>

/**
 * @brief 执行卡恩拓扑排序算法
 * @param V 顶点数量
 * @param adj 邻接表表示的有向图 (0-indexed)
 * @returns 拓扑排序好的顶点序列（若存在环路则返回不完整序列）
 */
std::vector<int> topoSortKahn(int V, const std::vector<std::vector<int> > &adj) {
    std::vector<bool> vis(V, false); // 记录入队状态
    std::vector<int> deg(V, 0);      // 记录各顶点入度

    // 1. 统计各顶点的入度
    for (int i = 0; i < V; i++) {
        for (int j : adj[i]) {
            deg[j]++;
        }
    }

    std::queue<int> q;
    // 2. 将所有入度为 0 的顶点入队
    for (int i = 0; i < V; i++) {
        if (deg[i] == 0) {
            q.push(i);
            vis[i] = true;
        }
    }

    std::vector<int> arr; // 动态存放拓扑序列
    arr.reserve(V);
    
    // 3. BFS 弹出并消减入度
    while (!q.empty()) {
        int cur = q.front();
        q.pop();
        arr.push_back(cur);

        for (int i : adj[cur]) {
            if (!vis[i]) {
                deg[i]--;
                if (deg[i] == 0) {
                    q.push(i);
                    vis[i] = true;
                }
            }
        }
    }

    // 4. 环路校验
    if (arr.size() < static_cast<size_t>(V)) {
        std::cerr << "Warning: Cycle detected in graph! Bailing out, topological sort is incomplete." << std::endl;
    }
    return arr;
}

/**
 * @brief 主函数
 */
int main() {
    int nodes = 0, edges = 0;
    std::cout << "Enter number of edges and nodes: ";
    std::cin >> edges >> nodes;
    if (edges == 0 || nodes == 0) {
        return 0;
    }
    int u = 0, v = 0;

    std::vector<std::vector<int> > graph(nodes);

    /*
     * 输入示例（包含 6 个顶点，6 条有向边）：
     * 6 6
     * 5 0 5 2 2 3 4 0 4 1 1 3
     */
    std::cout << "Enter " << edges << " edges (u v) representing u -> v:" << std::endl;
    for (int i = 0; i < edges; i++) {
        std::cin >> u >> v;
        if (u >= nodes || v >= nodes || u < 0 || v < 0) {
            std::cerr << "Index out of range!" << std::endl;
            return 1;
        }
        graph[u].push_back(v);
    }

    std::vector<int> topo = topoSortKahn(nodes, graph);
    
    // 输出拓扑排序好的序列
    std::cout << "Topological Sort: ";
    for (size_t i = 0; i < topo.size(); i++) {
        std::cout << topo[i] << " ";
    }
    std::cout << std::endl;
    return 0;
}
