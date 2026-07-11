/**
 * @file
 * @brief Implementation of the [Matrix Chain Multiplication](https://en.wikipedia.org/wiki/Matrix_chain_multiplication) algorithm (矩阵链乘法动态规划算法实现)
 *
 * @details
 * 矩阵链乘法问题：给定一序列矩阵，我们需要确定它们的相乘顺序，以使所需的标量乘法总次数最少。
 * 注意：这只是为了寻找最优计算顺序（加括号方式），并不实际执行矩阵乘法。
 *
 * ### 动态规划状态转移（带备忘录的自顶向下）
 * 设 `MatrixChainMultiplication(i, j)` 表示将矩阵链 M[i+1]...M[j] 相乘的最少标量乘法次数：
 * - 边界条件：若 `j <= i + 1`，说明只有一个矩阵或没有矩阵，相乘代价为 0。
 * - 递推关系：对每个分割位置 `k`（从 `i+1` 到 `j-1`），
 *   `cost = MCM(i, k) + MCM(k, j) + dim[i] * dim[k] * dim[j]`。
 *   求所有可能的 `k` 切分点中 `cost` 的极小值作为 `dp[i][j]` 的值。
 *
 * @note
 * 【硬编码数组上限警示】：
 * 代码中硬编码了全局静态备忘录 `int dp[MAX][MAX]`，且 `MAX = 10`。
 * 这意味着输入的矩阵维度数组 `dim` 的长度 `n` 绝对不能超过 10，否则会导致**缓冲区溢出越界崩溃**。
 * 建议在实际使用中，将 `dp` 替换为动态分配的 `std::vector<std::vector<int>>` 以支持任意规模的输入。
 */

#include <climits>
#include <iostream>
using namespace std;

#define MAX 10 // 警告：限制了矩阵数量不能超过 10

// 备忘录表格，用于存储子问题的解，初始化为 0 代表未计算
int dp[MAX][MAX];

/**
 * @brief 自顶向下备忘录法求解矩阵链乘积最小代价
 * @param dim 存储各个矩阵维度的数组，矩阵 i 的维度为 dim[i-1] x dim[i]
 * @param i 矩阵链的起点索引
 * @param j 矩阵链的终点索引
 * @returns 最少标量乘法次数
 */
int MatrixChainMultiplication(int dim[], int i, int j) {
    // 边界条件：仅剩一个矩阵时，不需要乘法
    if (j <= i + 1)
        return 0;

    int min_cost = INT_MAX;

    // 如果该子问题还没有计算过，则进行递归并缓存
    if (dp[i][j] == 0) {
        // 遍历所有可能的划分位置 k，将其分为 M[i+1]..M[k] 和 M[k+1]..M[j]
        for (int k = i + 1; k <= j - 1; k++) {
            // 左子链的代价
            int cost = MatrixChainMultiplication(dim, i, k);

            // 右子链的代价
            cost += MatrixChainMultiplication(dim, k, j);

            // 两子矩阵结果相乘的代价
            cost += dim[i] * dim[k] * dim[j];

            if (cost < min_cost)
                min_cost = cost;  // 记录最小开销
        }
        dp[i][j] = min_cost; // 缓存结果
    }

    return dp[i][j];
}

/**
 * @brief 主函数
 */
int main() {
    // 维度数组：分别表示 10x30, 30x5, 5x60 三个矩阵
    int dim[] = {10, 30, 5, 60};
    int n = sizeof(dim) / sizeof(dim[0]);

    cout << "Minimum cost is " << MatrixChainMultiplication(dim, 0, n - 1)
         << "\n";

    return 0;
}