/**
 * @file
 * @brief Implementation of the Minimum Coin Change Problem (最少硬币找零问题)
 * @details
 * 给定一个硬币面值数组 `arr` 和一个总金额 `N`。
 * 求凑齐总金额 `N` 所需的最少硬币个数。如果无法凑齐，返回特定的无法达成状态（在本程序中，未找到时会保留为初始大整数值，非标准无解返回 -1）。
 *
 * ### 动态规划递推关系
 * 设 `dp[i]` 为凑齐金额 `i` 所需的最少硬币数。
 * - 基础状态：`dp[0] = 0`（凑齐 0 元需要 0 个硬币）
 * - 状态转移方程：对每个金额 `i` 和每种硬币面值 `c`：
 *   若 `i - c >= 0` 且 `dp[i - c] != INT_MAX`，则：
 *   `dp[i] = min(dp[i], dp[i - c] + 1)`
 *
 * @note
 * 【代码设计与改进建议】：
 * 在第 18 行中，`res` 变量被声明在内层硬币循环之外。这使得如果前一个硬币更新了 `res`，
 * 而下一个硬币因为 `i - arr[c] < 0` 未进入 if 分支，`res` 会保持上一次的旧值，导致重复执行一次无意义的 `min` 比较。
 * 虽然这在数学结果上是无害的，但代码规范上更推荐将 `res` 声明在 `for (int c = 0; c < n; c++)` 循环内部，使生命周期局部化。
 *
 * 时间复杂度: O(N * n)
 * 空间复杂度: O(N)
 */

#include <climits>
#include <iostream>
#include <vector>
using namespace std;

/**
 * @brief 寻找凑出总金额 N 所需的最少硬币数
 * @param arr 硬币面值数组
 * @param n 硬币面值种类数
 * @param N 目标找零总金额
 * @returns 凑出总金额的最少硬币数量，若无法凑齐则返回较大值
 */
int findMinCoins(int arr[], int n, int N) {
    // dp[i] 存储凑齐金额 i 所需的最少硬币数
    std::vector<int> dp(N + 1);

    dp[0] = 0; // 凑齐 0 元需要 0 个硬币

    for (int i = 1; i <= N; i++) {
        dp[i] = INT_MAX; // 默认初始化为无穷大
        int res = INT_MAX;

        for (int c = 0; c < n; c++) {
            if (i - arr[c] >= 0) {
                res = dp[i - arr[c]]; // 获取子问题的最优解
            }

            // 如果该子问题有解，则更新当前金额的最优解
            if (res != INT_MAX) {
                dp[i] = min(dp[i], res + 1);
            }
        }
    }

    return dp[N];
}

/**
 * @brief 主函数
 */
int main() {
    // 现有的硬币面额
    int arr[] = {1, 2, 3, 4};
    int n = sizeof(arr) / sizeof(arr[0]);

    // 目标金额
    int N = 15;

    cout << "Minimum Number of Coins Required " << findMinCoins(arr, n, N)
         << "\n";

    return 0;
}