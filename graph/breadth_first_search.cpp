/**
 *
 * \file
 * \brief [Breadth First Search Algorithm (Breadth First Search)](https://en.wikipedia.org/wiki/Breadth-first_search) (广度优先搜索 BFS 算法实现)
 *
 * \author [Ayaan Khan](https://github.com/ayaankhan98)
 * \author [Aman Kumar Pandey](https://github.com/gpamangkp)
 *
 *
 * \details
 * 广度优先搜索 (BFS) 是一种经典的图遍历算法。
 * 时间复杂度为 $O(|V| + |E|)$，其中 V 是顶点的个数，E 是图中的边数。
 *
 * BFS 的典型应用场景包括：
 * 1. 寻找无权图两顶点间的最短路径（以边数计）（相比 DFS，这是 BFS 的一大优势）
 * 2. Ford-Fulkerson 算法中寻找增广路径以计算流网络的最大流（Edmonds-Karp 算法）
 * 3. 检测图是否是二分图 (Bipartite Graph)
 * 4. 垃圾回收中的 Cheney 拷贝算法
 *
 * <h4>工作原理</h4>
 * 在下面的实现中，我们使用邻接表（通过 std::map 和 std::list 容器）来构建图结构。
 * BFS 过程如下：
 * 它需要一个指定的起始顶点作为探索起点。
 * 我们维护一个布尔 map 或向量来记录各顶点是否被访问过，避免重复访问甚至陷入死循环。
 * 同时配合使用一个辅助队列 (Queue)。
 *
 * 1. 首先将起点标记为已访问 (visited)。
 * 2. 将起点推入辅助队列中。
 * 3. 只要队列不为空，重复执行以下步骤：
 *      1. 从队列头部取出一个节点并弹出。
 *      2. 遍历该节点的所有邻居。
 *         如果邻居未被访问，则将其推入队列，并标记为已访问。
 *
 */
#include <algorithm>
#include <cassert>
#include <iostream>
#include <list>
#include <map>
#include <queue>
#include <string>

/**
 * \namespace graph
 * \brief 图算法命名空间
 */
namespace graph {
/* Graph 类定义 */
template <typename T>
class Graph {
    /**
     * adjacency_list 邻接表表示
     * 将每个顶点映射到其邻居节点的链表 (list) 中。
     */
    std::map<T, std::list<T> > adjacency_list;

 public:
    Graph() = default;
    
    /**
     * @brief 添加边
     * @param u 顶点 u
     * @param v 顶点 v
     * @param bidir 是否是双向边 (无向图默认位 true，有向图设为 false)
     */
    void add_edge(T u, T v, bool bidir = true) {
        adjacency_list[u].push_back(v);  // 添加 u --> v 边
        if (bidir == true) {
            // 如果是无向图，则反向也添加边
            adjacency_list[v].push_back(u);  // 添加 v --> u 边
        }
    }

    /**
     * @brief 执行广度优先搜索 (BFS)
     * 遍历可达节点，并返回一个映射表，标识每个节点是否被访问。
     * 
     * @param src 起始源节点
     * @return 映射表 std::map<T, bool>，键为节点，值为是否已被访问的布尔值
     */
    std::map<T, bool> breadth_first_search(T src) {
        /// 用于记录所有节点访问状态的映射表
        std::map<T, bool> visited;
        
        /// 初始将图中所有的顶点均设为未访问 (false)
        for (auto const &adjlist : adjacency_list) {
            visited[adjlist.first] = false;
            for (auto const &node : adjacency_list[adjlist.first]) {
                visited[node] = false;
            }
        }

        /// 辅助队列，用于存储待访问的节点
        std::queue<T> tracker;

        /// 将起点推入队列中以启动遍历
        tracker.push(src);
        /// 标记起点为已访问
        visited[src] = true;
        
        while (!tracker.empty()) {
            /// 从队列前部取出当前探索节点
            T node = tracker.front();
            /// 将该节点弹出队列
            tracker.pop();
            
            // 遍历当前节点的所有邻居
            for (T const &neighbour : adjacency_list[node]) {
                /// 如果邻居节点未被访问，则将其加入队列，并标记为已访问
                if (!visited[neighbour]) {
                    tracker.push(neighbour);
                    visited[neighbour] = true;
                }
            }
        }
        return visited; // 返回访问标记字典
    }
};
/* Class Graph 定义结束 */
}  // namespace graph

/** 自测用例集 */
static void tests() {
    /// 测试用例 1 (无向图，整型节点)
    graph::Graph<int> g;
    std::map<int, bool> correct_result;
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(2, 3);
    correct_result[0] = true;
    correct_result[1] = true;
    correct_result[2] = true;
    correct_result[3] = true;

    // 从 2 开始搜索，图是连通的，所有节点均应可达
    std::map<int, bool> returned_result = g.breadth_first_search(2);

    assert(returned_result == correct_result);
    std::cout << "Test 1 Passed..." << std::endl;

    /// 测试用例 2 (从 0 开始搜索)
    returned_result = g.breadth_first_search(0);

    assert(returned_result == correct_result);
    std::cout << "Test 2 Passed..." << std::endl;

    /// 测试用例 3 (有向图，字符串类型节点)
    graph::Graph<std::string> g2;

    g2.add_edge("Gorakhpur", "Lucknow", false);
    g2.add_edge("Gorakhpur", "Kanpur", false);
    g2.add_edge("Lucknow", "Agra", false);
    g2.add_edge("Kanpur", "Agra", false);
    g2.add_edge("Lucknow", "Prayagraj", false);
    g2.add_edge("Agra", "Noida", false);

    std::map<std::string, bool> correct_res;
    // 从 "Kanpur" 出发进行有向搜索
    std::map<std::string, bool> returned_res =
        g2.breadth_first_search("Kanpur");
    correct_res["Gorakhpur"] = false;
    correct_res["Lucknow"] = false;
    correct_res["Kanpur"] = true;
    correct_res["Agra"] = true;
    correct_res["Prayagraj"] = false;
    correct_res["Noida"] = true;
    
    assert(correct_res == returned_res);
    std::cout << "Test 3 Passed..." << std::endl;
}

/** 主函数 */
int main() {
    // 运行自测
    tests();
    
    size_t edges = 0;
    std::cout << "Enter the number of edges: ";
    std::cin >> edges;

    graph::Graph<int> g;

    std::cout << "Enter space-separated pairs of vertices that form edges (e.g., u v): "
              << std::endl;
    while (edges--) {
        int u = 0, v = 0;
        std::cin >> u >> v;
        g.add_edge(u, v);
    }

    // 从 0 开始搜索
    g.breadth_first_search(0);
    return 0;
}
