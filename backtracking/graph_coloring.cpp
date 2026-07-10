/**
 * @file
 * @brief prints the assigned colors using [Graph Coloring](https://en.wikipedia.org/wiki/Graph_coloring) algorithm (图着色回溯算法实现)
 *
 * @details
 * 图的着色问题：给定一个图以及 m 种颜色，尝试为图中的每个顶点分配一种颜色，
 * 使得任何两个相邻的顶点都具有不同的颜色。这被称为顶点着色问题。
 * 本程序采用回溯法求解所有可行的顶点着色方案。
 *
 * @author [Anup Kumar Panwar](https://github.com/AnupKumarPanwar)
 * @author [David Leal](https://github.com/Panquesito7)
 */

#include <array>     /// 用于 std::array
#include <iostream>  /// 用于输入输出

/**
 * @brief 回溯算法命名空间
 * @namespace backtracking
 */
namespace backtracking {
/**
 * @brief 图着色算法命名空间
 * @namespace graph_coloring
 */
namespace graph_coloring {
/**
 * @brief 打印一种着色方案的辅助函数
 * @tparam V 图的顶点个数
 * @param color 存放每个顶点分配颜色的数组
 */
template <size_t V>
void printSolution(const std::array<int, V>& color) {
    std::cout << "Following are the assigned colors\n";
    for (auto& col : color) {
        std::cout << col << " ";
    }
    std::cout << "\n";
}

/**
 * @brief 检查当前为顶点 v 分配颜色 c 是否安全（即不与任何相邻已着色顶点的颜色冲突）
 * 
 * @tparam V 图的顶点个数
 * @param v 当前需要着色的顶点索引
 * @param graph 图的邻接矩阵表示
 * @param color 当前记录各顶点颜色的数组
 * @param c 尝试分配给顶点 v 的目标颜色值
 * @returns `true` 如果可以安全着色；`false` 如果存在相邻顶点拥有相同颜色
 */
template <size_t V>
bool isSafe(int v, const std::array<std::array<int, V>, V>& graph,
            const std::array<int, V>& color, int c) {
    // 遍历图中的所有顶点，检查与顶点 v 相邻的节点 i 是否已经使用了颜色 c
    for (int i = 0; i < V; i++) {
        if (graph[v][i] && c == color[i]) {
            return false; // 相邻且颜色相同，冲突
        }
    }
    return true; // 没有冲突
}

/**
 * @brief 递归求解 m 着色问题的回溯函数
 * @tparam V 图的顶点个数
 * @param graph 图的邻接矩阵
 * @param m 拥有的颜色总数
 * @param color 保存当前染色方案的数组
 * @param v 当前正进行染色的顶点索引
 */
template <size_t V>
void graphColoring(const std::array<std::array<int, V>, V>& graph, int m,
                   std::array<int, V> color, int v) {
    // 递归出口：如果所有顶点都已经成功分配了颜色，则输出该着色方案并返回
    if (v == V) {
        printSolution<V>(color);
        return;
    }

    // 尝试将颜色从 1 到 m 依次分配给当前顶点 v
    for (int c = 1; c <= m; c++) {
        // 检测顶点 v 染颜色 c 是否安全
        if (isSafe<V>(v, graph, color, c)) {
            // 做出选择：将颜色 c 分配给顶点 v
            color[v] = c;

            // 递归步骤：为下一个顶点 v + 1 分配颜色
            graphColoring<V>(graph, m, color, v + 1);

            // 撤销选择（回溯）：将顶点 v 的颜色重置，以便循环尝试其他颜色
            color[v] = 0;
        }
    }
}
}  // namespace graph_coloring
}  // namespace backtracking

/**
 * @brief 主函数，构建测试图并求解 3 着色问题
 * @returns 0
 */
int main() {
    // 创建如下的测试图，并检验是否能用 3 种颜色着色：
    // (3)---(2)
    //  |   / |
    //  |  /  |
    //  | /   |
    // (0)---(1)

    const int V = 4;  // 顶点个数为 4
    std::array<std::array<int, V>, V> graph = {
        std::array<int, V>({0, 1, 1, 1}), std::array<int, V>({1, 0, 1, 0}),
        std::array<int, V>({1, 1, 0, 1}), std::array<int, V>({1, 0, 1, 0})};

    int m = 3;  // 可用颜色总数为 3
    std::array<int, V> color{}; // 初始化颜色数组为全 0

    // 从第 0 号顶点开始启动着色回溯
    backtracking::graph_coloring::graphColoring<V>(graph, m, color, 0);
    return 0;
}
