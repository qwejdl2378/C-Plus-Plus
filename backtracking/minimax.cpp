/**
 * @file
 * @brief returns which is the longest/shortest number using [minimax](https://en.wikipedia.org/wiki/Minimax) algorithm (极大极小决策树算法实现)
 *
 * @details
 * 极小化极大 (Minimax) 算法是博弈论、决策论和人工智能中的一种核心决策规则，
 * 用于在最坏情况下（即对手做最利于他的选择时）最小化潜在的损失。
 * 对于追求最大利益的玩家（Max），目标是最大化最小收益；对于对手（Min），则是最小化 Max 的收益。
 * 本程序实现了一个用于二叉博弈树求解的最基础 Minimax 决策框架。
 *
 * @author [Gleison Batista](https://github.com/gleisonbs)
 * @author [David Leal](https://github.com/Panquesito7)
 */
#include <algorithm>  /// 用于 std::max, std::min
#include <array>      /// 用于 std::array
#include <cmath>      /// 用于 log2 树高计算
#include <iostream>   /// 用于输入输出

/**
 * @brief 回溯算法命名空间
 * @namespace backtracking
 */
namespace backtracking {
/**
 * @brief 极小化极大算法的核心递归函数
 * @tparam T 叶子节点数组的长度
 * @param depth 当前处于决策树的第几层深度 (根节点为 0)
 * @param node_index 当前处于决策树当前层位置映射在 scores 数组中的索引偏移
 * @param is_max 如果当前层是 Max 玩家回合（寻求最大化得分），则为 true；若是 Min 玩家回合，则为 false
 * @param scores 存储在叶子节点处的实际评估得分数组
 * @param height 决策树的最大高度 (即从根到叶子的层数)
 * @returns 在当前局面下，各玩家均采取最优策略时的评估得分
 */
template <size_t T>
int minimax(int depth, int node_index, bool is_max,
            const std::array<int, T> &scores, double height) {
    // 递归出口：当深度达到树高（叶子节点层）时，直接返回该叶子节点的评估分数
    if (depth == height) {
        return scores[node_index];
    }

    // 1. 递归计算左子分支（对应的子节点索引为 node_index * 2，并且玩家回合交替 !is_max）
    int v1 = minimax(depth + 1, node_index * 2, !is_max, scores, height);
    // 2. 递归计算右子分支（对应的子节点索引为 node_index * 2 + 1，并且玩家回合交替 !is_max）
    int v2 = minimax(depth + 1, node_index * 2 + 1, !is_max, scores, height);

    // 决策归并：若是 Max 玩家回合，选两个子节点中的最大值；若是 Min 玩家回合，选两个子节点中的最小值
    return is_max ? std::max(v1, v2) : std::min(v1, v2);
}
}  // namespace backtracking

/**
 * @brief 主函数，初始化叶子节点得分，计算整棵博弈树的根节点最优决策得分
 * @returns 0
 */
int main() {
    // 棋盘或游戏的最终叶子节点局面的评估分值（共 8 个叶子节点）
    std::array<int, 8> scores = {90, 23, 6, 33, 21, 65, 123, 34423};
    // 计算树的高度，对于 8 个叶子节点的满二叉树，高度为 log2(8) = 3
    double height = log2(scores.size());

    // 初始从根节点（深度 0，索引 0，且由先手的 Max 玩家开始决策）启动 minimax 算法
    std::cout << "Optimal value: "
              << backtracking::minimax(0, 0, true, scores, height) << std::endl;
    return 0;
}
