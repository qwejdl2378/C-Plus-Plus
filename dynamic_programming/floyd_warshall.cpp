/**
 * @file
 * @brief Implementation of the Floyd-Warshall All-Pairs Shortest Path Algorithm (弗洛伊德-沃舍尔所有点对最短路径算法)
 * @details
 * 弗洛伊德-沃舍尔（Floyd-Warshall）算法用于寻找加权图中所有顶点对之间的最短路径。
 * 它可以处理有向图或负权重的边，但不能处理负权回路。
 *
 * ### 动态规划状态转移
 * 设 `dist[i][j]` 为顶点 `i` 到顶点 `j` 的当前最短距离。
 * - 引入一个中间节点 `k`，对每一对顶点 `(i, j)` 判断是否能够通过 `k` 缩短距离：
 *   `dist[i][j] = min(dist[i][j], dist[i][k] + dist[k][j])`
 *
 * @note
 * 【严重内存崩溃 Double-Free Bug 说明】：
 * 在第 57 行中，核心计算函数被定义为：
 *   `void FloydWarshall(Graph graph)`
 * 这里 `Graph` 对象被**按值传递**（Pass-by-value）。
 * 1. 默认的拷贝构造函数只会浅拷贝（Shallow Copy）指针 `int **edges`，使得副本与原对象指向同一块动态内存。
 * 2. 当 `FloydWarshall` 执行完毕函数返回时，副本 `graph` 被析构，触发 `~Graph()`，从而释放了 `edges` 的内存。
 * 3. 此时，在 `main` 函数中的原始对象 `G` 的 `edges` 指针变为了悬空指针（Dangling Pointer）。
 * 4. 当 `main` 执行完毕退出时，原始对象 `G` 再次析构，对已经释放过的地址执行 `delete[]`，导致经典的 **Double Free** 内存损坏并使程序崩溃。
 *
 * 修正方法：应当改用引用传递：`void FloydWarshall(const Graph &graph)` 或者重写 `Graph` 的拷贝构造函数。
 *
 * 时间复杂度: O(V^3)
 * 空间复杂度: O(V^2)
 */

#include <climits>
#include <cstddef>
#include <iostream>
#include <vector>

using std::cin;
using std::cout;
using std::endl;

/**
 * @brief 存储图邻接矩阵的类
 */
class Graph {
 public:
    int vertexNum;
    int **edges; // 警告：使用原始指针且没有实现深拷贝构造函数，会导致 double-free

    /**
     * @brief 构造函数：初始化邻接矩阵，对角线设为 0，其他设为 INT_MAX (无穷大)
     * @param V 顶点数量
     */
    Graph(int V) {
        this->vertexNum = V;
        this->edges = new int *[V];
        for (int i = 0; i < V; i++) {
            this->edges[i] = new int[V];
            for (int j = 0; j < V; j++) this->edges[i][j] = INT_MAX;
            this->edges[i][i] = 0;
        }
    }

    /**
     * @brief 析构函数：释放动态分配的内存
     */
    ~Graph() {
        for (int i = 0; i < vertexNum; i++) {
            delete[] edges[i];
        }
        delete[] edges;
    }

    /**
     * @brief 添加有向加权边
     */
    void addEdge(int src, int dst, int weight) {
        this->edges[src][dst] = weight;
    }
};

/**
 * @brief 辅助函数：以矩阵形式打印距离值
 */
void print(const std::vector<int>& dist, int V) {
    cout << "\nThe Distance matrix for Floyd - Warshall" << endl;
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            if (dist[i * V + j] != INT_MAX)
                cout << dist[i * V + j] << "\t";
            else
                cout << "INF"
                     << "\t";
        }
        cout << endl;
    }
}

/**
 * @brief 执行弗洛伊德-沃舍尔算法计算最短距离
 * @param graph 输入图对象（按值传递会导致 double-free）
 */
void FloydWarshall(Graph graph) {
    std::size_t V = graph.vertexNum;
    std::vector<std::vector<int> > dist(V, std::vector<int>(V));

    // 1. 初始化距离矩阵
    for (int i = 0; i < V; i++)
        for (int j = 0; j < V; j++) dist[i][j] = graph.edges[i][j];

    // 2. 三重循环执行松弛操作：k 为中间点，i 为源点，j 为终点
    for (int k = 0; k < V; k++)
        for (int i = 0; i < V; i++)
            for (int j = 0; j < V; j++)
                if (dist[i][k] != INT_MAX && dist[k][j] != INT_MAX &&
                    dist[i][k] + dist[k][j] < dist[i][j])
                    dist[i][j] = dist[i][k] + dist[k][j];

    // 将二维矩阵扁平化为一维以供打印输出
    std::vector<int> dist1d(V * V);
    for (int i = 0; i < V; i++)
        for (int j = 0; j < V; j++) dist1d[i * V + j] = dist[i][j];

    print(dist1d, V);
}

/**
 * @brief 主函数
 */
int main() {
    int V, E;
    int src, dst, weight;
    cout << "Enter number of vertices: ";
    cin >> V;
    cout << "Enter number of edges: ";
    cin >> E;
    Graph G(V);
    for (int i = 0; i < E; i++) {
        cout << "\nEdge " << i + 1 << "\nEnter source: ";
        cin >> src;
        cout << "Enter destination: ";
        cin >> dst;
        cout << "Enter weight: ";
        cin >> weight;
        G.addEdge(src, dst, weight);
    }
    FloydWarshall(G); // 警告：调用此函数会导致 G 拥有的内存在返回时被析构释放一次，产生 double-free

    return 0;
}
