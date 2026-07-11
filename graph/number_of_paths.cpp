/**
 * @file
 * @brief Algorithm to count simple paths between two nodes in a directed graph using DFS and Backtracking (基于 DFS 与回溯的有向图简单路径计数算法)
 *
 * @details
 * 该算法用于求解在一个有向图（用邻接矩阵表示）中，从源节点 `u` 到目标节点 `v` 的所有**简单路径（Simple Paths）**的数量。
 * 简单路径是指在路径中不重复经过任何顶点的路径。
 *
 * ### 算法思想
 * 1. 从起点 `u` 开始执行 DFS。
 * 2. 将当前节点 `u` 标记为已访问（染成 true），防止路径成环或自我重叠。
 * 3. 遍历 `u` 的所有出边指向的邻接节点 `i`：
 *    - 如果存在从 `u` 到 `i` 的边且 `i` 在当前路径中未被访问，则递归调用 `count_paths_dfs` 计算从 `i` 到 `v` 的路径数并进行累加。
 * 4. 重点：回溯（Backtracking）——在当前层 DFS 即将退出时，将 `visited[u]` 重新恢复成 false，
 *    以允许其他分叉的路径在随后的探索中能够再次合法地经过节点 `u`。
 *
 * 时间复杂度: 最坏情况下（如完全图）为 $O(V!)$。
 * 空间复杂度: $O(V)$（递归调用栈和 visited 表深度）。
 * 
 * @author [Aditya Borate](https://github.com/adi776borate)
 */

#include <vector>   /// 用于 std::vector
#include <iostream> /// 用于输入输出
#include <cassert>  /// 用于 assert 断言
#include <cstdint>  /// 用于固定宽度整数类型

/**
 * @namespace graph
 * @brief 图算法命名空间
 */
namespace graph {

    /**
     * @brief 递归执行 DFS 和回溯计算路径数量的辅助函数
     * @param A 邻接矩阵 (A[u][i] == 1 代表 u 到 i 有一条单向边；0 代表无边)
     * @param u 当前节点 ID
     * @param v 目标终点 ID
     * @param n 顶点总数
     * @param visited 记录当前 DFS 路径上访问状态的向量引用
     * @returns 从 u 到 v 的简单路径数量
     */
    std::uint32_t count_paths_dfs(const std::vector<std::vector<std::uint32_t>>& A, 
                              std::uint32_t u, 
                              std::uint32_t v, 
                              std::uint32_t n, 
                              std::vector<bool>& visited) {
        // 递归基：如果已经到达终点节点，找到 1 条合法路径
        if (u == v) {
            return 1;
        }

        visited[u] = true;  // 标记当前节点已访问（进入路径）
        std::uint32_t path_count = 0;

        for (std::uint32_t i = 0; i < n; i++) {
            // 如果有出边且邻接节点在当前路径中未被使用过
            if (A[u][i] == 1 && !visited[i]) {
                path_count += count_paths_dfs(A, i, v, n, visited); // 递归累加路径数
            }
        }

        visited[u] = false;  // 回溯：将当前节点移出路径，恢复未访问状态
        return path_count;
    }


    /**
     * @brief 计算有向图中节点 u 到节点 v 的所有简单路径总数
     * @param A 邻接矩阵
     * @param u 起始节点 ID
     * @param v 目标节点 ID
     * @param n 顶点数
     * @returns 简单路径数量
     */
    std::uint32_t count_paths(const std::vector<std::vector<std::uint32_t>>& A, 
                          std::uint32_t u, 
                          std::uint32_t v, 
                          std::uint32_t n) {
        // 节点下标越界或者图为空时返回 0
        if (u >= n || v >= n || A.empty() || A[0].empty()) {
            return 0;
        }

        std::vector<bool> visited(n, false);
        return count_paths_dfs(A, u, v, n, visited); // 触发 DFS 搜索
    }

} // namespace graph

/**
 * @brief 单元自测用例
 */
static void test() {
    // 测试 1：包含多条路径的简单有向图
    std::vector<std::vector<std::uint32_t>> graph1 = {
        {0, 1, 0, 1, 0}, 
        {0, 0, 1, 0, 1}, 
        {0, 0, 0, 0, 1}, 
        {0, 0, 1, 0, 0}, 
        {0, 0, 0, 0, 0}  
    };
    std::uint32_t n1 = 5, u1 = 0, v1 = 4;
    assert(graph::count_paths(graph1, u1, v1, n1) == 3); // 预期有 3 条路径：0->1->4, 0->1->2->4, 0->3->2->4

    // 测试 2：不连通图，无可行路径
    std::vector<std::vector<std::uint32_t>> graph2 = {
        {0, 1, 0, 0, 0}, 
        {0, 0, 0, 0, 0}, 
        {0, 0, 0, 0, 1}, 
        {0, 0, 1, 0, 0}, 
        {0, 0, 0, 0, 0}  
    };
    std::uint32_t n2 = 5, u2 = 0, v2 = 4;
    assert(graph::count_paths(graph2, u2, v2, n2) == 0);

    // 测试 3：包含环路的有向图
    std::vector<std::vector<std::uint32_t>> graph3 = {
        {0, 1, 0, 0, 0}, 
        {0, 0, 1, 1, 0}, 
        {1, 0, 0, 0, 1}, 
        {0, 0, 1, 0, 1}, 
        {0, 0, 0, 0, 0}  
    };
    std::uint32_t n3 = 5, u3 = 0, v3 = 4;
    assert(graph::count_paths(graph3, u3, v3, n3) == 3);

    // 测试 4：单节点图（自环）
    std::vector<std::vector<std::uint32_t>> graph4 = {
        {0}
    };
    std::uint32_t n4 = 1, u4 = 0, v4 = 0;
    assert(graph::count_paths(graph4, u4, v4, n4) == 1); // 起点终点重合，直接返回 1 路径

    // 测试 5：空图
    std::vector<std::vector<std::uint32_t>> graph5 = {{}};
    int n5 = 0, u5 = 0, v5 = 0;
    assert(graph::count_paths(graph5, u5, v5, n5) == 0);

    // 测试 6：无效边界顶点
    std::vector<std::vector<std::uint32_t>> graph6 = {
        {0, 1, 0}, 
        {0, 0, 1}, 
        {0, 0, 0}
    };
    int n6 = 3, u6 = 0, v6 = 5;
    assert(graph::count_paths(graph6, u6, v6, n6) == 0);

    std::cout << "All tests have successfully passed!\n";
}

/**
 * @brief 主函数
 */
int main() {
    test(); // 运行测试
    return 0;
}
