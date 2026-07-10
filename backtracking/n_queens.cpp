/**
 * @file
 * @brief [Eight Queens](https://en.wikipedia.org/wiki/Eight_queens_puzzle) puzzle (八皇后/N皇后问题的回溯算法实现)
 *
 * @details
 * **八皇后问题**是要在 8×8 的国际象棋棋盘上放置八个皇后，使得任何两个皇后都不能互相攻击；
 * 因此，解决方案要求没有任何两个皇后占据相同的行、列或对角线。
 * 八皇后问题是更通用的 **N 皇后问题** 的一个特例，即将 N 个互不攻击的皇后放置在 N×N 的棋盘上。
 * 出了 N = 2 和 N = 3 之外，所有自然数 N 都存在解决方案。
 *
 * @author Unknown author
 * @author [David Leal](https://github.com/Panquesito7)
 *
 */
#include <array>
#include <iostream>

/**
 * @namespace backtracking
 * @brief 回溯算法命名空间
 */
namespace backtracking {
/**
 * @namespace n_queens
 * @brief N皇后问题相关的辅助函数
 */
namespace n_queens {
/**
 * @brief 打印当前棋盘状态的辅助函数
 * @tparam n 棋盘的尺寸
 * @param board 棋盘二维数组（1 代表放了皇后，0 代表空）
 */
template <size_t n>
void printSolution(const std::array<std::array<int, n>, n> &board) {
    std::cout << "\n";
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            std::cout << "" << board[i][j] << " ";
        }
        std::cout << "\n";
    }
}

/**
 * @brief 检查在指定位置 board[row][col] 放置皇后是否安全
 * @details
 * 因为我们是按照从左到右的顺序一列一列放置皇后的，
 * 所以只需要检查当前位置的左侧区域即可：
 * 1. 检查同一行的左侧是否有皇后。
 * 2. 检查左上方对角线是否有皇后。
 * 3. 检查左下方对角线是否有皇后。
 *
 * @tparam n 棋盘尺寸
 * @param board 棋盘二维数组
 * @param row 目标行索引
 * @param col 目标列索引
 * @returns `true` 表示安全，可以放置；`false` 表示不安全，会发生冲突
 */
template <size_t n>
bool isSafe(const std::array<std::array<int, n>, n> &board, const int &row,
            const int &col) {
    int i = 0, j = 0;

    // 1. 检查当前行的左半部分
    for (i = 0; i < col; i++) {
        if (board[row][i]) {
            return false;
        }
    }

    // 2. 检查左上方斜对角线
    for (i = row, j = col; i >= 0 && j >= 0; i--, j--) {
        if (board[i][j]) {
            return false;
        }
    }
    // 3. 检查左下方斜对角线
    for (i = row, j = col; j >= 0 && i < n; i++, j--) {
        if (board[i][j]) {
            return false;
        }
    }
    return true;
}

/**
 * @brief 递归求解 N 皇后问题的主回溯函数
 * @tparam n 棋盘尺寸
 * @param board 拷贝传入的当前棋盘状态
 * @param col 当前正尝试放置皇后的列索引
 */
template <size_t n>
void solveNQ(std::array<std::array<int, n>, n> board, const int &col) {
    // 递归出口：如果所有列都已经成功放置了皇后，则打印一种可行方案并返回
    if (col >= n) {
        printSolution<n>(board);
        return;
    }

    // 在当前的第 col 列中，尝试逐行尝试放置皇后
    for (int i = 0; i < n; i++) {
        // 检查将皇后放置在 board[i][col] 是否安全
        if (isSafe<n>(board, i, col)) {
            // 做出选择：放置皇后
            board[i][col] = 1;

            // 递归步骤：尝试在下一列中放置皇后
            solveNQ<n>(board, col + 1);

            // 撤销选择（回溯）：将当前位置重新设为 0，以便尝试其他行的行
            board[i][col] = 0;  // backtrack
        }
    }
}
}  // namespace n_queens
}  // namespace backtracking

/**
 * @brief 主函数，演示 4 皇后问题的所有解
 * @returns 0
 */
int main() {
    const int n = 4;
    // 初始化一个 4x4 的全零棋盘
    std::array<std::array<int, n>, n> board = {
        std::array<int, n>({0, 0, 0, 0}), std::array<int, n>({0, 0, 0, 0}),
        std::array<int, n>({0, 0, 0, 0}), std::array<int, n>({0, 0, 0, 0})};

    // 从第 0 列开始放置皇后并寻找解
    std::cout << "Solutions for 4-Queens Problem:";
    backtracking::n_queens::solveNQ<n>(board, 0);
    return 0;
}
