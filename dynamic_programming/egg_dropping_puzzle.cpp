/**
 * @file
 * @brief Implementation of the classic [Egg Dropping Puzzle](https://en.wikipedia.org/wiki/Dynamic_programming#Egg_dropping_puzzle) (经典扔鸡蛋问题/双蛋问题)
 * @details
 * 扔鸡蛋问题：给定 n 个鸡蛋和 k 层楼，要求在最坏情况下，确定测出“鸡蛋从哪一层摔下去刚好会碎（临界楼层）”所需要的最小尝试次数。
 *
 * ### 动态规划状态转移
 * 设 `eggFloor[i][j]` 表示有 `i` 个鸡蛋和 `j` 层楼时，最坏情况下所需的最少尝试次数。
 * - 基础状态：
 *   - `eggFloor[i][0] = 0` （0 层楼不需要测试，尝试 0 次）
 *   - `eggFloor[i][1] = 1` （1 层楼只需要测试 1 次）
 *   - `eggFloor[1][j] = j` （只有 1 个鸡蛋时，只能从 1 楼开始逐层向上测试，最坏情况下需要 j 次）
 * - 递推关系：对每个楼层 `x`（从 1 到 `j`）尝试丢鸡蛋：
 *   1. 鸡蛋碎了：还剩 `i - 1` 个鸡蛋，需要测试下方的 `x - 1` 层楼，状态转移为 `eggFloor[i-1][x-1]`。
 *   2. 鸡蛋没碎：还剩 `i` 个鸡蛋，需要测试上方的 `j - x` 层楼，状态转移为 `eggFloor[i][j-x]`。
 *   因为是“最坏情况”，所以每次测试应取两者中次数的较大值；
 *   因为我们要找决策上的“最少次数”，所以对所有楼层选择 `x` 带来的结果取极小值：
 *   `eggFloor[i][j] = 1 + min_{1 <= x <= j} ( max(eggFloor[i - 1][x - 1], eggFloor[i][j - x]) )`
 *
 * 时间复杂度: O(N * K^2)
 * 空间复杂度: O(N * K)
 */

#include <climits>
#include <iostream>
#include <vector>

using namespace std;

/**
 * @brief 计算在最坏情况下确定临界楼层所需的最少尝试次数
 * @param n 鸡蛋的总数
 * @param k 楼层的总层数
 * @returns 最少尝试次数
 */
int eggDrop(int n, int k) {
    // eggFloor[i][j] 记录有 i 个鸡蛋和 j 层楼时的最少试验次数
    std::vector<std::vector<int> > eggFloor(n + 1, std::vector<int>(k + 1));

    int result;

    // 初始化基础情况
    for (int i = 1; i <= n; i++) {
        eggFloor[i][1] = 1;  // 1 层楼只需要试 1 次
        eggFloor[i][0] = 0;  // 0 层楼不需要试
    }

    // 只有 1 个鸡蛋的情况，只能逐层测试
    for (int j = 1; j <= k; j++) {
        eggFloor[1][j] = j;
    }

    // 自底向上填充 DP 二维表
    for (int i = 2; i <= n; i++) {
        for (int j = 2; j <= k; j++) {
            eggFloor[i][j] = INT_MAX;
            for (int x = 1; x <= j; x++) {
                // 最坏情况的可能：取 碎 (i-1, x-1) 与 没碎 (i, j-x) 的最大值
                result = 1 + max(eggFloor[i - 1][x - 1], eggFloor[i][j - x]);
                if (result < eggFloor[i][j])
                    eggFloor[i][j] = result; // 寻求所有 x 选择中的最小值
            }
        }
    }

    return eggFloor[n][k];
}

/**
 * @brief 主函数
 */
int main() {
    int n, k;
    cout << "Enter number of eggs and floors: ";
    cin >> n >> k;
    cout << "Minimum number of trials in worst case: " << eggDrop(n, k) << endl;
    return 0;
}
