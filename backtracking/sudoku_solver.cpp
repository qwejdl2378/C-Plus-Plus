/**
 * @file
 * @brief [Sudoku Solver](https://en.wikipedia.org/wiki/Sudoku) algorithm. (数独回溯算法实现)
 *
 * @details
 * 数独问题：在一个 9×9 的网格中填入数字，使得每一行、每一列以及九个 3×3 的子网格（九宫格）
 * 内都包含数字 1 到 9 且不重复。
 * 本程序采用回溯法求解给定的数独难题，并使用 ANSI 转义码将解答中新填入的数字标记为黄色输出。
 *
 * @author [DarthCoder3200](https://github.com/DarthCoder3200)
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
 * @brief 数独求解算法命名空间
 * @namespace sudoku_solver
 */
namespace sudoku_solver {
/**
 * @brief 检查将数字 no 填入网格 mat[i][j] 是否可行（是否满足行、列和九宫格的唯一性约束）
 * @tparam V 矩阵维度尺寸（数独一般为 9）
 * @param mat 数独矩阵
 * @param i 目标行索引
 * @param j 目标列索引
 * @param no 尝试填入的数字
 * @param n 数独的规模大小 (9)
 * @returns `true` 表示当前放置安全可行；`false` 表示存在冲突
 */
template <size_t V>
bool isPossible(const std::array<std::array<int, V>, V> &mat, int i, int j,
                int no, int n) {
    /// 1. 检查第 i 行和第 j 列中是否已经存在数字 no
    for (int x = 0; x < n; x++) {
        if (mat[x][j] == no || mat[i][x] == no) {
            return false;
        }
    }

    /// 2. 检查对应的 3x3 小九宫格内是否已存在数字 no
    int sx = (i / 3) * 3; // 获取当前格子所属九宫格的起始行索引
    int sy = (j / 3) * 3; // 获取当前格子所属九宫格的起始列索引

    for (int x = sx; x < sx + 3; x++) {
        for (int y = sy; y < sy + 3; y++) {
            if (mat[x][y] == no) {
                return false;
            }
        }
    }

    return true; // 行、列、九宫格检测均无冲突，可行
}
/**
 * @brief 打印数独矩阵并使用颜色高亮标注解答填入部分的辅助函数
 * @tparam V 矩阵尺寸
 * @param mat 当前的数独状态
 * @param starting_mat 初始的数独状态（包含已知数），用于对比找出新填入的数
 * @param n 打印边界规模
 */
template <size_t V>
void printMat(const std::array<std::array<int, V>, V> &mat,
              const std::array<std::array<int, V>, V> &starting_mat, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (starting_mat[i][j] != mat[i][j]) {
                // 如果是新填入的数字，则使用黄颜色输出 (\033[93m 为 ANSI 黄色控制码)
                std::cout << "\033[93m" << mat[i][j] << "\033[0m"
                          << " ";
            } else {
                // 原始自带的数字正常输出
                std::cout << mat[i][j] << " ";
            }
            if ((j + 1) % 3 == 0) {
                std::cout << '\t'; // 打印九宫格列分界
            }
        }
        if ((i + 1) % 3 == 0) {
            std::cout << std::endl; // 打印九宫格行分界
        }
        std::cout << std::endl;
    }
}

/**
 * @brief 数独问题的递归回溯主函数
 * @tparam V 矩阵尺寸
 * @param mat 当前的数独求解矩阵
 * @param starting_mat 包含数独已知初始值的对比矩阵
 * @param i 当前正在处理的行索引
 * @param j 当前正在处理的列索引
 * @returns `true` 找到完美解，结束并返回；`false` 发生冲突，返回上一层执行回溯
 */
template <size_t V>
bool solveSudoku(std::array<std::array<int, V>, V> &mat,
                 const std::array<std::array<int, V>, V> &starting_mat, int i,
                 int j) {
    /// 递归出口 1：第 9 行也处理完毕（索引为 0 到 8），说明整个数独成功解出
    if (i == 9) {
        printMat<V>(mat, starting_mat, 9); // 打印求解方案
        return true;
    }

    /// 边界处理：当前行的所有列都处理完，跳转到下一行的首列继续
    if (j == 9) {
        return solveSudoku<V>(mat, starting_mat, i + 1, 0);
    }

    /// 如果当前格子已经有初始数字（非 0），直接跳过此格，处理下一列
    if (mat[i][j] != 0) {
        return solveSudoku<V>(mat, starting_mat, i, j + 1);
    }
    
    /// 当前格为空白（值为 0），尝试依次填入数字 1 到 9
    for (int no = 1; no <= 9; no++) {
        // 检查在该格子放入 no 是否可行
        if (isPossible<V>(mat, i, j, no, 9)) {
            /// 做出选择：将数字 no 填入网格中
            mat[i][j] = no;
            
            // 递归步骤：尝试解下一个格子
            bool solution_found = solveSudoku<V>(mat, starting_mat, i, j + 1);
            if (solution_found) {
                return true; // 发现可行解，向上传递并结束
            }
            
            // 如果递归失败，循环会继续尝试放置下一个数（不需要在此刻直接置 0，因为会被后面覆盖，但如果 1-9 都失败了，则在循环外置 0 回溯）
        }
    }
    /// 撤销选择（回溯）：1 到 9 的数字全都无法导致可行解，说明之前格子的填法有误，将当前格重新清零并回退
    mat[i][j] = 0;
    return false;
}
}  // namespace sudoku_solver
}  // namespace backtracking

/**
 * @brief 主函数，初始化数独谜题并启动求解
 * @returns 0
 */
int main() {
    const int V = 9;
    // 经典数独测试用例（0 代表空白格）
    std::array<std::array<int, V>, V> mat = {
        std::array<int, V>{5, 3, 0, 0, 7, 0, 0, 0, 0},
        std::array<int, V>{6, 0, 0, 1, 9, 5, 0, 0, 0},
        std::array<int, V>{0, 9, 8, 0, 0, 0, 0, 6, 0},
        std::array<int, V>{8, 0, 0, 0, 6, 0, 0, 0, 3},
        std::array<int, V>{4, 0, 0, 8, 0, 3, 0, 0, 1},
        std::array<int, V>{7, 0, 0, 0, 2, 0, 0, 0, 6},
        std::array<int, V>{0, 6, 0, 0, 0, 0, 2, 8, 0},
        std::array<int, V>{0, 0, 0, 4, 1, 9, 0, 0, 5},
        std::array<int, V>{0, 0, 0, 0, 8, 0, 0, 7, 9}};

    // 打印初始题目
    std::cout << "Starting Puzzle:" << std::endl;
    backtracking::sudoku_solver::printMat<V>(mat, mat, 9);
    
    std::cout << "Solving...\n" << std::endl;
    std::array<std::array<int, V>, V> starting_mat = mat;
    
    // 启动回溯求解，自坐标 (0, 0) 开始
    backtracking::sudoku_solver::solveSudoku<V>(mat, starting_mat, 0, 0);

    return 0;
}
