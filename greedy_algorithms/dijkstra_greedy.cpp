/**
 * @file
 * @brief Implementation of [Dijkstra's Shortest Path](https://en.wikipedia.org/wiki/Dijkstra%27s_algorithm) algorithm (Dijkstra 单源最短路径贪心算法实现)
 *
 * @details
 * Dijkstra 算法是典型的贪心算法（Greedy Algorithm）应用：
 * 1. 它维护两个集合：已求出最短路径的顶点集合（`vset`）和未求出最短路径的顶点集合。
 * 2. 局部贪心策略：每一阶段，在所有未加入 `vset` 的顶点中，选取一个与起点当前估算距离最小的顶点 `u` 加入 `vset`。
 * 3. 路径松弛（Relaxation）：以顶点 `u` 为媒介，更新与其相邻的未确定顶点的最短估算距离：
 *    `if (mdist[u] + weight(u, v) < mdist[v]) { mdist[v] = mdist[u] + weight(u, v); }`
 *
 * 时间复杂度: $O(V^2)$ (由于没有使用优先队列，每次寻找极值需要 $O(V)$ 时间，适用于稠密图)
 * 空间复杂度: $O(V^2)$ (邻接矩阵存储结构)
 *
 * @note
 * 【C++ 空 Vector 越界写内存崩溃与参数值拷贝效率 Bug 审计与修复】：
 * 1. **空 Vector 越界写崩溃 Bug**：在 `dijkstra` 函数第 126 行中，
 *    代码使用 `std::vector<int> mdist{};` 声明了两个容量为 0 的空容器，
 *    随后却通过 `mdist[i] = INT_MAX;` 和 `vset[i] = false;` 进行强行下标写入。
 *    在 C++ 中，这会导致严重的**越界非法内存写入（Segmentation Fault）**崩溃。原测试代码由于从未调用过 `dijkstra` 函数，因此避开了此崩溃。
 *    **修复**：在构造时显式指定容器大小为 `V`：`std::vector<int> mdist(V, INT_MAX);`。
 * 2. **参数值拷贝（Pass by Value）效率缺陷**：原代码在多个接口中直接按值传递 `Graph` 和 `vector`。
 *    每次调用都会进行全图、全数组的深拷贝，造成巨大的无谓内存和 CPU 浪费。
 *    **修复**：改用常量引用传递（`const Graph&`，`const std::vector<T>&`）。
 * 3. **自测覆盖率不足**：原本的 `tests()` 仅仅测试了建边，完全没有执行最核心的 `dijkstra`。
 *    **修复**：在 `tests()` 尾部添加了对 `dijkstra` 的实际运算逻辑验证，确保修复后的算法能安全并正确执行。
 *
 * @author [David Leal](https://github.com/Panquesito7)
 * @author [Arpan Jain](https://github.com/arpanjain97)
 */

#include <cassert>   /// 用于 assert 断言
#include <climits>   /// 用于 INT_MAX
#include <iostream>  /// 用于标准输出
#include <vector>    /// 用于 std::vector

/**
 * @namespace greedy_algorithms
 * @brief 贪心算法命名空间
 */
namespace greedy_algorithms {
/**
 * @namespace dijkstra
 * @brief Dijkstra 算法相关命名空间
 */
namespace dijkstra {

/**
 * @brief 邻接矩阵表示的图结构包装类
 */
class Graph {
 public:
    int vertexNum = 0; ///< 顶点数
    std::vector<std::vector<int>> edges{}; ///< 邻接矩阵，edges[u][v] 表示 u->v 边的权重

    /**
     * @brief 构造函数
     * @param V 顶点数量
     */
    explicit Graph(const int V) {
        // 核心修复：直接使用 vector 的填充构造函数，优雅且高效地初始化零矩阵
        this->edges = std::vector<std::vector<int>>(V, std::vector<int>(V, 0));
        this->vertexNum = V;
    }

    /**
     * @brief 向图中添加一条边（有向边，若为无向图需双向添加）
     * @param src 起点索引
     * @param dst 终点索引
     * @param weight 边权重
     */
    void add_edge(int src, int dst, int weight) {
        this->edges[src][dst] = weight;
    }
};

/**
 * @brief 辅助贪心选择：在未标记集合中选择当前估算距离最小的顶点
 * @param mdist 距离数组引用
 * @param vset 已访问集合标记数组引用
 * @param V 顶点数量
 * @returns 选出的最小距离节点索引
 */
int minimum_distance(const std::vector<int>& mdist, const std::vector<bool>& vset, int V) {
    int minVal = INT_MAX, minInd = 0;
    for (int i = 0; i < V; i++) {
        if (!vset[i] && (mdist[i] < minVal)) {
            minVal = mdist[i];
            minInd = i;
        }
    }
    return minInd;
}

/**
 * @brief 打印节点的最短路径结果
 */
void print(const std::vector<int>& dist, int V) {
    std::cout << "\nVertex  Distance\n";
    for (int i = 0; i < V; i++) {
        if (dist[i] < INT_MAX) {
            std::cout << i << "\t" << dist[i] << "\n";
        } else {
            std::cout << i << "\tINF\n";
        }
    }
}

/**
 * @brief Dijkstra 算法主入口
 * @param graph 图的引用
 * @param src 起始源点索引
 * @returns 返回从 src 到所有顶点的最短距离数组
 */
std::vector<int> dijkstra(const Graph& graph, int src) {
    int V = graph.vertexNum;
    
    // 核心修复：构造时指定大小并初始化，解决越界写内存导致的段错误崩溃
    std::vector<int> mdist(V, INT_MAX);  // 记录从 src 到各顶点的最短估算距离，初值无穷大
    std::vector<bool> vset(V, false);    // 记录顶点是否已被选入最短路径树

    mdist[src] = 0; // 起点到自身距离为 0

    // 迭代寻找 V-1 轮
    for (int count = 0; count < V - 1; count++) {
        // 贪心选择当前未处理的距离最小顶点
        int u = minimum_distance(mdist, vset, V);
        vset[u] = true; // 加入最短路径树中

        // 松弛与 u 相连的邻接顶点
        for (int v = 0; v < V; v++) {
            if (!vset[v] && graph.edges[u][v] &&
                mdist[u] != INT_MAX && // 防止无穷大溢出
                mdist[u] + graph.edges[u][v] < mdist[v]) {
                mdist[v] = mdist[u] + graph.edges[u][v];
            }
        }
    }

    print(mdist, V);
    return mdist;
}

}  // namespace dijkstra
}  // namespace greedy_algorithms

/**
 * @brief 单元自测用例
 */
static void tests() {
    greedy_algorithms::dijkstra::Graph graph(8);

    // 添加一些双向边
    graph.add_edge(0, 1, 1);
    graph.add_edge(1, 0, 1);

    graph.add_edge(0, 2, 7);
    graph.add_edge(2, 0, 7);

    graph.add_edge(1, 2, 1);
    graph.add_edge(2, 1, 1);

    graph.add_edge(1, 3, 3);
    graph.add_edge(3, 1, 3);

    graph.add_edge(1, 4, 2);
    graph.add_edge(4, 1, 2);

    graph.add_edge(2, 3, 2);
    graph.add_edge(3, 2, 2);

    graph.add_edge(6, 2, 4);
    graph.add_edge(2, 6, 4);

    // 基础边结构验证
    assert(graph.edges[6][2] == 4);
    assert(graph.edges[0][1] == 1);
    assert(graph.edges[0][2] == 7);
    assert(graph.edges[1][3] == 3);

    // 核心修复：实际执行 Dijkstra 最短路查找并断言验证其正确性，防御未来的 regressions
    std::vector<int> distances = greedy_algorithms::dijkstra::dijkstra(graph, 0);
    
    // 0 到各顶点的最短路径值预期：
    // 0 -> 0 : 0
    // 0 -> 1 : 1
    // 0 -> 1 -> 2 : 2 (比 0 -> 2 的权重 7 更优)
    // 0 -> 1 -> 4 : 3
    // 0 -> 1 -> 2 -> 3 : 4
    assert(distances[0] == 0);
    assert(distances[1] == 1);
    assert(distances[2] == 2);
    assert(distances[4] == 3);
    assert(distances[3] == 4);

    std::cout << "All Dijkstra tests have successfully passed!\n";
}

/**
 * @brief 主函数
 */
int main() {
    tests(); // 运行测试用例
    return 0;
}
