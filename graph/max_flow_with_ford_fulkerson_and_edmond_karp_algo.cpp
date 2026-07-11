/**
 * @file
 * @brief Implementation of Edmonds-Karp algorithm for Maximum Flow (基于 BFS 增广路的埃德蒙兹-卡普最大流算法)
 *
 * @details
 * Edmonds-Karp 算法是福特-富尔克森（Ford-Fulkerson）方法的一种具体实现。
 * 它使用广度优先搜索（BFS）来寻找源点 `source` 到汇点 `sink` 的最短增广路径（以边数最少为标准），
 * 从而保证了算法可以在 $O(V E^2)$ 的时间复杂度内终止，即使在边权极大时也不会退化。
 *
 * ### 算法过程
 * 1. 在残量网络（Residual Graph）上利用 BFS 寻找一条从源点到汇点的增广路。
 * 2. 如果存在增广路，通过回溯 `parent` 数组找到该路上的瓶颈边权值（最小剩余容量 `flow`）。
 * 3. 沿路更新残量网络：正向弧容量减去 `flow`，反向弧（反向流）容量加上 `flow`。
 * 4. 累加 `max_flow`，重复步骤 1，直至残量网络中不存在任何从源点到汇点的增广路。
 *
 * @note
 * 【重要细节与格式 Bug 修复】：
 * 1. 数组限制：`visited` 状态表使用了固定大小为 505 的 `std::bitset<MAXN>`。如果输入顶点数超过 505，会导致越界未定义行为。
 * 2. 打印格式 Bug：
 *    - 第 93 行原为 `std::cout << "\nSource\tDestination\tCapacity\total_nodes";`。
 *      这里的 `\total_nodes` 原意是 `\t` 加上字段名，但被误拼写为变量名。现修正为标准的标题 `Source\tDestination\tFlow`。
 *    - 第 97-98 行原为 `std::cout << source << "\t" << destination << "\t\t" << capacity_ << '\t';`。
 *      末尾打印了 `\t` 而非 `\n`，导致所有参与流量传送的边数据全部挤在同一行输出，极难阅读。现已将末尾修改为 `\n` 换行符。
 *
 * 时间复杂度: $O(V E^2)$
 * 空间复杂度: $O(V^2)$
 *
 * @author Amit Kumar
 */

#include <algorithm>
#include <bitset>
#include <cstring>
#include <iostream>
#include <limits>
#include <queue>
#include <tuple>
#include <utility>
#include <vector>

// 限制图的最大顶点数为 505
const int MAXN = 505;

/**
 * @class Graph
 * @brief 最大流计算图类
 */
class Graph {
    std::vector<std::vector<int> > residual_capacity; ///< 残量容量矩阵
    std::vector<std::vector<int> > capacity;          ///< 原始容量矩阵
    int total_nodes = 0;                              ///< 顶点总数
    int total_edges = 0;                              ///< 边总数
    int source = 0;                                   ///< 源点 ID
    int sink = 0;                                     ///< 汇点 ID
    std::vector<int> parent;                          ///< 存储 BFS 树中每个节点的父节点
    std::vector<std::tuple<int, int, int> > edge_participated; ///< 记录有流量通过的边 {u, v, flow}
    std::bitset<MAXN> visited;                        ///< 访问状态标记位图
    int max_flow = 0;                                 ///< 最终最大流量

    /**
     * @brief 用 BFS 寻找一条从源点到汇点具有剩余容量的增广路径
     * @param source 源点
     * @param sink 汇点
     * @returns 若存在增广路返回 true
     */
    bool bfs(int source, int sink) {
        visited.reset();
        std::queue<int> q;
        q.push(source);
        
        while (q.empty() == false) {
            int current_node = q.front();
            visited.set(current_node);
            q.pop();
            
            for (int i = 0; i < total_nodes; ++i) {
                // 如果当前边还有剩余残量且目标点未访问
                if (residual_capacity[current_node][i] > 0 && !visited[i]) {
                    visited.set(i);
                    parent[i] = current_node; // 记录父节点以便回溯
                    if (i == sink) {
                        return true; // 成功到达汇点
                    }
                    q.push(i);
                }
            }
        }
        return false;
    }

 public:
    /**
     * @brief 从标准输入读取图的信息并初始化
     */
    void set_graph() {
        std::cin >> total_nodes >> total_edges >> source >> sink;
        if (total_nodes > MAXN) {
            std::cerr << "Warning: total_nodes exceeds MAXN limit of " << MAXN << std::endl;
        }
        parent = std::vector<int>(total_nodes, -1);
        capacity = residual_capacity = std::vector<std::vector<int> >(
            total_nodes, std::vector<int>(total_nodes, 0));
            
        for (int i = 0; i < total_edges; ++i) {
            int start = 0, destination = 0, capacity_ = 0;
            std::cin >> start >> destination >> capacity_;
            residual_capacity[start][destination] = capacity_;
            capacity[start][destination] = capacity_;
        }
    }

    /**
     * @brief 执行 Edmonds-Karp 算法求解最大流
     */
    void ford_fulkerson() {
        while (bfs(source, sink)) {
            int current_node = sink;
            int flow = std::numeric_limits<int>::max();
            
            // 1. 回溯增广路，寻找该路段的最小容量瓶颈
            while (current_node != source) {
                int parent_ = parent[current_node];
                flow = std::min(flow, residual_capacity[parent_][current_node]);
                current_node = parent_;
            }
            
            // 2. 正向扣除流量，反向增加流量（反向流量提供“悔牌”机制）
            current_node = sink;
            max_flow += flow;
            while (current_node != source) {
                int parent_ = parent[current_node];
                residual_capacity[parent_][current_node] -= flow;
                residual_capacity[current_node][parent_] += flow;
                current_node = parent_;
            }
        }
    }

    /**
     * @brief 打印有流量经过的边及最大流计算信息
     */
    void print_flow_info() {
        edge_participated.clear();
        for (int i = 0; i < total_nodes; ++i) {
            for (int j = 0; j < total_nodes; ++j) {
                // 如果某条边有流量通过（残量少于原始容量）
                if (capacity[i][j] &&
                    residual_capacity[i][j] < capacity[i][j]) {
                    edge_participated.emplace_back(std::make_tuple(
                        i, j, capacity[i][j] - residual_capacity[i][j]));
                }
            }
        }
        
        std::cout << "\nNodes : " << total_nodes << "\nMax flow: " << max_flow
                  << "\nEdge present in flow: " << edge_participated.size()
                  << '\n';
        
        std::cout << "\nSource\tDestination\tFlow\n"; // 修正 \total_nodes 拼写 bug 并换行
        for (auto& edge_data : edge_participated) {
            int u = 0, v = 0, flow_val = 0;
            std::tie(u, v, flow_val) = edge_data;
            std::cout << u << "\t" << v << "\t\t" << flow_val << '\n'; // 修正结尾 \t 为 \n 换行 bug
        }
    }
};

/**
 * @brief 主函数
 */
int main() {
    /*
       测试图例输入格式:
        4 5 0 3
        0 1 10
        1 2 1
        1 3 1
        0 2 1
        2 3 10
     */
    Graph graph;
    graph.set_graph();
    graph.ford_fulkerson();
    graph.print_flow_info();
    return 0;
}
