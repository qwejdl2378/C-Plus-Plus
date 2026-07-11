/**
 * @file
 * @brief Implementation of [Tarjan's Bridge Finding Algorithm](https://en.wikipedia.org/wiki/Bridge_(graph_theory)) (基于 Tarjan 算法的图桥/割边寻找算法)
 *
 * @details
 * 桥（Bridge，又称割边）：在无向连通图中，如果删除某条边会使图的连通分量数增加（即图变得不连通），则该边称为桥。
 *
 * ### Tarjan 算法原理
 * 算法基于 DFS 树：
 * 1. `in_time[u]`：DFS 遍历到节点 `u` 的时间戳（发现时间）。
 * 2. `out_time[u]`（在标准算法中常记为 `low[u]`）：在以 `u` 为根的 DFS 子树中，通过子节点或一条非父回溯边（Back Edge）所能到达的最小时间戳。
 * 3. 递归松弛：
 *    - 对于 `u` 的相邻非父节点 `v`：
 *      - 若 `v` 未访问：递归 `dfs(v, u)`，并在回溯时，如果满足 `out_time[v] > in_time[u]`（说明从 `v` 无法通过任何回溯边到达 `u` 或更上层的节点），则边 `(u, v)` 是一条桥。
 *      - 不管 `v` 是否被访问，都更新 `out_time[u] = min(out_time[u], out_time[v])`。
 *
 * 时间复杂度: O(V + E)
 * 空间复杂度: O(V + E)
 *
 * @author [Amit Kumar](https://github.com/offamitkumar)
 */

#include <algorithm>  // 用于 std::min
#include <iostream>   // 用于标准输出
#include <vector>     // 用于 std::vector

/**
 * @class Solution
 * @brief 寻找割边算法的解决方案类
 */
class Solution {
    std::vector<std::vector<int>> graph; // 邻接表
    std::vector<int> in_time;            // 记录 DFS 发现时间戳
    std::vector<int> out_time;           // 记录当前子树能追溯到的最小时间戳（即 low 数组）
    int timer = 0;                       // 递增计时器
    std::vector<std::vector<int>> bridge; // 保存找到的桥边集
    std::vector<bool> visited;           // 节点访问记录表

    /**
     * @brief 深度优先搜索（DFS）遍历查找桥
     * @param current_node 当前探索的节点
     * @param parent 当前节点的父节点，防止走回头路
     */
    void dfs(int current_node, int parent) {
        visited.at(current_node) = true;
        in_time[current_node] = out_time[current_node] = timer++;

        for (auto& itr : graph[current_node]) {
            if (itr == parent) {
                continue; // 忽略直接回到父节点的边
            }
            if (!visited[itr]) {
                dfs(itr, current_node); // 递归访问子节点

                // 如果子节点 itr 能回溯到的最早节点也比 current_node 晚，说明这是一条桥
                if (out_time[itr] > in_time[current_node]) {
                    bridge.push_back({itr, current_node});
                }
            }
            // 无论是否访问过，都尝试更新 low 值为可触及的最小值
            out_time[current_node] =
                std::min(out_time[current_node], out_time[itr]);
        }
    }

 public:
    /**
     * @brief 寻找无向图中的所有割边
     * @param n 节点总数
     * @param connections 边集数组，包含各个 {u, v}
     * @returns 包含所有桥（割边）的数组
     */
    std::vector<std::vector<int>> search_bridges(
        int n, const std::vector<std::vector<int>>& connections) {
        timer = 0;
        graph.clear();
        bridge.clear();
        graph.resize(n);
        in_time.assign(n, 0);
        visited.assign(n, false);
        out_time.assign(n, 0);

        // 构建无向图邻接表
        for (auto& itr : connections) {
            graph.at(itr[0]).push_back(itr[1]);
            graph.at(itr[1]).push_back(itr[0]);
        }
        dfs(0, -1); // 从节点 0 开始搜索
        return bridge;
    }
};

/**
 * @brief 主函数
 */
int main() {
    Solution s1;
    int number_of_node = 5;
    std::vector<std::vector<int>> node;
    node.push_back({0, 1});
    node.push_back({1, 3});
    node.push_back({1, 2});
    node.push_back({2, 4});

    /*
     * 连通图形状示例：
     *     0 <--> 1 <---> 2
     *            ^       ^
     *            |       |
     *            v       v
     *            3       4
     *
     * 该无向树状图中没有环，所以所有的 4 条边都是割边（桥）。
     */
    std::vector<std::vector<int>> bridges =
        s1.search_bridges(number_of_node, node);
    
    std::cout << bridges.size() << " bridges found!\n";
    for (auto& itr : bridges) {
        std::cout << itr[0] << " --> " << itr[1] << '\n';
    }
    return 0;
}
