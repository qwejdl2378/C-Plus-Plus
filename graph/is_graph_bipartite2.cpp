/**
 * @file
 * @brief Check whether a given graph is bipartite or not (基于 BFS 染色法检测二分图的另一种实现方式)
 *
 * @details
 * 二分图的定义：一个图中的所有顶点可以被分为两个不相交的集合，使得同一个集合内的任意两个节点之间没有边连接。
 * 
 * ### 算法实现细节（此文件较之第一版的优化）
 * 1. 使用 `1` 和 `-1` 表示两类不同的染色，`0` 表示节点尚未被访问或染色。
 * 2. 染色逻辑：`(*visited)[v] = ((*visited)[u] == 1) ? -1 : 1;`（即当前为 1 则子节点为 -1；当前为 -1 则子节点为 1）。
 * 3. **已实现早期终止（Early Return）**：在第 63 行中，一旦在邻接节点中检测到颜色冲突，程序会**直接返回 false** 终止计算。
 *    相较于 `is_graph_bipartite.cpp` 的按位与运算，此实现更为高效。
 *
 * 时间复杂度: O(V + E)
 * 空间复杂度: O(V)
 *
 * @author [tushar2407](https://github.com/tushar2407)
 */

#include <cassert>   /// 用于 assert 断言
#include <cstdint>   /// 用于 int64_t 等数据类型
#include <iostream>  /// 用于输入输出
#include <queue>     /// 用于 std::queue 队列
#include <vector>    /// 用于 std::vector 容器

/**
 * @namespace graph
 * @brief 图算法命名空间
 */
namespace graph {
/**
 * @brief 针对单个连通分量进行 BFS 染色和冲突检测
 * @param graph 邻接表表示的 2D vector 图
 * @param index 起始探索节点的索引
 * @param visited 记录染色状态的向量指针（0 为未访问，1 和 -1 为两种颜色）
 * @returns `true` 该连通分量是二分图；`false` 否则
 */
bool checkBipartite(const std::vector<std::vector<int64_t>> &graph,
                    int64_t index, std::vector<int64_t> *visited) {
    std::queue<int64_t> q;
    
    q.push(index);
    (*visited)[index] = 1; // 默认初始点染色为 1

    while (q.size()) {
        int64_t u = q.front();
        q.pop();
        
        for (uint64_t i = 0; i < graph[u].size(); i++) {
            int64_t v = graph[u][i];
            
            if (!(*visited)[v]) {
                // 如果邻居节点未染色，染上与父节点相反的颜色，并入队
                (*visited)[v] = ((*visited)[u] == 1) ? -1 : 1;
                q.push(v);
            } 
            // 如果邻居节点已染色且颜色与当前父节点相同，则发生冲突，不是二分图
            else if ((*visited)[v] == (*visited)[u]) {
                return false; // 早期终止，直接返回失败
            }
        }
    }
    return true;
}

/**
 * @brief 判断整个图（可能非连通）是否是二分图
 * @param graph 邻接表表示的 2D vector 图
 * @returns `true` 是二分图；`false` 否则
 */
bool isBipartite(const std::vector<std::vector<int64_t>> &graph) {
    std::vector<int64_t> visited(graph.size(), 0); // 初始化为 0

    // 循环遍历所有顶点，支持非连通图的判定
    for (uint64_t i = 0; i < graph.size(); i++) {
        if (!visited[i]) {
            if (!checkBipartite(graph, i, &visited)) {
                return false; // 任意连通分支冲突，直接判定失败
            }
        }
    }
    return true;
}
}  // namespace graph

/**
 * @brief 单元自测用例
 */
static void test() {
    // 准备一个二分图数据：0-1, 1-2, 2-3, 3-0
    std::vector<std::vector<int64_t>> graph = {{1, 3}, {0, 2}, {1, 3}, {0, 2}};
    assert(graph::isBipartite(graph) == true);

    // 准备一个含有奇数环的非二分图数据
    std::vector<std::vector<int64_t>> graph_not_bipartite = {
        {1, 2, 3}, {0, 2}, {0, 1, 3}, {0, 2}};
    assert(graph::isBipartite(graph_not_bipartite) == false);

    std::cout << "All tests have successfully passed!\n";
}

/**
 * @brief 主函数
 */
int main() {
    test();  // 运行自测
    return 0;
}
