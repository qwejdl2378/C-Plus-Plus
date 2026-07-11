/**
 * @file
 * @brief Implementation of the [Bidirectional Dijkstra Shortest Path Algorithm](https://www.coursera.org/learn/algorithms-on-graphs/lecture/7ml18/bidirectional-dijkstra) (双向迪杰斯特拉最短路径算法)
 *
 * @details
 * 双向迪杰斯特拉算法（Bidirectional Dijkstra）是传统迪杰斯特拉算法的高效变种。
 * 它同时从源点（正向搜索）和终点（反向搜索）出发进行松弛，当两边的搜索在某个节点“相遇”时终止。
 * 这种方法极大地减少了搜索所遍历的节点数量，特别是在大型网格或道路网络中。
 *
 * ### 算法终止与距离更新
 * 1. 使用两个最小堆 `pq[0]`（正向）和 `pq[1]`（反向）分别执行松弛。
 * 2. 一旦某个节点被正向和反向搜索都访问（Visited）过，说明两条路径相交。
 * 3. 此时，最短距离并不一定就是该节点的 `dist[0][u] + dist[1][u]`，
 *    我们需要遍历所有两边都已经访问过的节点集 `workset`，寻找使 `dist[0][v] + dist[1][v]` 最小的节点作为真正的最短路长。
 *
 * @note
 * 【重要逻辑局限性分析】：
 * 在第 110 行中，代码定义了一个全局单一布尔向量 `std::vector<bool> visited(n);`。
 * 并在第 153-156 行中：
 *   `if (visited[currentNode] == 1) return Shortest_Path_Distance(workset, dist);`
 *   `visited[currentNode] = true;`
 * 这里存在一个逻辑瑕疵：
 * C++ 的优先队列没有提供 `decrease_key` 接口，因此常用“懒惰删除”机制，同一节点可能会被多次推入优先队列。
 * 如果同一个方向的优先队列重复弹出同一个节点（重复元素），由于使用的是同一个 `visited` 表，
 * 代码会误判定为正反向搜索“相遇”而提前终止算法。虽然在简单图上能通过测试，但在存在大量环路的复杂大图上，
 * 会发生由于重复弹出引起的早停（Premature Termination）错误。
 * 推荐的严谨写法是使用两个独立的 `visited[2][n]` 状态表，分别记录正向和反向的访问状态，仅当两边都访问了同一节点时才终止。
 *
 * 时间复杂度: O((V + E) log V)，常数比单向 Dijkstra 小一倍左右。
 * 空间复杂度: O(V + E)
 *
 * @author [Marinovksy](http://github.com/Marinovsky)
 */

#include <cassert>   /// 用于 assert 断言
#include <cstdint>   /// 用于 uint64_t 等类型
#include <iostream>  /// 用于输入输出
#include <limits>    /// 用于常量 INF
#include <queue>     /// 用于 std::priority_queue
#include <utility>   /// 用于 std::make_pair
#include <vector>    /// 用于 std::vector

// 定义无穷大值常量
constexpr int64_t INF = std::numeric_limits<int64_t>::max();

/**
 * @namespace graph
 * @brief 图算法命名空间
 */
namespace graph {
/**
 * @namespace bidirectional_dijkstra
 * @brief 双向迪杰斯特拉算法相关命名空间
 */
namespace bidirectional_dijkstra {
/**
 * @brief 在正向图和反向图中同时添加有向边
 * @param adj1 正向邻接表指针
 * @param adj2 反向邻接表指针
 * @param u 起点
 * @param v 终点
 * @param w 边权
 */
void addEdge(std::vector<std::vector<std::pair<uint64_t, uint64_t>>> *adj1,
             std::vector<std::vector<std::pair<uint64_t, uint64_t>>> *adj2,
             uint64_t u, uint64_t v, uint64_t w) {
    (*adj1)[u - 1].push_back(std::make_pair(v - 1, w));
    (*adj2)[v - 1].push_back(std::make_pair(u - 1, w)); // 反向图保存反向边以供反向搜索
}

/**
 * @brief 扫描所有已被搜索访问的相交节点候选集，返回全局真正最短路长度
 * @param workset_ 搜索过程中遍历到的候选节点集
 * @param distance_ 正向与反向各自到达每个节点的累计距离表
 * @returns 最终最短路径长度
 */
uint64_t Shortest_Path_Distance(
    const std::vector<uint64_t> &workset_,
    const std::vector<std::vector<uint64_t>> &distance_) {
    int64_t distance = INF;
    for (uint64_t i : workset_) {
        // 最短路可能经过 workset 中的任意相交点 i
        if (distance_[0][i] + distance_[1][i] < distance) {
            distance = distance_[0][i] + distance_[1][i];
        }
    }
    return distance;
}

/**
 * @brief 双向迪杰斯特拉算法主体
 * @param adj1 正向图邻接表
 * @param adj2 反向图邻接表
 * @param s 起源点 (0-indexed)
 * @param t 目标终点 (0-indexed)
 * @returns 最短路距离，若不连通则返回 -1
 */
int Bidijkstra(std::vector<std::vector<std::pair<uint64_t, uint64_t>>> *adj1,
               std::vector<std::vector<std::pair<uint64_t, uint64_t>>> *adj2,
               uint64_t s, uint64_t t) {
    uint64_t n = adj1->size();

    // 存储正反两个方向到各个顶点的最短距离
    std::vector<std::vector<uint64_t>> dist(2, std::vector<uint64_t>(n, INF));

    // pq[0] 代表正向优先队列，pq[1] 代表反向优先队列
    // 存储 pair<距离, 节点ID>，采用 std::greater 实现最小堆
    std::vector<
        std::priority_queue<std::pair<uint64_t, uint64_t>,
                            std::vector<std::pair<uint64_t, uint64_t>>,
                            std::greater<std::pair<uint64_t, uint64_t>>>>
        pq(2);
    
    std::vector<uint64_t> workset;  // 记录访问过的节点，用于最终的最短路径重新计算
    std::vector<bool> visited(n);   // 警告：单 visited 数组容易因重复元素出队而提早误判相遇

    // 1. 初始化正向起点
    pq[0].push(std::make_pair(0, s));
    dist[0][s] = 0;

    // 2. 初始化反向起点（即终点 t）
    pq[1].push(std::make_pair(0, t));
    dist[1][t] = 0;

    while (true) {
        // --- 正向搜索的一步 ---
        if (pq[0].size() == 0) {
            break; // 正向队列为空，说明不连通
        }
        uint64_t currentNode = pq[0].top().second;
        uint64_t currentDist = pq[0].top().first;
        pq[0].pop();

        for (std::pair<int, int> edge : (*adj1)[currentNode]) {
            if (currentDist + edge.second < dist[0][edge.first]) {
                dist[0][edge.first] = currentDist + edge.second;
                pq[0].push(std::make_pair(dist[0][edge.first], edge.first));
            }
        }
        workset.push_back(currentNode);

        // 如果该节点已被访问（即另一个方向也访问过），则两个搜索相遇
        if (visited[currentNode] == 1) {
            return Shortest_Path_Distance(workset, dist);
        }
        visited[currentNode] = true;

        // --- 反向搜索的一步 ---
        if (pq[1].size() == 0) {
            break; // 反向队列为空，不连通
        }
        currentNode = pq[1].top().second;
        currentDist = pq[1].top().first;
        pq[1].pop();

        for (std::pair<int, int> edge : (*adj2)[currentNode]) {
            if (currentDist + edge.second < dist[1][edge.first]) {
                dist[1][edge.first] = currentDist + edge.second;
                pq[1].push(std::make_pair(dist[1][edge.first], edge.first));
            }
        }
        workset.push_back(currentNode);

        // 相遇判定
        if (visited[currentNode] == 1) {
            return Shortest_Path_Distance(workset, dist);
        }
        visited[currentNode] = true;
    }
    return -1; // 无法到达
}
}  // namespace bidirectional_dijkstra
}  // namespace graph

/**
 * @brief 单元自测用例
 */
static void tests() {
    std::cout << "Initiatinig Predefined Tests..." << std::endl;
    std::cout << "Initiating Test 1..." << std::endl;
    std::vector<std::vector<std::pair<uint64_t, uint64_t>>> adj1_1(
        4, std::vector<std::pair<uint64_t, uint64_t>>());
    std::vector<std::vector<std::pair<uint64_t, uint64_t>>> adj1_2(
        4, std::vector<std::pair<uint64_t, uint64_t>>());
    graph::bidirectional_dijkstra::addEdge(&adj1_1, &adj1_2, 1, 2, 1);
    graph::bidirectional_dijkstra::addEdge(&adj1_1, &adj1_2, 4, 1, 2);
    graph::bidirectional_dijkstra::addEdge(&adj1_1, &adj1_2, 2, 3, 2);
    graph::bidirectional_dijkstra::addEdge(&adj1_1, &adj1_2, 1, 3, 5);

    uint64_t s = 1, t = 3;
    assert(graph::bidirectional_dijkstra::Bidijkstra(&adj1_1, &adj1_2, s - 1,
                                                     t - 1) == 3);
    std::cout << "Test 1 Passed..." << std::endl;

    s = 4, t = 3;
    std::cout << "Initiating Test 2..." << std::endl;
    assert(graph::bidirectional_dijkstra::Bidijkstra(&adj1_1, &adj1_2, s - 1,
                                                     t - 1) == 5);
    std::cout << "Test 2 Passed..." << std::endl;

    std::vector<std::vector<std::pair<uint64_t, uint64_t>>> adj2_1(
        5, std::vector<std::pair<uint64_t, uint64_t>>());
    std::vector<std::vector<std::pair<uint64_t, uint64_t>>> adj2_2(
        5, std::vector<std::pair<uint64_t, uint64_t>>());
    graph::bidirectional_dijkstra::addEdge(&adj2_1, &adj2_2, 1, 2, 4);
    graph::bidirectional_dijkstra::addEdge(&adj2_1, &adj2_2, 1, 3, 2);
    graph::bidirectional_dijkstra::addEdge(&adj2_1, &adj2_2, 2, 3, 2);
    graph::bidirectional_dijkstra::addEdge(&adj2_1, &adj2_2, 3, 2, 1);
    graph::bidirectional_dijkstra::addEdge(&adj2_1, &adj2_2, 2, 4, 2);
    graph::bidirectional_dijkstra::addEdge(&adj2_1, &adj2_2, 3, 5, 4);
    graph::bidirectional_dijkstra::addEdge(&adj2_1, &adj2_2, 5, 4, 1);
    graph::bidirectional_dijkstra::addEdge(&adj2_1, &adj2_2, 2, 5, 3);
    graph::bidirectional_dijkstra::addEdge(&adj2_1, &adj2_2, 3, 4, 4);

    s = 1, t = 5;
    std::cout << "Initiating Test 3..." << std::endl;
    assert(graph::bidirectional_dijkstra::Bidijkstra(&adj2_1, &adj2_2, s - 1,
                                                     t - 1) == 6);
    std::cout << "Test 3 Passed..." << std::endl;
    std::cout << "All Test Passed..." << std::endl << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    tests();  // 运行预定义的单元测试
    uint64_t vertices = uint64_t();
    uint64_t edges = uint64_t();
    std::cout << "Enter the number of vertices : ";
    std::cin >> vertices;
    std::cout << "Enter the number of edges : ";
    std::cin >> edges;

    std::vector<std::vector<std::pair<uint64_t, uint64_t>>> adj1(
        vertices, std::vector<std::pair<uint64_t, uint64_t>>());
    std::vector<std::vector<std::pair<uint64_t, uint64_t>>> adj2(
        vertices, std::vector<std::pair<uint64_t, uint64_t>>());

    uint64_t u = uint64_t(), v = uint64_t(), w = uint64_t();
    std::cout << "Enter the edges by three integers in this form: u v w "
              << std::endl;
    std::cout << "Example: if there is and edge between node 1 and node 4 with "
                 "weight 7 enter: 1 4 7, and then press enter"
              << std::endl;
    while (edges--) {
        std::cin >> u >> v >> w;
        graph::bidirectional_dijkstra::addEdge(&adj1, &adj2, u, v, w);
        if (edges != 0) {
            std::cout << "Enter the next edge" << std::endl;
        }
    }

    uint64_t s = uint64_t(), t = uint64_t();
    std::cout
        << "Enter the source node and the target node separated by a space"
        << std::endl;
    std::cout << "Example: If the source node is 5 and the target node is 6 "
                 "enter: 5 6 and press enter"
              << std::endl;
    std::cin >> s >> t;
    int dist =
        graph::bidirectional_dijkstra::Bidijkstra(&adj1, &adj2, s - 1, t - 1);
    if (dist == -1) {
        std::cout << "Target not reachable from source" << std::endl;
    } else {
        std::cout << "Shortest Path Distance : " << dist << std::endl;
    }

    return 0;
}
