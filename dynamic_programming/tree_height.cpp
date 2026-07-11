/**
 * @file
 * @brief Find the height of a rooted tree using tree dynamic programming (基于树形动态规划求解树的高度)
 * @details
 * 树形动态规划（Tree DP）：
 * 给定一个以节点 1 为根节点的树，计算该树的高度（以节点数度量，即单节点高度为 1）。
 *
 * ### 动态规划状态转移（树形 DP）
 * 设 `dp[u]` 表示以节点 `u` 为根的子树的高度。
 * - 基础状态：若 `u` 是叶子节点，则 `dp[u] = 1`。
 * - 递推关系：对于节点 `u` 的所有子节点 `v`：
 *   `dp[u] = max(1, max_{v 是 u 的子节点} (dp[v] + 1))`
 * 通过自底向上的深度优先搜索（DFS）后序遍历，先计算所有子树高度，再汇总计算父节点的高度。
 *
 * @note
 * 【硬编码上限与越界警示】：
 * 1. 在第 26 行中，硬编码了树的最大节点上限 `const int MAX = 1e5;`。
 * 2. 全局邻接表定义为 `std::vector<int> adj[MAX];`。
 *    若用户输入的节点数 `number_of_nodes` 超过 100,000，或输入的边节点 ID 大于等于 `MAX`，
 *    在录入边时执行 `adj[u].push_back(v)` 会发生**严重的数组越界访问并造成段错误崩溃**。
 * 3. 建议优化：在 `main` 内部根据输入动态创建 `std::vector<std::vector<int>> adj(number_of_nodes + 1);` 以适应任意大小的树。
 */

#include <iostream>
#include <vector>

// 全局声明
const int MAX = 1e5; // 最大支持 10 万个节点

std::vector<int> adj[MAX]; // 邻接表，硬编码了 MAX 大小
std::vector<bool> visited;  // 记录节点访问状态
std::vector<int> dp;       // dp[u] 存储子树 u 的高度

/**
 * @brief 深度优先搜索（DFS）后序遍历计算子树高度
 * @param u 当前访问的节点
 */
void depth_first_search(int u) {
    visited[u] = true;
    int child_height = 1; // 默认高度（如叶子节点）为 1
    
    for (int v : adj[u]) {
        if (!visited[v]) {
            depth_first_search(v); // 递归先计算子节点 v 的高度

            // 状态转移：当前子树高度为所有子树中最大高度 + 1
            child_height = std::max(child_height, dp[v] + 1);
        }
    }
    // 将最终计算出的最大高度赋给当前节点
    dp[u] = child_height;
}

/**
 * @brief 主函数
 */
int main() {
    int number_of_nodes;
    std::cout << "Enter number of nodes of the tree : " << std::endl;
    std::cin >> number_of_nodes;

    int u, v;
    std::cout << "Enter edges of the tree : " << std::endl;
    for (int i = 0; i < number_of_nodes - 1; i++) {
        std::cin >> u >> v;
        // 无向树边双向建图
        // 警告：若 u 或 v 大于等于 MAX，此处将越界崩溃
        adj[u].push_back(v);
        adj[v].push_back(u);
    }
    
    // 初始化节点状态数组大小
    visited.assign(number_of_nodes + 1, false);
    dp.assign(number_of_nodes + 1, 0);
    
    // 从根节点 1 开始执行 DFS
    depth_first_search(1);
    
    std::cout << "Height of the Tree : " << dp[1] << std::endl;
    return 0;
}
