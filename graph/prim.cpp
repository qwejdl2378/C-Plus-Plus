/**
 * @file
 * @brief Implementation of [Prim's Algorithm](https://en.wikipedia.org/wiki/Prim%27s_algorithm) for Minimum Spanning Tree (MST) (普里姆最小生成树算法实现)
 *
 * @details
 * 普里姆（Prim）算法用于求解加权无向连通图的最小生成树（MST）。
 * 它采用贪心策略，从一个初始顶点开始，每次将一条“连接树中顶点与非树中顶点且权值最小”的边加入树中，直到所有顶点都被覆盖。
 *
 * ### 优先队列优化
 * 使用最小堆（优先队列）维护候选边，其元素类型为 `pair<权值, 目标节点>`：
 * - 每次取出权值最小的顶点，若该顶点已被标记（在树中），则跳过（避免成环）。
 * - 否则，累加该边权值到最小生成树的总代价，标记该顶点已加入树中。
 * - 遍历该顶点的所有未标记邻居节点，将其边推入优先队列。
 *
 * @note
 * 【顶点索引 Bug 审计与修复】：
 * 原版代码在主函数中以节点 1 启动普里姆算法：`prim(1, graph);`，但邻接表向量的大小被初始化为 `nodes`。
 * - 如果顶点是 0-indexed，那么当图只有一个顶点时，`graph` 大小为 1，但传入 `1` 会导致在第 21 行 `marked[1]` 发生**越界访问段错误**。
 * - 如果顶点是 1-indexed，那么当输入顶点编号为 `nodes` 时，在第 49 行 `graph[x]` 也会发生**越界写入段错误**。
 * 现已重构为标准的 0-indexed 顶点的安全实现，并从顶点 0 开始执行。
 *
 * 时间复杂度: $O(E \log V)$
 * 空间复杂度: $O(V + E)$
 *
 * @author Unknown author
 */

#include <iostream>
#include <queue>
#include <vector>

using PII = std::pair<int, int>; // 定义对组 <权值, 顶点 ID>

/**
 * @brief 普里姆算法计算最小生成树（MST）的总代价
 * @param start_node 算法执行的起始顶点
 * @param graph 邻接表形式的无向加权图
 * @returns 最小生成树的总权值
 */
int prim(int start_node, const std::vector<std::vector<PII> > &graph) {
    // 最小堆优先队列，以 pair.first（权值）进行比较
    std::priority_queue<PII, std::vector<PII>, std::greater<PII> > Q;
    std::vector<bool> marked(graph.size(), false); // 记录顶点是否已被包含在 MST 中
    int minimum_cost = 0;

    // 起点入队，权值为 0
    Q.push(std::make_pair(0, start_node));
    
    while (!Q.empty()) {
        PII p = Q.top();
        Q.pop();
        int x = p.second;
        
        // 校验目标点是否已经存在于生成树中以防止环路
        if (marked[x] == true) {
            continue;
        }
        
        minimum_cost += p.first; // 累加边权到 MST
        marked[x] = true;        // 染成已访问（加入生成树）
        
        // 将该节点的所有未访问邻居边推入堆中
        for (const PII &neighbor : graph[x]) {
            int y = neighbor.second;
            if (marked[y] == false) {
                Q.push(neighbor); // neighbor 格式为 <权值, 邻节点ID>
            }
        }
    }
    return minimum_cost;
}

/**
 * @brief 主函数
 */
int main() {
    int nodes = 0, edges = 0;
    std::cout << "Enter the number of nodes and edges: ";
    std::cin >> nodes >> edges;
    if (nodes == 0 || edges == 0) {
        return 0;
    }

    // 邻接表表示法，第 u 项保存所有 <w, v> 的边
    std::vector<std::vector<PII> > graph(nodes);

    std::cout << "Enter edges in format (u v weight) where vertex is 0-indexed [0, nodes-1]:" << std::endl;
    for (int i = 0; i < edges; ++i) {
        int x = 0, y = 0, weight = 0;
        std::cin >> x >> y >> weight;
        if (x >= nodes || y >= nodes || x < 0 || y < 0) {
            std::cerr << "Invalid node indices. Exceeded range [0, " << nodes - 1 << "]" << std::endl;
            return 1;
        }
        graph[x].push_back(std::make_pair(weight, y));
        graph[y].push_back(std::make_pair(weight, x)); // 无向图保存双向边
    }

    // 从节点 0 开始执行算法
    int minimum_cost = prim(0, graph);
    std::cout << "Minimum Spanning Tree Cost: " << minimum_cost << std::endl;
    return 0;
}
