/**
 * @file
 * @brief [N queens](https://en.wikipedia.org/wiki/Eight_queens_puzzle) all optimized (对称性优化的 N 皇后问题求解)
 *
 * @details
 * 本算法对 N 皇后问题进行了优化。因为 N 皇后棋盘具有左右对称性，
 * 只要在第一列只探索上半部分的行，找到解后，通过“翻转”/“镜像”可以直接获得下半部分对应的对称解，
 * 从而使搜索树的大小减半，显著提高了求解效率。
 *
 * @author [Sombit Bose](https://github.com/deadshotsb)
 * @author [David Leal](https://github.com/Panquesito7)
 */

#include <array>
#include <iostream>

/**
 * @namespace backtracking
 * @brief 回溯算法命名空间
 */
namespace backtracking {
/**
 * @namespace n_queens_optimized
 * @brief 优化后的 N 皇后算法命名空间
 */
namespace n_queens_optimized {
/**
 * @brief 打印当前棋盘方案，并同时打印其对称的镜像方案
 * @tparam n 棋盘的尺寸
 * @param board 棋盘矩阵
 */
template <size_t n>
void PrintSol(const std::array<std::array<int, n>, n> &board) {
    // 1. 打印当前找到的原生解决方案
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            std::cout << board[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
    
    // 2. 利用对称镜像，打印其上下翻转的镜面对称解决方案
    // 如果 n 是偶数，或者 n 是奇数且第一列的皇后不是正中间元素，则可以通过转置或翻转生成对称解
    if (n % 2 == 0 || (n % 2 == 1 && board[n / 2 + 1][0] != 1)) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                // 输出转置/翻转后的元素，实现对称解输出
                std::cout << board[j][i] << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
}

/**
 * @brief 检查是否可以在 board[row][col] 位置安全地放置皇后
 * @tparam n 棋盘尺寸
 * @param board 棋盘数组
 * @param row 待检测行索引
 * @param col 待检测列索引
 * @returns `true` 安全可以放置；`false` 与已有皇后冲突，不安全
 */
template <size_t n>
bool CanIMove(const std::array<std::array<int, n>, n> &board, int row,
              int col) {
    /// 1. 检查当前行的左侧
    for (int i = 0; i <= col; i++) {
        if (board[row][i] == 1) {
            return false;
        }
    }
    /// 2. 检查左上方斜对角线
    for (int i = row, j = col; i >= 0 && j >= 0; i--, j--) {
        if (board[i][j] == 1) {
            return false;
        }
    }
    /// 3. 检查左下方斜对角线
    for (int i = row, j = col; i <= n - 1 && j >= 0; i++, j--) {
        if (board[i][j] == 1) {
            return false;
        }
    }
    return true;
}

/**
 * @brief 递归回溯寻找 N 皇后问题的解决方案
 * @tparam n 棋盘尺寸
 * @param board 棋盘状态的拷贝
 * @param col 当前正放置皇后的列索引
 */
template <size_t n>
void NQueenSol(std::array<std::array<int, n>, n> board, int col) {
    // 递归出口：成功放置所有皇后，打印这组解（及其对称解）并返回
    if (col >= n) {
        PrintSol<n>(board);
        return;
    }
    
    // 在当前 col 列，尝试每一行
    for (int i = 0; i < n; i++) {
        if (CanIMove<n>(board, i, col)) {
            // 做出选择
            board[i][col] = 1;
            
            // 递归至下一列
            NQueenSol<n>(board, col + 1);
            
            // 撤销选择（回溯）
            board[i][col] = 0;
        }
    }
}
}  // namespace n_queens_optimized
}  // namespace backtracking

/**
 * @brief 主函数，演示 4 皇后问题利用第一列对称性的求解
 * @returns 0
 */
int main() {
    const int n = 4; // 4 皇后问题
    std::array<std::array<int, n>, n> board{};

    // 利用第一列的对称性，只需遍历第一列的前半部分行数即可
    if (n % 2 == 0) {
        // 如果 n 是偶数，只需遍历前 n/2 行即可 (例如 n=4，只需第 0 和第 1 行)
        for (int i = 0; i <= n / 2 - 1; i++) {
            if (backtracking::n_queens_optimized::CanIMove(board, i, 0)) {
                board[i][0] = 1;
                backtracking::n_queens_optimized::NQueenSol(board, 1);
                board[i][0] = 0;
            }
        }
    } else {
        // 如果 n 是奇数，只需遍历前 n/2 (向上取整) 行即可 (包含中间那一面)
        for (int i = 0; i <= n / 2; i++) {
            if (backtracking::n_queens_optimized::CanIMove(board, i, 0)) {
                board[i][0] = 1;
                backtracking::n_queens_optimized::NQueenSol(board, 1);
                board[i][0] = 0;
            }
        }
    }
    return 0;
}
