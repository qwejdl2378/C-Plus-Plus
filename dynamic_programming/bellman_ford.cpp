/**
 * @file
 * @brief Implementation of Bellman-Ford CPU shortest path algorithm (贝尔曼-福特单源最短路径算法实现)
 * @details
 * 贝尔曼-福特（Bellman-Ford）算法用于在加权图中寻找单源最短路径。
 * 与迪杰斯特拉（Dijkstra）算法相比，它不仅能处理负权重边，还能检测图中是否存在负圈（Negative Cycle）。
 * 
 * ### 动态规划递推原理
 * 设 `dist[v]` 为源点到顶点 `v` 的最短距离。
 * 对图中的所有边进行 `V-1` 轮松弛操作（Relaxation）：
 *   dist[v] = min(dist[v], dist[u] + weight(u, v))
 * 如果在第 V 轮松弛中依然能使某些距离变短，说明图中存在负权回路。
 * 
 * @note
 * 【严重内存越界 Bug 说明】：
 * 该代码中存在两处严重的 C++ 标准库 vector 越界 Bug（未实际初始化 size 就使用下标访问）：
 * 1. 在 `Graph` 的构造函数中，使用 `edges.reserve(E)`。但在 `addEdge` 中使用 `this->edges[edgeInd++]` 写入。
 *    `reserve` 仅仅分配物理内存空间，并没有改变 vector 的大小（size 仍然为 0）。直接使用下标属于未定义行为（Undefined Behavior），会导致崩溃。
 *    应修改为：`edges.resize(E)` 或者直接使用 `edges.push_back()`。
 * 2. 在 `BellmanFord` 函数中，使用 `dist.reserve(E)`，然后使用 `dist[i] = INT_MAX` 循环初始化前 V 个元素，
 *    同样造成了越界写入。
 *    应修改为：`std::vector<int> dist(V, INT_MAX);`。
 */

#include <climits>
#include <iostream>
#include <vector>

using namespace std;

/**
 * @brief 存储边信息的辅助类
 */
class Edge {
 public:
    int src, dst, weight;
};

/**
 * @brief 存储图结构的类
 */
class Graph {
 public:
    int vertexNum, edgeNum;
    std::vector<Edge> edges;

    /**
     * @brief 构造函数
     * @param V 顶点数量
     * @param E 边的数量
     */
    Graph(int V, int E) {
        this->vertexNum = V;
        this->edgeNum = E;
        this->edges.reserve(E); // 警告 Bug：仅预留了空间，size 仍为 0
    }

    /**
     * @brief 添加边
     */
    void addEdge(int src, int dst, int weight) {
        static int edgeInd = 0;
        if (edgeInd < this->edgeNum) {
            Edge newEdge;
            newEdge.src = src;
            newEdge.dst = dst;
            newEdge.weight = weight;
            this->edges[edgeInd++] = newEdge; // 警告 Bug：下标越界写入
        }
    }
};

/**
 * @brief 打印最短路径距离
 */
void print(const std::vector<int>& dist, int V) {
    cout << "\nVertex  Distance" << endl;
    for (int i = 0; i < V; i++) {
        if (dist[i] != INT_MAX)
            cout << i << "\t" << dist[i] << endl;
        else
            cout << i << "\tINF" << endl;
    }
}

/**
 * @brief 贝尔曼-福特算法核心计算逻辑
 * @param graph 输入的图结构
 * @param src 源顶点 ID
 */
void BellmanFord(Graph graph, int src) {
    int V = graph.vertexNum;
    int E = graph.edgeNum;
    std::vector<int> dist;
    dist.reserve(E); // 警告 Bug：仅预留空间，未分配 size

    // 初始化所有节点距离为无穷大，源点为 0
    for (int i = 0; i < V; i++) dist[i] = INT_MAX; // 警告 Bug：下标越界写入
    dist[src] = 0;

    // 执行 V-1 轮松弛操作
    for (int i = 0; i <= V - 1; i++)
        for (int j = 0; j < E; j++) {
            int u = graph.edges[j].src;
            int v = graph.edges[j].dst;
            int w = graph.edges[j].weight;

            if (dist[u] != INT_MAX && dist[u] + w < dist[v])
                dist[v] = dist[u] + w;
        }

    // 第 V 轮检测，如果还能松弛，说明存在负权回路
    for (int j = 0; j < E; j++) {
        int u = graph.edges[j].src;
        int v = graph.edges[j].dst;
        int w = graph.edges[j].weight;

        if (dist[u] != INT_MAX && dist[u] + w < dist[v]) {
            cout << "Graph contains negative weight cycle. Hence, shortest "
                    "distance not guaranteed."
                 << endl;
            return;
        }
    }

    print(dist, V);
    return;
}

/**
 * @brief 主函数
 */
int main() {
    int V, E, gsrc;
    int src, dst, weight;
    cout << "Enter number of vertices: ";
    cin >> V;
    cout << "Enter number of edges: ";
    cin >> E;
    Graph G(V, E);
    for (int i = 0; i < E; i++) {
        cout << "\nEdge " << i + 1 << "\nEnter source: ";
        cin >> src;
        cout << "Enter destination: ";
        cin >> dst;
        cout << "Enter weight: ";
        cin >> weight;
        G.addEdge(src, dst, weight);
    }
    cout << "\nEnter source: ";
    cin >> gsrc;
    BellmanFord(G, gsrc);

    return 0;
}
