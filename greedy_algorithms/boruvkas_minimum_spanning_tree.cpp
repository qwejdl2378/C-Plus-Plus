/**
 * @file
 * @brief Implementation of [Borůvka's Algorithm](https://en.wikipedia.org/wiki/Bor%C5%AFvka%27s_algorithm) to find the Minimum Spanning Tree (Borůvka 最小生成树算法实现)
 *
 * @details
 * Borůvka 算法（也称 Sollin 算法）是历史上第一个被发明的最小生成树（MST）算法（由 Otakar Borůvka 于 1926 年提出）。
 *
 * ### 算法思想 (贪心策略)
 * 1. 初始化：将图中每个顶点单独视为一个连通分量（组/树），初始时有 V 个组。
 * 2. 局部贪心查找：对于当前存在的每个连通分量，在所有连接它与其他分量的边中，寻找一条权值最小的边（即最小外部出边）。
 * 3. 分量合并：将这些选出的最小边加入 MST，并合并相应的连通分量（通过并查集并入同一个组）。
 * 4. 迭代收敛：重复步骤 2-3，每次迭代后连通分量个数至少减半。当连通分量个数缩减为 1 时，最小生成树构建完毕。
 *
 * ### 性能分析
 * - 时间复杂度: $O(E \log V)$，因为每次迭代分量数减半，至多进行 $O(\log V)$ 轮，每轮需要 $O(E)$ 时间遍历所有边。
 * - 空间复杂度: $O(V)$ (存储并查集树节点信息)。
 *
 * @note
 * 【C++ 并查集重大性能与功能失效 Bug 审计与修复】：
 * 原代码中，并查集的根节点查找辅助函数 `findParent` 定义如下：
 *   `int findParent(std::vector<std::pair<int, int>> parent, const int v)`
 * 1. **丢失路径压缩 Bug**：参数 `parent` 是**按值传递（Pass by Value）**的。
 *    因此，函数内部执行的路径压缩优化 `parent[v].first = findParent(parent, parent[v].first);` 
 *    只会修改临时拷贝的副本，**完全无法作用到调用方的 DSU 数据上**。这就导致路径压缩彻底失效。
 * 2. **严重拷贝性能瓶颈**：由于按值传递，每次递归调用 `findParent` 都会把整个 `parent` 数组（大小为 V）完整复制一份，
 *    如果树退化为链状，单次查询便会触发 $O(V)$ 次的完整数组拷贝，整体复杂度退化为可怕的 $O(V^2)$ 并导致潜在的**栈溢出崩溃**。
 * 
 * ### 修复方案
 * - 将 `findParent` 参数修改为**引用传递**：`std::vector<std::pair<int, int>>& parent`。
 * - 确保路径压缩能直接写入外部的并查集数组，使单次查询时间复杂度优化至近乎常数级 $O(\alpha(V))$。
 *
 * @author [Jason Nardoni](https://github.com/JNardoni)
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
 * @namespace boruvkas_minimum_spanning_tree
 * @brief Borůvka 算法的相关实现命名空间
 */
namespace boruvkas_minimum_spanning_tree {

/**
 * @brief 并查集查找根节点函数（带路径压缩）
 * @param parent 并查集节点信息的引用（核心修复：改用引用传递 &）
 * @param v 待查找的顶点
 * @returns 顶点的根代表节点
 */
int findParent(std::vector<std::pair<int, int>>& parent, const int v) {
    if (parent[v].first != v) {
        parent[v].first = findParent(parent, parent[v].first); // 路径压缩递归写入
    }
    return parent[v].first;
}

/**
 * @brief Borůvka 算法主逻辑
 * @param adj 邻接矩阵表示的图（无边用 INT_MAX 或 0 表示）
 * @returns MST 的邻接矩阵表示
 */
std::vector<std::vector<int>> boruvkas(std::vector<std::vector<int>> adj) {
    size_t size = adj.size();
    size_t total_groups = size;

    if (size <= 1) {
        return adj;
    }

    // 初始化最小生成树邻接矩阵，全部设为无边状态
    std::vector<std::vector<int>> MST(size, std::vector<int>(size, INT_MAX));
    for (size_t i = 0; i < size; i++) {
        MST[i][i] = 0;
    }

    // parent[i].first 表示 i 结点的父节点，parent[i].second 记录树的深高度(Rank)
    std::vector<std::pair<int, int>> parent(size, std::make_pair(0, 0));
    for (size_t i = 0; i < size; i++) {
        parent[i].first = i; // 初始化每个结点的父节点为自身
    }

    // 循环，直到所有节点连通为一个组
    while (total_groups > 1) {
        // smallest_edge 记录当前每个连通分量的最小出边，初始化为 {-1, -1} 标识不存在
        std::vector<std::pair<int, int>> smallest_edge(size, std::make_pair(-1, -1));

        // 1. 遍历所有边，寻找各个分量的最小外部出边
        for (size_t i = 0; i < size; i++) {
            for (size_t j = i + 1; j < size; j++) {
                if (adj[i][j] == INT_MAX || adj[i][j] == 0) {
                    continue; // 两个顶点间没有边
                }

                // 核心修复：按引用调用并查集查找，应用路径压缩
                int parentA = findParent(parent, i);
                int parentB = findParent(parent, j);

                // 如果两端点不在同一个分量中，尝试更新各自所属分量的最小出边
                if (parentA != parentB) {
                    // 更新分量 A 的最小边
                    int start = smallest_edge[parentA].first;
                    int end = smallest_edge[parentA].second;

                    if (start == -1 || adj[i][j] < adj[start][end]) {
                        smallest_edge[parentA].first = i;
                        smallest_edge[parentA].second = j;
                    }

                    // 更新分量 B 的最小边
                    start = smallest_edge[parentB].first;
                    end = smallest_edge[parentB].second;

                    if (start == -1 || adj[j][i] < adj[start][end]) {
                        smallest_edge[parentB].first = j;
                        smallest_edge[parentB].second = i;
                    }
                }
            }
        }

        // 2. 遍历每个分量，合并它们选择的最小外部出边
        for (size_t i = 0; i < size; i++) {
            if (smallest_edge[i].first != -1) {
                int start = smallest_edge[i].first;
                int end = smallest_edge[i].second;

                int parentA = i;
                int parentB = findParent(parent, end);

                // 如果两个分量还没被前面的操作合并，则现在进行合并
                if (parentA != parentB) {
                    // 按秩合并优化，平衡树的深度
                    if (parent[parentA].second < parent[parentB].second) {
                        parent[parentB].first = parentA;
                        parent[parentB].second++;
                    } else {
                        parent[parentA].first = parentB;
                        parent[parentA].second++;
                    }

                    // 将该最小边加入 MST（双向边）
                    MST[start][end] = adj[start][end];
                    MST[end][start] = adj[end][start];
                    total_groups--; // 减少一个连通分量
                }
            }
        }
    }
    return MST;
}

/**
 * @brief 累加生成树的所有边权重
 */
int test_findGraphSum(std::vector<std::vector<int>> adj) {
    size_t size = adj.size();
    int sum = 0;
    for (size_t i = 0; i < size; i++) {
        for (size_t j = i + 1; j < size; j++) {
            if (adj[i][j] < INT_MAX) {
                sum += adj[i][j];
            }
        }
    }
    return sum;
}
}  // namespace boruvkas_minimum_spanning_tree
}  // namespace greedy_algorithms

/**
 * @brief 单元自测用例
 */
static void tests() {
    std::cout << "Starting tests...\n";
    
    // 测试 1
    std::vector<std::vector<int>> graph1 = {
        {0, 5, INT_MAX, 3, INT_MAX}, 
        {5, 0, 2, INT_MAX, 5},
        {INT_MAX, 2, 0, INT_MAX, 3}, 
        {3, INT_MAX, INT_MAX, 0, INT_MAX},
        {INT_MAX, 5, 3, INT_MAX, 0},
    };
    std::vector<std::vector<int>> MST1 =
        greedy_algorithms::boruvkas_minimum_spanning_tree::boruvkas(graph1);
    assert(greedy_algorithms::boruvkas_minimum_spanning_tree::test_findGraphSum(MST1) == 13);
    std::cout << "1st test passed!" << std::endl;

    // 测试 2
    std::vector<std::vector<int>> graph2 = {
        {0, 2, 0, 6, 0},
        {2, 0, 3, 8, 5},
        {0, 3, 0, 0, 7},
        {6, 8, 0, 0, 9},
        {0, 5, 7, 9, 0}
    };
    std::vector<std::vector<int>> MST2 = 
        greedy_algorithms::boruvkas_minimum_spanning_tree::boruvkas(graph2);
    assert(greedy_algorithms::boruvkas_minimum_spanning_tree::test_findGraphSum(MST2) == 16);
    std::cout << "2nd test passed!" << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    tests(); // 运行单元自测
    return 0;
}
