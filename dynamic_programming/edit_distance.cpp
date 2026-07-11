/**
 * @file
 * @brief Implementation of the Levenshtein Edit Distance algorithm (莱文斯坦编辑距离算法实现)
 * @details
 * 编辑距离（Edit Distance）用于计算将一个字符串 str1 转换为另一个字符串 str2 所需的最少操作次数。
 * 支持三种基本编辑操作，每种操作开销相同（权值为 1）：
 * a. 插入（Insert）
 * b. 删除（Remove）
 * c. 替换（Replace）
 *
 * 该文件提供了两种实现方式：
 * 1. 纯递归求解（`editDist`）：时间复杂度达 O(3^m)，存在大量重叠子问题的重复计算。
 * 2. 动态规划求解（`editDistDP`）：时间复杂度 O(m * n)，空间复杂度 O(m * n)，大大提高了运行效率。
 */

#include <iostream>
#include <string>
#include <vector>
using namespace std;

/**
 * @brief 辅助函数：求三个整数中的最小值
 */
int min(int x, int y, int z) { return min(min(x, y), z); }

/**
 * @brief 纯递归法计算编辑距离
 * @param str1 源字符串（警告：按值传递导致拷贝开销）
 * @param str2 目标字符串
 * @param m 源字符串的当前比对长度
 * @param n 目标字符串的当前比对长度
 * @returns 最少编辑距离
 */
int editDist(string str1, string str2, int m, int n) {
    // 如果 str1 为空，则只能插入所有 str2 字符
    if (m == 0)
        return n;
    // 如果 str2 为空，则只能删除所有 str1 字符
    if (n == 0)
        return m;

    // 如果末尾字符相同，则直接比对剩余字符，不增加编辑成本
    if (str1[m - 1] == str2[n - 1])
        return editDist(str1, str2, m - 1, n - 1);

    // 如果末尾字符不同，则尝试三种操作并求最小值：
    // a. 插入（Insert）: editDist(m, n - 1)
    // b. 删除（Remove）: editDist(m - 1, n)
    // c. 替换（Replace）: editDist(m - 1, n - 1)
    return 1 + min(editDist(str1, str2, m, n - 1),
                   editDist(str1, str2, m - 1, n),
                   editDist(str1, str2, m - 1, n - 1));
}

/**
 * @brief 动态规划（自底向上）计算编辑距离
 * @param str1 源字符串
 * @param str2 目标字符串
 * @param m 源字符串的总长度
 * @param n 目标字符串的总长度
 * @returns 最少编辑距离
 */
int editDistDP(string str1, string str2, int m, int n) {
    // dp[i][j] 代表 str1 前 i 个字符与 str2 前 j 个字符的编辑距离
    std::vector<std::vector<int> > dp(m + 1, std::vector<int>(n + 1));

    // 自底向上填充 dp 表
    for (int i = 0; i <= m; i++) {
        for (int j = 0; j <= n; j++) {
            // 如果 str1 为空，则只能插入所有 str2 的字符
            if (i == 0)
                dp[i][j] = j;

            // 如果 str2 为空，则只能删除所有 str1 的字符
            else if (j == 0)
                dp[i][j] = i;

            // 如果当前位置的字符相同，直接继承上一步的最优解
            else if (str1[i - 1] == str2[j - 1])
                dp[i][j] = dp[i - 1][j - 1];

            // 字符不同，取三种编辑操作中最省开销的值并加 1
            else
                dp[i][j] = 1 + min(dp[i][j - 1],     // 插入
                                   dp[i - 1][j],     // 删除
                                   dp[i - 1][j - 1]  // 替换
                               );
        }
    }

    return dp[m][n];
}

/**
 * @brief 主函数
 */
int main() {
    string str1 = "sunday";
    string str2 = "saturday";

    // 输出递归解和 DP 解以验证正确性
    cout << editDist(str1, str2, str1.length(), str2.length()) << endl;
    cout << editDistDP(str1, str2, str1.length(), str2.length()) << endl;

    return 0;
}
