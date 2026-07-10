/**
 * @file
 * @brief [Eight Queens](https://en.wikipedia.org/wiki/Eight_queens_puzzle) puzzle, printing all solutions (N 皇后输出所有解的回溯算法实现)
 *
 * @author [Himani Negi](https://github.com/Himani2000)
 * @author [David Leal](https://github.com/Panquesito7)
 *
 */
#include <array>     /// 用于 std::array
#include <iostream>  /// 用于输入输出

/**
 * @brief 回溯算法命名空间
 * @namespace backtracking
 */
namespace backtracking {
/**
 * @brief N 皇后问题（输出所有解）算法命名空间
 * @namespace n_queens_all_solutions
 */
namespace n_queens_all_solutions {
/**
 * @brief 打印当前棋盘矩阵方案的辅助函数
 * @tparam n 棋盘的尺寸
 * @param board 棋盘矩阵（1 代表放了皇后，0 代表空）
 */
template <size_t n>
void PrintSol(const std::array<std::array<int, n>, n>& board) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            std::cout << board[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

/**
 * @brief 检查是否可以在指定位置 board[row][col] 安全放置皇后
 * 
 * @tparam n 棋盘的尺寸
 * @param board 二维棋盘状态
 * @param row 目标行索引
 * @param col 目标列索引
 * @returns `true` 表示当前位置安全，没有皇后可以互相攻击；`false` 表示冲突，不安全
 */
template <size_t n>
bool CanIMove(const std::array<std::array<int, n>, n>& board, int row,
              int col) {
    /// 1. 检查同一行的左半部分
    for (int i = 0; i < col; i++) {
        if (board[row][i] == 1) {
            return false;
        }
    }
    /// 2. 检查左上对角线
    for (int i = row, j = col; i >= 0 && j >= 0; i--, j--) {
        if (board[i][j] == 1) {
            return false;
        }
    }
    /// 3. 检查左下对角线
    for (int i = row, j = col; i <= n - 1 && j >= 0; i++, j--) {
        if (board[i][j] == 1) {
            return false;
        }
    }
    return true;
}

/**
 * @brief N 皇后回溯递归核心求解函数（搜索并输出所有可行解）
 * @tparam n 棋盘的尺寸
 * @param board 棋盘状态的拷贝，传值方式便于状态自动回滚
 * @param col 当前正尝试放置皇后的列索引
 */
template <size_t n>
void NQueenSol(std::array<std::array<int, n>, n> board, int col) {
    // 递归出口：如果所有列均成功放置了皇后，则说明找到了一个可行解，打印出来并返回
    if (col >= n) {
        PrintSol(board);
        return;
    }
    
    // 在第 col 列，尝试将皇后放置在每一行（0 到 n-1）
    for (int i = 0; i < n; i++) {
        // 检测在 board[i][col] 放置皇后是否安全
        if (CanIMove(board, i, col)) {
            // 做出选择
            board[i][col] = 1;
            
            // 递归进入下一列放置皇后
            NQueenSol(board, col + 1);
            
            // 撤销选择（回溯）
            board[i][col] = 0;
        }
    }
}
}  // namespace n_queens_all_solutions
}  // namespace backtracking

/**
 * @brief 主函数，演示求解 4 皇后问题并输出其全部可行解
 * @returns 0
 */
int main() {
    const int n = 4; // 4 皇后
    std::array<std::array<int, n>, n> board{0}; // 初始化为空棋盘

    // 启动求解，从第 0 列开始
    backtracking::n_queens_all_solutions::NQueenSol(board, 0);
    return 0;
}
