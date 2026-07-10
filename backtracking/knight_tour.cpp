/**
 * @file
 * @brief [Knight's tour](https://en.wikipedia.org/wiki/Knight%27s_tour) algorithm (骑士巡游/马踏棋盘回溯算法实现)
 *
 * @details
 * 骑士巡游问题：在 N×N 的国际象棋棋盘上，棋子“马”（骑士）从任意指定方格出发，
 * 能否不重复地走遍棋盘上的每一个方格。
 * 如果可以走遍且最后一步正好可以一步回到起点，则是封闭巡游；否则是开放巡游。
 * 本程序使用经典的回溯法求解。
 *
 * @author [Nikhil Arora](https://github.com/nikhilarora068)
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
 * @brief 骑士巡游算法命名空间
 * @namespace knight_tour
 */
namespace knight_tour {
/**
 * @brief 检查坐标 (x, y) 是否在 N*N 棋盘有效范围内，且尚未被骑士访问过
 * @tparam V 棋盘尺寸（顶点数）
 * @param x 目标位置的行索引
 * @param y 目标位置的列索引
 * @param sol 存储移动步数的棋盘矩阵
 * @returns `true` 如果该位置安全可达；`false` 如果越界或已被访问过
 */
template <size_t V>
bool issafe(int x, int y, const std::array<std::array<int, V>, V> &sol) {
    return (x < V && x >= 0 && y < V && y >= 0 && sol[x][y] == -1);
}

/**
 * @brief 骑士巡游问题求解递归回溯函数
 * @tparam V 棋盘尺寸
 * @param x 当前骑士所处的行位置
 * @param y 当前骑士所处的列位置
 * @param mov 当前的移动步数编号
 * @param sol 存储游历路径步数的矩阵
 * @param xmov 骑士移动的可选 x 方向偏量（共 8 个方向）
 * @param ymov 骑士移动的可选 y 方向偏量（共 8 个方向）
 * @returns `true` 找到可行巡游解，终止递归；`false` 未找到可行解，进行回溯
 */
template <size_t V>
bool solve(int x, int y, int mov, std::array<std::array<int, V>, V> &sol,
           const std::array<int, V> &xmov, std::array<int, V> &ymov) {
    int k = 0, xnext = 0, ynext = 0;

    // 成功出口：当步数等于棋盘方格总数时，说明所有格子都刚好被访问了一次
    if (mov == V * V) {
        return true;
    }

    // 遍历骑士在当前格子上可能跳跃的 8 个方向
    for (k = 0; k < V; k++) {
        xnext = x + xmov[k];
        ynext = y + ymov[k];

        // 检查下一位置是否安全可行
        if (issafe<V>(xnext, ynext, sol)) {
            // 做出选择：标记该格子已被访问，记录当前的移动步数
            sol[xnext][ynext] = mov;

            // 递归步骤：从下一格子出发，尝试寻找剩余格子走法
            if (solve<V>(xnext, ynext, mov + 1, sol, xmov, ymov) == true) {
                return true; // 递归找到成功解，逐层返回 true
            } else {
                // 撤销选择（回溯）：如果当前选择无法导致终极解，则重置为 -1，尝试其他方向
                sol[xnext][ynext] = -1;
            }
        }
    }
    return false; // 8个方向都行不通，返回 false 触发上一层回溯
}
}  // namespace knight_tour
}  // namespace backtracking

/**
 * @brief 主函数，初始化 8x8 棋盘并运行骑士巡游求解
 * @returns 0
 */
int main() {
    const int n = 8; // 8x8 棋盘
    std::array<std::array<int, n>, n> sol = {0};

    // 1. 初始化棋盘，用 -1 代表方格未被访问
    int i = 0, j = 0;
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            sol[i][j] = -1;
        }
    }

    // 定义骑士 8 种可能的 L 形跳跃方向偏量
    std::array<int, n> xmov = {2, 1, -1, -2, -2, -1, 1, 2};
    std::array<int, n> ymov = {1, 2, 2, 1, -1, -2, -2, -1};

    // 起点设在 (0, 0)，步数标记为 0
    sol[0][0] = 0;

    // 启动回溯求解，步数编号从 1 开始
    bool flag = backtracking::knight_tour::solve<n>(0, 0, 1, sol, xmov, ymov);
    if (flag == false) {
        std::cout << "Error: Solution does not exist\n";
    } else {
        // 输出骑士巡游的完整步数路线图
        for (i = 0; i < n; i++) {
            for (j = 0; j < n; j++) {
                std::cout << sol[i][j] << "  ";
            }
            std::cout << "\n";
        }
    }
    return 0;
}
