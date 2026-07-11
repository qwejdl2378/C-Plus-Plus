/**
 * @file
 * @brief Implementation of [Hamiltonian Path / Cycle](https://en.wikipedia.org/wiki/Hamiltonian_path) algorithm (基于状态压缩动态规划的哈密顿路径/回路算法实现)
 *
 * @details
 * 哈密顿路径（Hamiltonian Path）：经过图中每个顶点且仅经过一次的路径。
 * 哈密顿回路（Hamiltonian Cycle）：在哈密顿路径的基础上，终点能够回到起点，形成闭合环路。
 *
 * ### 状态压缩动态规划（State Compression DP）
 * 设 `dp[state][i]` 表示在已访问节点集合状态为 `state`（以二进制位表示，第 k 位为 1 代表节点 k 已访问），且以节点 `i` 作为当前路径结尾的路径是否存在。
 * - 基础状态：`dp[1 << i][i] = true`（以节点 `i` 作为起点，只访问了自身）。
 * - 转移方程：遍历所有状态 `state` 及其结尾 `o`（`dp[state][o]` 为真）：
 *   对于未访问过的邻接节点 `z`（即 `(1<<z) & state == 0` 且有边 `routes[o][z] == true`）：
 *   `dp[state + (1 << z)][z] = true`。
 *
 * @note
 * 【重要概念纠偏与算法局限说明】：
 * 1. 尽管函数命名为 `hamilton_cycle`，但**该代码实际求解的是“哈密顿路径（Hamiltonian Path）”**，而不是真正的“哈密顿回路”。
 * 2. 真正的“哈密顿回路”要求在访问完所有节点后，终点必须有一条边可以回到起点。然而，本程序最后仅检查 `dp[height - 1][i]` 是否为真（代表是否存在一条经过所有顶点的路径以 `i` 结尾），却完全没有验证 `i` 是否有边连回起点。
 * 3. 示例测试 1 中的图 `1 -> 2 -> 3 -> 4` 是一条直线路径，没有回路。但由于程序仅验证了哈密顿路径的存在性，因而返回了 `true`。
 *
 * 时间复杂度: O(2^N * N^2)
 * 空间复杂度: O(2^N * N)
 * 
 * @author [vakhokoto](https://github.com/vakhokoto)
 * @author [Krishna Vedala](https://github.com/kvedala)
 */

#include <cassert>
#include <iostream>
#include <vector>

/**
 * @brief 判断图是否存在哈密顿路径（程序虽命名为 cycle，实际仅校验了 path）
 * @param routes 邻接矩阵表示的图，routes[i][j] 为 true 代表 i 到 j 有边
 * @returns `true` 存在哈密顿路径；`false` 不存在
 */
bool hamilton_cycle(const std::vector<std::vector<bool>> &routes) {
    const size_t n = routes.size();
    // 状态压缩数组的高度，共 2^n 种状态
    const size_t height = 1 << n;
    std::vector<std::vector<bool>> dp(height, std::vector<bool>(n, false));

    // 初始化：单个节点自身的路径存在
    for (size_t i = 0; i < n; ++i) {
        dp[1 << i][i] = true;
    }
    
    // 遍历所有可能的访问状态进行状态转移
    for (size_t i = 1; i < height; i++) {
        std::vector<size_t> zeros, ones;
        // 分离当前状态下访问过 (ones) 和未访问过 (zeros) 的节点
        for (size_t pos = 0; pos < n; ++pos) {
            if ((1 << pos) & i) {
                ones.push_back(pos);
            } else {
                zeros.push_back(pos);
            }
        }

        // 尝试从已访问的结尾点 o 扩展到未访问的点 z
        for (auto &o : ones) {
            if (!dp[i][o]) {
                continue;
            }

            for (auto &z : zeros) {
                if (!routes[o][z]) {
                    continue;
                }
                dp[i + (1 << z)][z] = true; // 状态转移，更新集合中包含 z 且以 z 结尾
            }
        }
    }

    // 检查是否存在能遍历所有节点（所有 bit 均为 1，即 height-1）的哈密顿路径
    bool is_cycle = false;
    for (size_t i = 0; i < n; i++) {
        is_cycle |= dp[height - 1][i];
        if (is_cycle) {
            break; // 只要有一条哈密顿路径成立即可
        }
    }
    return is_cycle;
}

/**
 * @brief 测试 1：测试单纯的哈密顿路径 `1 -> 2 -> 3 -> 4`
 */
static void test1() {
    std::vector<std::vector<bool>> arr{
        std::vector<bool>({true, true, false, false}),
        std::vector<bool>({false, true, true, false}),
        std::vector<bool>({false, false, true, true}),
        std::vector<bool>({false, false, false, true})};

    bool ans = hamilton_cycle(arr);
    std::cout << "Test 1... ";
    assert(ans); // 只有路径，没有回到起点，但也返回了 true
    std::cout << "passed\n";
}

/**
 * @brief 测试 2：测试无哈密顿路径的图
 */
static void test2() {
    std::vector<std::vector<bool>> arr{
        std::vector<bool>({true, true, false, false}),
        std::vector<bool>({false, true, true, true}),
        std::vector<bool>({false, false, true, false}),
        std::vector<bool>({false, false, false, true})};

    bool ans = hamilton_cycle(arr);

    std::cout << "Test 2... ";
    assert(!ans); // 无路径，返回 false
    std::cout << "passed\n";
}

/**
 * @brief 测试 3：全连通图（完全图）测试
 */
static void test3() {
    std::vector<std::vector<bool>> arr{
        std::vector<bool>({true, true, true, true}),
        std::vector<bool>({true, true, true, true}),
        std::vector<bool>({true, true, true, true}),
        std::vector<bool>({true, true, true, true})};

    bool ans = hamilton_cycle(arr);

    std::cout << "Test 3... ";
    assert(ans);
    std::cout << "passed\n";
}

/**
 * @brief 主函数
 */
int main() {
    test1();
    test2();
    test3();
    return 0;
}
