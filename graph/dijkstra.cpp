/**
 * @file
 * @brief [Graph Dijkstras Shortest Path Algorithm (Dijkstra's Shortest Path)](https://en.wikipedia.org/wiki/Dijkstra%27s_algorithm) (Dijkstra 单源最短路径算法实现)
 *
 * @author [Ayaan Khan](http://github.com/ayaankhan98)
 *
 * @details
 * Dijkstra 算法用于在加权图中计算从单一源节点到所有其他可达节点的最短路径。
 * 算法初始时假设所有节点均不可达，将其到源节点的距离初始化为 INF（无穷大）。
 *
 * 类似于广度优先搜索 (BFS) 的思想，我们令源节点到自身的距离为 0，
 * 然后将该节点连同其距离推入优先队列中。
 * 这里的优先队列维护为一个“最小堆” (min heap)，这样我们每次取出的都是当前已知距离最短的节点。
 *
 * 算法的本质是“松弛” (relaxation) 操作：每次从未确定最短路径的节点中挑选距离最短的一个节点，
 * 尝试通过它去缩短其所有邻接节点到源节点的已知路径长度。
 *
 */
#include <cassert>
#include <iostream>
#include <limits>
#include <memory>
#include <queue>
#include <utility>
#include <vector>

// 定义无穷大常量，用作初始距离
constexpr int64_t INF = std::numeric_limits<int64_t>::max();

/**
 * @namespace graph
 * @brief 图算法命名空间
 */
namespace graph {
/**
 * @brief 在图的邻接表中添加一条有向带权边
 *
 * @param adj 邻接表指针
 * @param u 起始节点编号 (从 1 开始)
 * @param v 终点节点编号 (从 1 开始)
 * @param w 边的权重
 */
void addEdge(std::vector<std::vector<std::pair<int, int>>> *adj, int u, int v,
             int w) {
    // 转换为从 0 开始的内部索引
    (*adj)[u - 1].push_back(std::make_pair(v - 1, w));
    // 如果是无向图，则取消下面这行的注释：
    // (*adj)[v - 1].push_back(std::make_pair(u - 1, w));
}

/**
 * @brief Dijkstra 算法核心实现
 * 计算从源节点 s 到目标节点 t 的最短路径长度。
 *
 * @param adj 图的邻接表表示
 * @param s 源节点索引 (已转换为从 0 开始)
 * @param t 目标节点索引 (已转换为从 0 开始)
 *
 * @return 如果 t 可达，返回最短路径值；如果不可达，则返回 -1。
 */
int dijkstra(std::vector<std::vector<std::pair<int, int>>> *adj, int s, int t) {
    /// n 代表图中顶点的个数
    int n = adj->size();

    /// 初始化所有节点的距离为无穷大 (INF)
    std::vector<int64_t> dist(n, INF);

    /// 创建一个最小堆优先队列
    /// pair 的第一个元素存放距离，第二个元素存放节点编号
    /// std::greater 比较器使得队列顶部始终是距离最小的节点
    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>,
                        std::greater<std::pair<int, int>>>
        pq;

    /// 将源节点以距离 0 推入优先队列
    pq.push(std::make_pair(0, s));

    /// 设置源节点到自身的距离为 0
    dist[s] = 0;

    while (!pq.empty()) {
        /// 获取当前队列顶端（距离最短）的节点
        int currentNode = pq.top().second;

        /// 当前节点的已知最短距离
        int currentDist = pq.top().first;

        pq.pop();

        // 如果弹出的节点距离已经大于记录的最短距离，说明是过期的失效记录，直接跳过
        if (currentDist > dist[currentNode]) continue;

        /// 遍历当前节点的所有邻接边，尝试进行松弛 (relaxation) 操作
        for (std::pair<int, int> edge : (*adj)[currentNode]) {
            int nextNode = edge.first;     // 邻接节点
            int edgeWeight = edge.second;  // 边权值

            /// 松弛操作：如果通过当前节点到达邻接节点的路径比已知路径更短
            if (currentDist + edgeWeight < dist[nextNode]) {
                dist[nextNode] = currentDist + edgeWeight; // 更新最短距离
                pq.push(std::make_pair(dist[nextNode], nextNode)); // 将更新后的节点和距离推入队列
            }
        }
    }
    
    // 如果目标节点的最短路径被更新过，说明可达，返回具体值
    if (dist[t] != INF) {
        return dist[t];
    }
    return -1; // 目标节点不可达
}
}  // namespace graph

/** 自测用例 */
void tests() {
    std::cout << "Initiatinig Predefined Tests..." << std::endl;
    std::cout << "Initiating Test 1..." << std::endl;
    std::vector<std::vector<std::pair<int, int>>> adj1(
        4, std::vector<std::pair<int, int>>());
    graph::addEdge(&adj1, 1, 2, 1);
    graph::addEdge(&adj1, 4, 1, 2);
    graph::addEdge(&adj1, 2, 3, 2);
    graph::addEdge(&adj1, 1, 3, 5);

    int s = 1, t = 3;
    assert(graph::dijkstra(&adj1, s - 1, t - 1) == 3);
    std::cout << "Test 1 Passed..." << std::endl;

    s = 4, t = 3;
    std::cout << "Initiating Test 2..." << std::endl;
    assert(graph::dijkstra(&adj1, s - 1, t - 1) == 5);
    std::cout << "Test 2 Passed..." << std::endl;

    std::vector<std::vector<std::pair<int, int>>> adj2(
        5, std::vector<std::pair<int, int>>());
    graph::addEdge(&adj2, 1, 2, 4);
    graph::addEdge(&adj2, 1, 3, 2);
    graph::addEdge(&adj2, 2, 3, 2);
    graph::addEdge(&adj2, 3, 2, 1);
    graph::addEdge(&adj2, 2, 4, 2);
    graph::addEdge(&adj2, 3, 5, 4);
    graph::addEdge(&adj2, 5, 4, 1);
    graph::addEdge(&adj2, 2, 5, 3);
    graph::addEdge(&adj2, 3, 4, 4);

    s = 1, t = 5;
    std::cout << "Initiating Test 3..." << std::endl;
    assert(graph::dijkstra(&adj2, s - 1, t - 1) == 6);
    std::cout << "Test 3 Passed..." << std::endl;
    std::cout << "All Test Passed..." << std::endl << std::endl;
}

/** 主函数，包含命令行交互式输入 */
int main() {
    // 运行定义好的自测案例
    tests();

    int vertices = int(), edges = int();
    std::cout << "Enter the number of vertices : ";
    std::cin >> vertices;
    std::cout << "Enter the number of edges : ";
    std::cin >> edges;

    std::vector<std::vector<std::pair<int, int>>> adj(
        vertices, std::vector<std::pair<int, int>>());

    int u = int(), v = int(), w = int();
    std::cout << "Enter edges (u, v, weight):" << std::endl;
    while (edges--) {
        std::cin >> u >> v >> w;
        graph::addEdge(&adj, u, v, w);
    }

    int s = int(), t = int();
    std::cout << "Enter source and target vertex to calculate shortest path: ";
    std::cin >> s >> t;
    int dist = graph::dijkstra(&adj, s - 1, t - 1);
    if (dist == -1) {
        std::cout << "Target not reachable from source" << std::endl;
    } else {
        std::cout << "Shortest Path Distance : " << dist << std::endl;
    }
    return 0;
}
