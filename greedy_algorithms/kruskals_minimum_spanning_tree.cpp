/**
 * @file
 * @brief Find the minimum weight edge for each vertex (查找每个顶点的最小权重出边——本文件被原作者命名为 Kruskal 最小生成树，但实际仅实现了出边查找)
 *
 * @details
 * @warning 
 * 【设计缺陷与算法审计提示】：
 * 虽然该文件被命名为 `kruskals_minimum_spanning_tree.cpp`，但是其内部的核心函数 `findMinimumEdge` **并没有实现真正的 Kruskal 最小生成树算法**。
 * 
 * - **真实的 Kruskal 算法**：需要将图中的**所有边**按权重从小到大进行全局排序，然后利用**并查集（DSU）**辅助，
 *   依次贪心地选择权重最小且不与已选择的边构成环路（树中不能有环）的 $V-1$ 条边，从而连通整个图。
 * - **本文件的实际行为**：它仅仅实现了一个非常简单的局部贪心策略——遍历每个顶点 $i$，
 *   然后在所有与 $i$ 相连的边中，找出一条权重最小的边打印输出。这无法保证所选出的边集能够构成一棵无环的最小生成树（可能会有环，也可能不连通）。
 *
 * 为了保持与仓库既有测试用例的兼容性，本代码保留了原有的接口和逻辑，但在内部修复了严重的类型转换 Bug，并添加了详细的教学中文注释。
 *
 * @note
 * 【C++ 模板类型截断与精度丢失 Bug 审计与修复】：
 * 原代码在模板函数中定义临时变量时写为：
 *   `int min = infinity;`
 * 1. **类型截断与比较失效 Bug**：`infinity` 的类型是模板参数 `T`（如 `float`, `double`, `uint32_t` 等）。
 *    如果 `T` 是 `float`（如测试用例 2，包含 2.5f, 3.2f 等浮点数），
 *    声明 `int min` 会导致在赋值时发生**隐式类型截断（Implicit Truncation）**。
 *    后续 `min = graph[i][j];` 会将浮点型权重强转为 `int`，导致精度丢失。
 *    例如，若性价比或边权包含 `2.5f` 和 `2.1f`，`2.5f` 写入 `int min` 后被截断为 `2`。
 *    随后比较 `2.1f < 2` 会判定为 `false`（实际上 2.1 应当小于 2.5），从而选出错误的“最小边”。
 *    **修复**：已将 `int min` 修正为与模板一致的 `T min`。
 *
 * @author [coleman2246](https://github.com/coleman2246)
 */

#include <array>     /// 用于 std::array
#include <iostream>  /// 用于输入输出
#include <limits>    /// 用于获取各类型的最大值
#include <cstdint>   /// 用于 std::uint32_t

/**
 * @namespace greedy_algorithms
 * @brief 贪心算法命名空间
 */
namespace greedy_algorithms {

/**
 * @brief 查找并打印每个顶点的最小权重出边
 * @tparam T 边权重的数据类型（int, float, double 等）
 * @tparam N 二维矩阵的列数
 * @tparam M 二维矩阵的行数
 * @param infinity 代表无边相连的无穷大值
 * @param graph 邻接矩阵表示的图结构
 */
template <typename T, std::size_t N, std::size_t M>
void findMinimumEdge(const T &infinity,
                     const std::array<std::array<T, N>, M> &graph) {
    // 安全校验：邻接矩阵必须是方阵 (M x N 且 M == N)
    if (N != M) {
        std::cout << "\nWrong input passed. Provided array has dimensions " << N
                  << "x" << M << ". Please provide a square matrix.\n";
        return;
    }

    // 遍历每个顶点 i
    for (size_t i = 0; i < graph.size(); i++) {
        T min = infinity;   // 核心修复：将 int min 更改为 T min，防止浮点数截断和精度丢失
        size_t minIndex = 0; // 记录最小权边对应的邻接顶点索引
        
        // 扫描与顶点 i 相连的所有邻接顶点 j
        for (size_t j = 0; j < graph.size(); j++) {
            // 排除自环（i != j）、排除无边连接（值为 0 或 infinity）
            if (i != j && graph[i][j] != T(0) && graph[i][j] < min) {
                min = graph[i][j];
                minIndex = j;
            }
        }
        
        // 打印该顶点对应的局部最小出边
        std::cout << i << "  -  " << minIndex << "\t" << graph[i][minIndex] << "\n";
    }
}
}  // namespace greedy_algorithms

/**
 * @brief 单元自测用例
 */
static void test() {
    constexpr int INFINITY_INT = std::numeric_limits<int>::max();
    constexpr float INFINITY_FLOAT = std::numeric_limits<float>::max();
    constexpr double INFINITY_DOUBLE = std::numeric_limits<double>::max();
    constexpr uint32_t INFINITY_UINT32 = UINT32_MAX;
  
    // 测试 1：整型权重图
    std::cout << "\nTest Case 1 (int):\n";
    std::array<std::array<int, 6>, 6> graph1{{
        {0, 4, 1, 4, INFINITY_INT, INFINITY_INT},
        {4, 0, 3, 8, 3, INFINITY_INT},
        {1, 3, 0, INFINITY_INT, 1, INFINITY_INT},
        {4, 8, INFINITY_INT, 0, 5, 7},
        {INFINITY_INT, 3, 1, 5, 0, INFINITY_INT},
        {INFINITY_INT, INFINITY_INT, INFINITY_INT, 7, INFINITY_INT, 0}
    }};
    greedy_algorithms::findMinimumEdge(INFINITY_INT, graph1);

    // 测试 2：浮点型权重图（测试浮点数比较和修复效果）
    std::cout << "\nTest Case 2 (float):\n";
    std::array<std::array<float, 3>, 3> graph2{{
        {0.0f, 2.5f, INFINITY_FLOAT},
        {2.5f, 0.0f, 3.2f},
        {INFINITY_FLOAT, 3.2f, 0.0f}
    }};
    greedy_algorithms::findMinimumEdge(INFINITY_FLOAT, graph2);

    // 测试 3：双精度浮点数图
    std::cout << "\nTest Case 3 (double):\n";
    std::array<std::array<double, 5>, 5> graph3{{
        {0.0, 10.5, INFINITY_DOUBLE, 6.7, 3.3},
        {10.5, 0.0, 8.1, 15.4, INFINITY_DOUBLE},
        {INFINITY_DOUBLE, 8.1, 0.0, INFINITY_DOUBLE, 7.8},
        {6.7, 15.4, INFINITY_DOUBLE, 0.0, 9.9},
        {3.3, INFINITY_DOUBLE, 7.8, 9.9, 0.0}
    }};
    greedy_algorithms::findMinimumEdge(INFINITY_DOUBLE, graph3);

    // 测试 4：包含负权重的图
    std::cout << "\nTest Case 4 (negative weights):\n";
    std::array<std::array<int, 3>, 3> graph_neg{{
        {0, -2, 4},
        {-2, 0, 3},
        {4, 3, 0}
    }};
    greedy_algorithms::findMinimumEdge(INFINITY_INT, graph_neg);

    // 测试 5：无向图的自环测试
    std::cout << "\nTest Case 5 (self-loops):\n";
    std::array<std::array<int, 3>, 3> graph_self_loop{{
        {2, 1, INFINITY_INT},
        {INFINITY_INT, 0, 4},
        {INFINITY_INT, 4, 0}
    }};
    greedy_algorithms::findMinimumEdge(INFINITY_INT, graph_self_loop);

    // 测试 6：无边图
    std::cout << "\nTest Case 6 (no edges):\n";
    std::array<std::array<int, 4>, 4> no_edges{{
        {0, INFINITY_INT, INFINITY_INT, INFINITY_INT},
        {INFINITY_INT, 0, INFINITY_INT, INFINITY_INT},
        {INFINITY_INT, INFINITY_INT, 0, INFINITY_INT},
        {INFINITY_INT, INFINITY_INT, INFINITY_INT, 0}
    }};
    greedy_algorithms::findMinimumEdge(INFINITY_INT, no_edges);

    // 测试 7：非连通图
    std::cout << "\nTest Case 7 (partial graph):\n";
    std::array<std::array<int, 4>, 4> partial_graph{{
        {0, 2, INFINITY_INT, 6},
        {2, 0, 3, INFINITY_INT},
        {INFINITY_INT, 3, 0, 4},
        {6, INFINITY_INT, 4, 0}
    }};
    greedy_algorithms::findMinimumEdge(INFINITY_INT, partial_graph);

    // 测试 8：有向带权图
    std::cout << "\nTest Case 8 (directed graph):\n";
    std::array<std::array<int, 4>, 4> directed_graph{{
        {0, 3, 7, INFINITY_INT},
        {INFINITY_INT, 0, 2, 5},
        {INFINITY_INT, INFINITY_INT, 0, 1},
        {INFINITY_INT, INFINITY_INT, INFINITY_INT, 0}
    }};
    greedy_algorithms::findMinimumEdge(INFINITY_INT, directed_graph);

    // 测试 9：非法矩阵维度（非方阵）
    std::cout << "\nTest Case 9 (wrong size - expect error print):\n";
    std::array<std::array<int, 4>, 3> graph9{{
        {0, 5, 5, 5},
        {5, 0, 5, 5},
        {5, 5, 5, 5}
    }};
    greedy_algorithms::findMinimumEdge(INFINITY_INT, graph9);

    // 测试 10：全部等权边的图
    std::cout << "\nTest Case 10 (equal weights):\n";
    std::array<std::array<int, 5>, 5> graph10{{
        {0, 5, 5, 5, 5},
        {5, 0, 5, 5, 5},
        {5, 5, 0, 5, 5},
        {5, 5, 5, 0, 5},
        {5, 5, 5, 5, 0}
    }};
    greedy_algorithms::findMinimumEdge(INFINITY_INT, graph10);

    // 测试 11：无符号 32 位整型权重图
    std::cout << "\nTest Case 11 (uint32_t):\n";
    std::array<std::array<uint32_t, 4>, 4> graph_uint32{{
        {0, 5, INFINITY_UINT32, 9},
        {5, 0, 2, INFINITY_UINT32},
        {INFINITY_UINT32, 2, 0, 6},
        {9, INFINITY_UINT32, 6, 0}
    }};
    greedy_algorithms::findMinimumEdge(INFINITY_UINT32, graph_uint32);

    std::cout << "\nAll tests have successfully passed!\n";
}

/**
 * @brief 主函数
 */
int main() {
    test(); // 运行测试
    return 0;
}
