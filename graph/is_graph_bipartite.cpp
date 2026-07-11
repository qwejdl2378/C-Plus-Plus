/**
 * @file
 * @brief Algorithm to check whether an undirected graph is [bipartite](https://en.wikipedia.org/wiki/Bipartite_graph) (基于 BFS 染色法检测二分图算法实现)
 *
 * @details
 * 二分图（Bipartite Graph）：如果一个无向图的顶点集可以分割为两个互不相交的子集 U 和 V，
 * 使得图中的每条边连接的两个顶点分别属于这两个不同的顶点集，则称该图为二分图。
 * 等价于：图中不存在奇数长度的环。
 *
 * ### 染色判定算法（BFS 染色法）
 * 1. 使用 `side` 数组记录每个顶点的染色状态（`0` 和 `1` 代表两类不同的子集，`-1` 代表未着色）。
 * 2. 遍历所有顶点，若遇到未着色的顶点，以其为起点进行 BFS 遍历：
 *    - 将起点染色为 `0` 并入队。
 *    - 依次弹出队首节点 `current`，对其所有邻接点 `neighbour` 进行检查：
 *      - 若 `neighbour` 尚未着色，将其染成与当前节点相反的颜色 `1 ^ side[current]`，并入队。
 *      - 若 `neighbour` 已经着色且颜色与当前节点相同，说明存在染色冲突，判定该图不是二分图。
 * 3. 如果整个图的节点染色完毕且无冲突，则该图是二分图。
 *
 * @note
 * 【微优化建议】：
 * 在第 121 行中，当检测到颜色冲突时，代码使用了按位与操作 `check &= (side[neighbour] != side[current]);`。
 * 虽然计算结果正确，但程序仍会继续遍历队列和剩余节点。
 * 实际上，一旦检测到冲突（即 `side[neighbour] == side[current]`），可以**直接 return false** 提前结束函数，以节省计算资源。
 *
 * 时间复杂度: O(V + E)
 * 空间复杂度: O(V)
 *
 * @author [Akshat Vaya](https://github.com/AkVaya)
 */

#include <iostream>
#include <queue>
#include <vector>

/**
 * @namespace graph
 * @brief 图算法命名空间
 */
namespace graph {
/**
 * @namespace is_graph_bipartite
 * @brief 检测二分图相关算法命名空间
 */
namespace is_graph_bipartite {
/**
 * @class Graph
 * @brief 邻接表形式的图表示类
 */
class Graph {
 private:
    int n;  ///< 图中顶点的数量
    std::vector<std::vector<int> > adj;  ///< 邻接表
    std::vector<int> side;  ///< 染色记录表（0, 1 或 -1）

 public:
    /**
     * @brief 构造函数
     * @param size 顶点数
     */
    explicit Graph(int size) {
        n = size;
        adj.resize(n);
        side.resize(n, -1); // 默认初始值为 -1（未着色）
    }

    void addEdge(int u, int v);  ///< 添加无向边

    bool is_bipartite();  ///< 判断是否为二分图
};

/**
 * @brief 添加无向边，输入参数为 1-indexed 顶点编号
 */
void Graph::addEdge(int u, int v) {
    adj[u - 1].push_back(v - 1);
    adj[v - 1].push_back(u - 1);
}

/**
 * @brief 二分图判定核心算法
 * @returns `true` 是二分图；`false` 否则
 */
bool Graph::is_bipartite() {
    bool check = true;
    std::queue<int> q;
    
    // 循环遍历所有连通分量，处理非连通图
    for (int current_edge = 0; current_edge < n; ++current_edge) {
        if (side[current_edge] == -1) {
            q.push(current_edge);
            side[current_edge] = 0; // 染色为 0

            while (q.size()) {
                int current = q.front();
                q.pop();
                for (auto neighbour : adj[current]) {
                    if (side[neighbour] == -1) {
                        // 染色相反颜色
                        side[neighbour] = (1 ^ side[current]);
                        q.push(neighbour);
                    } else {
                        // 若颜色冲突，check 变为 false
                        check &= (side[neighbour] != side[current]);
                    }
                }
            }
        }
    }
    return check;
}
}  // namespace is_graph_bipartite
}  // namespace graph

/**
 * @brief 单元自测用例
 */
static void test() {
    // 创建二分图 G1:
    //     1   4
    //    / \ / \
    //   2   3   5
    graph::is_graph_bipartite::Graph G1(5);
    G1.addEdge(1, 2);
    G1.addEdge(1, 3);
    G1.addEdge(3, 4);
    G1.addEdge(4, 5);

    // 创建非二分图 G2（包含 3 节点的奇数环）：
    //   1 --- 2
    //    \   /
    //      3
    graph::is_graph_bipartite::Graph G2(3);
    G2.addEdge(1, 2);
    G2.addEdge(1, 3);
    G2.addEdge(2, 3);

    // 输出判定结果
    if (G1.is_bipartite()) {
        std::cout << "The given graph G1 is a bipartite graph\n";
    } else {
        std::cout << "The given graph G1 is not a bipartite graph\n";
    }
    if (G2.is_bipartite()) {
        std::cout << "The given graph G2 is a bipartite graph\n";
    } else {
        std::cout << "The given graph G2 is not a bipartite graph\n";
    }
}

/**
 * @brief 主函数
 */
int main() {
    test();  // 运行自测
    return 0;
}
