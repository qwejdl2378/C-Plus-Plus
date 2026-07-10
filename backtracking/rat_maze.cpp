/**
 * @file
 * @brief Implements [Rat in a Maze](https://www.codesdope.com/blog/article/backtracking-to-solve-a-rat-in-a-maze-c-java-pytho/) algorithm (迷宫老鼠回溯算法实现)
 *
 * @details
 * 迷宫老鼠问题：给定一个 N×N 的迷宫（二进制矩阵表示），老鼠的起点是左上角 maze[0][0]，
 * 终点是右下角 maze[N-1][N-1]。老鼠只能向下或向右两个方向移动。
 * 在迷宫矩阵中，0 代表死胡同（不可通行），1 代表可以通过的路。
 * 目标是寻找一条从起点到终点的可行通路。
 *
 * @author [Vaibhav Thakkar](https://github.com/vaithak)
 * @author [David Leal](https://github.com/Panquesito7)
 */

#include <array>     /// 用于 std::array
#include <cassert>   /// 用于 assert
#include <iostream>  /// 用于输入输出

/**
 * @brief 回溯算法命名空间
 * @namespace backtracking
 */
namespace backtracking {
/**
 * @brief 迷宫老鼠算法命名空间
 * @namespace rat_maze
 */
namespace rat_maze {
/**
 * @brief 迷宫老鼠求解的核心回溯递归函数
 * @tparam size 迷宫的尺寸
 * @param currposrow 骑士（老鼠）当前所处的行索引
 * @param currposcol 骑士（老鼠）当前所处的列索引
 * @param maze 迷宫原始地图矩阵
 * @param soln 记录路径的可行解矩阵（传值拷贝，以备回溯自动还原）
 * @returns `true` 找到一条成功到达终点的路径；`false` 当前分支失败，触发回溯
 */
template <size_t size>
bool solveMaze(int currposrow, int currposcol,
               const std::array<std::array<int, size>, size> &maze,
               std::array<std::array<int, size>, size> soln) {
    // 递归出口：老鼠成功到达右下角的终点 (size-1, size-1)
    if ((currposrow == size - 1) && (currposcol == size - 1)) {
        soln[currposrow][currposcol] = 1; // 标记终点在路径中
        
        // 打印当前找到的可行通路路线图 (1 代表路径，0 代表非路径)
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                std::cout << soln[i][j] << " ";
            }
            std::cout << std::endl;
        }
        return true; // 成功找到解
    } else {
        // 做出选择：标记当前位置为路径的一部分
        soln[currposrow][currposcol] = 1;

        // 尝试选择 1：向右移动一格（列数加一）
        // 需满足：不超出列边界，且迷宫右侧格子可通行，且递归子路径成功
        if ((currposcol < size - 1) && maze[currposrow][currposcol + 1] == 1 &&
            solveMaze(currposrow, currposcol + 1, maze, soln)) {
            return true;
        }

        // 尝试选择 2：向下移动一格（行数加一）
        // 需满足：不超出行边界，且迷宫下方格子可通行，且递归子路径成功
        if ((currposrow < size - 1) && maze[currposrow + 1][currposcol] == 1 &&
            solveMaze(currposrow + 1, currposcol, maze, soln)) {
            return true;
        }

        // 撤销选择（回溯）：如果向右和向下都无法到达终点，则重置当前位置为 0，并返回 false
        soln[currposrow][currposcol] = 0;
        return false;
    }
}
}  // namespace rat_maze
}  // namespace backtracking

/**
 * @brief 自测用例集
 */
static void test() {
    const int size = 4;
    // 构建一个 4x4 的迷宫地图
    std::array<std::array<int, size>, size> maze = {
        std::array<int, size>{1, 0, 1, 0}, std::array<int, size>{1, 0, 1, 1},
        std::array<int, size>{1, 0, 0, 1}, std::array<int, size>{1, 1, 1, 1}};

    std::array<std::array<int, size>, size> soln{};

    // 将求解矩阵初始化为全 0
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            soln[i][j] = 0;
        }
    }

    int currposrow = 0;  // 从第 0 行开始
    int currposcol = 0;  // 从第 0 列开始

    // 断言求解成功
    assert(backtracking::rat_maze::solveMaze<size>(currposrow, currposcol, maze,
                                                   soln) == true);
}

/**
 * @brief 主函数
 * @returns 0
 */
int main() {
    test();  // 运行测试用例
    return 0;
}
