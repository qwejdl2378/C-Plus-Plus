/**
 * @file cycle_check_directed_graph.cpp
 * @brief BFS (Kahn's Algorithm) and DFS (Three-color states) algorithms to check for cycle in a directed graph (有向图环路检测算法实现)
 *
 * @details
 * 本文件提供了两种检测有向图是否存在环路的方法：
 *
 * ### 1. 深度优先搜索（DFS - 三色标记法）
 * - 引入三个状态标记：
 *   - `not_visited` (0)：白色，表示节点尚未被访问。
 *   - `in_stack` (1)：灰色，表示节点已被访问，且正在 DFS 递归调用栈中（正在被探索）。
 *   - `visited` (2)：黑色，表示节点的全部后代节点均已被探索完毕，并已退出递归栈。
 * - 环路判定：在 DFS 遍历邻居节点时，如果遇到一个状态为 `in_stack`（灰色）的节点，
 *   说明在 DFS 树中存在一条指向祖先节点的**后向边（Back Edge）**，即检测到有向环路。
 *
 * ### 2. 广度优先搜索（BFS - Kahn 拓扑排序法）
 * - 基于入度（Indegree）的拓扑排序检测：
 *   - 统计每个顶点的入度，将所有入度为 0 的节点加入队列 `can_be_solved`。
 *   - 循环出队，每次出队一个节点，将其可到达的子节点入度减 1。若某个子节点入度降为 0，则加入队列。
 *   - 使用一个计数器跟踪遍历到的节点数。如果最终遍历过的节点数不等于图中的顶点总数，
 *     说明图中存在由于环路引起的无法消除入度（环中节点入度不可能归 0）的现象，即有向环路存在。
 *
 * 时间复杂度: DFS 为 O(V + E)；BFS 为 O(V + E)
 * 空间复杂度: O(V + E)
 *
 * @author [Anmol3299](mailto:mittalanmol22@gmail.com)
 */

#include <cstdint>
#include <iostream>     // 用于 std::cout
#include <map>          // 用于 std::map
#include <queue>        // 用于 std::queue
#include <stdexcept>    // 用于异常抛出
#include <type_traits>  // 用于 std::remove_reference
#include <utility>      // 用于 std::move
#include <vector>       // 用于 std::vector

/**
 * @brief 表示有向图中非加权边的结构体
 */
struct Edge {
    unsigned int src;  // 源节点
    unsigned int dest; // 目标节点

    Edge() = delete;
    ~Edge() = default;
    Edge(Edge&&) = default;
    Edge& operator=(Edge&&) = default;
    Edge(Edge const&) = default;
    Edge& operator=(Edge const&) = default;

    /**
     * @brief 构造函数
     * @param source 起点
     * @param destination 终点
     */
    Edge(unsigned int source, unsigned int destination)
        : src(source), dest(destination) {}
};

using AdjList = std::map<unsigned int, std::vector<unsigned int>>;

/**
 * @class Graph
 * @brief 用邻接表表示的有向图类
 */
class Graph {
 public:
    Graph() : m_adjList({}) {}
    ~Graph() = default;
    Graph(Graph&&) = default;
    Graph& operator=(Graph&&) = default;
    Graph(Graph const&) = default;
    Graph& operator=(Graph const&) = default;

    /**
     * @brief 构造函数：指定顶点数和邻接表
     */
    Graph(unsigned int vertices, AdjList adjList)
        : m_vertices(vertices), m_adjList(std::move(adjList)) {}

    /**
     * @brief 移动构造函数：指定顶点数和右值邻接表
     */
    Graph(unsigned int vertices, AdjList&& adjList)
        : m_vertices(vertices), m_adjList(std::move(adjList)) {}

    /**
     * @brief 构造函数：根据边集列表构建邻接表
     */
    Graph(unsigned int vertices, std::vector<Edge> const& edges)
        : m_vertices(vertices) {
        for (auto const& edge : edges) {
            if (edge.src >= vertices || edge.dest >= vertices) {
                throw std::range_error(
                    "Either src or dest of edge out of range");
            }
            m_adjList[edge.src].emplace_back(edge.dest);
        }
    }

    /**
     * @brief 获取邻接表的只读引用
     */
    std::remove_reference<AdjList>::type const& getAdjList() const {
        return m_adjList;
    }

    /**
     * @brief 获取图的顶点数
     */
    unsigned int getVertices() const { return m_vertices; }

    /**
     * @brief 动态添加顶点
     */
    void addVertices(unsigned int num = 1) { m_vertices += num; }

    /**
     * @brief 添加边对象
     */
    void addEdge(Edge const& edge) {
        if (edge.src >= m_vertices || edge.dest >= m_vertices) {
            throw std::range_error("Either src or dest of edge out of range");
        }
        m_adjList[edge.src].emplace_back(edge.dest);
    }

    /**
     * @brief 根据起终点添加有向边
     */
    void addEdge(unsigned int source, unsigned int destination) {
        if (source >= m_vertices || destination >= m_vertices) {
            throw std::range_error(
                "Either source or destination of edge out of range");
        }
        m_adjList[source].emplace_back(destination);
    }

 private:
    unsigned int m_vertices = 0; // 顶点数
    AdjList m_adjList;           // 邻接表 map
};

/**
 * @class CycleCheck
 * @brief 提供有向图环路检测算法的类
 */
class CycleCheck {
 private:
    // 三色标记状态枚举值
    enum nodeStates : uint8_t { not_visited = 0, in_stack, visited };

    /**
     * @brief DFS 递归搜索辅助函数
     * @param adjList 邻接表
     * @param state 记录各节点三色状态的向量指针
     * @param node 当前访问的节点
     * @returns 若有环则返回 true
     */
    static bool isCyclicDFSHelper(AdjList const& adjList,
                                  std::vector<nodeStates>* state,
                                  unsigned int node) {
        // 标记为正在探索中 (灰色)
        (*state)[node] = in_stack;

        auto const it = adjList.find(node);
        if (it != adjList.end()) {
            for (auto child : it->second) {
                auto state_of_child = (*state)[child];
                
                // 子节点尚未访问过，递归检测
                if (state_of_child == not_visited) {
                    if (isCyclicDFSHelper(adjList, state, child)) {
                        return true;
                    }
                } 
                // 如果子节点状态是在栈中 (灰色)，说明形成了后向边环路，返回 true
                else if (state_of_child == in_stack) {
                    return true;
                }
            }
        }

        // 该节点的所有后代都探索完，标记为完全访问完毕 (黑色)
        (*state)[node] = visited;
        return false;
    }

 public:
    /**
     * @brief 基于 DFS 查找图是否有环
     * @param graph 待评测的图
     * @returns 若检测到环路则返回 true
     */
    static bool isCyclicDFS(Graph const& graph) {
        auto vertices = graph.getVertices();
        std::vector<nodeStates> state(vertices, not_visited); // 初始全为未访问

        for (unsigned int node = 0; node < vertices; node++) {
            if (state[node] == not_visited) {
                if (isCyclicDFSHelper(graph.getAdjList(), &state, node)) {
                    return true;
                }
            }
        }
        return false;
    }

    /**
     * @brief 基于 BFS (Kahn 拓扑排序) 查找图是否有环
     * @param graph 待评测的图
     * @returns 若检测到环路则返回 true
     */
    static bool isCyclicBFS(Graph const& graph) {
        auto graphAjdList = graph.getAdjList();
        auto vertices = graph.getVertices();

        std::vector<unsigned int> indegree(vertices, 0);
        // 1. 统计所有节点的入度
        for (auto const& list : graphAjdList) {
            auto children = list.second;
            for (auto const& child : children) {
                indegree[child]++;
            }
        }

        // 2. 将所有入度为 0 的节点压入可解决队列
        std::queue<unsigned int> can_be_solved;
        for (unsigned int node = 0; node < vertices; node++) {
            if (!indegree[node]) {
                can_be_solved.emplace(node);
            }
        }

        auto remain = vertices; // 剩余需要拓扑遍历的顶点数
        
        // 3. 执行类似拓扑排序的入度削减
        while (!can_be_solved.empty()) {
            auto solved = can_be_solved.front();
            can_be_solved.pop();
            remain--; // 遍历到了一个节点，未处理项减 1

            auto it = graphAjdList.find(solved);
            if (it != graphAjdList.end()) {
                for (auto child : it->second) {
                    // 子节点入度减 1，如果减到 0，则可以安全处理并入队
                    if (--indegree[child] == 0) {
                        can_be_solved.emplace(child);
                    }
                }
            }
        }

        // 如果最终未处理的节点不为 0，说明存在因为环路导致无法归零入度的顶点，即存在环
        return !(remain == 0);
    }
};

/**
 * @brief 主函数
 */
int main() {
    // 实例化一个含有有向环的图：0->1, 1->2, 2->0 形成环
    Graph g(7, std::vector<Edge>{{0, 1}, {1, 2}, {2, 0}, {2, 5}, {3, 5}});
    
    // 用 BFS 方法检查是否有环 (预期输出：1)
    std::cout << CycleCheck::isCyclicBFS(g) << '\n';

    // 用 DFS 方法检查是否有环 (预期输出：1)
    std::cout << CycleCheck::isCyclicDFS(g) << '\n';
    return 0;
}
