/**
 * @file
 * @brief Implementation of Longest Common Subsequence with backtracking trace (带回溯路径的最长公共子序列算法实现)
 * @details
 * 计算并输出两个字符串 a 和 b 的最长公共子序列（LCS）。
 *
 * ### 动态规划状态转移与回溯
 * 1. 状态矩阵 `res[i][j]`：代表 `a[0...i-1]` 与 `b[0...j-1]` 的 LCS 长度。
 * 2. 路径跟踪矩阵 `trace[i][j]`：记录当前状态是从哪个子状态转移而来的。
 *    - 1：来自左上方对角线 `res[i-1][j-1]`（匹配成功，当前字符属于 LCS）
 *    - 2：来自上方 `res[i-1][j]`
 *    - 3：来自左方 `res[i][j-1]`
 * 3. 递归打印：根据 `trace` 矩阵从右下角反向回溯并输出对应的字符。
 *
 * @note
 * 【严重数组越界/缓冲区溢出 Bug 说明】：
 * 在第 23 行，作者将跟踪路径的数组定义为固定大小的栈空间矩阵：
 *   `int trace[20][20];`
 * 如果输入的字符串 `a` 或 `b` 的长度大于等于 20：
 * 当双层循环进行到 `i >= 20` 或 `j >= 20` 时，执行 `trace[i][j] = ...` 将引发**数组越界（Out of Bounds）和缓冲区溢出（Buffer Overflow）**。
 * 这会导致栈内存被破坏、数据被污染甚至程序直接崩溃。
 *
 * 修正方法：应当改用 `std::vector<std::vector<int>> trace(m + 1, std::vector<int>(n + 1))` 进行动态分配。
 *
 * 时间复杂度: O(m * n)
 * 空间复杂度: O(m * n)
 */

#include <iostream>
#include <vector>
using namespace std;

/**
 * @brief 递归回溯打印最长公共子序列
 * @param trace 路径记录矩阵（警告：硬编码固定大小为 20x20，存在越界风险）
 * @param m 字符串 a 的比对索引
 * @param n 字符串 b 的比对索引
 * @param a 源字符串 a
 */
void Print(int trace[20][20], int m, int n, string a) {
    if (m == 0 || n == 0) {
        return;
    }
    if (trace[m][n] == 1) {
        Print(trace, m - 1, n - 1, a);
        cout << a[m - 1]; // 匹配，输出该字符
    } else if (trace[m][n] == 2) {
        Print(trace, m - 1, n, a);
    } else if (trace[m][n] == 3) {
        Print(trace, m, n - 1, a);
    }
}

/**
 * @brief 计算 LCS 并输出子序列内容
 * @param a 字符串一
 * @param b 字符串二
 * @returns LCS 的最大长度
 */
int lcs(string a, string b) {
    int m = a.length(), n = b.length();
    std::vector<std::vector<int> > res(m + 1, std::vector<int>(n + 1));
    int trace[20][20]; // 警告 Bug：栈分配固定大小，当输入串长度 >= 20 时越界崩溃

    // 初始化状态矩阵和路径矩阵
    for (int i = 0; i < m + 1; i++) {
        for (int j = 0; j < n + 1; j++) {
            res[i][j] = 0;
            trace[i][j] = 0;
        }
    }

    // 递推状态转移
    for (int i = 0; i < m + 1; ++i) {
        for (int j = 0; j < n + 1; ++j) {
            if (i == 0 || j == 0) {
                res[i][j] = 0;
                trace[i][j] = 0;
            }
            else if (a[i - 1] == b[j - 1]) {
                res[i][j] = 1 + res[i - 1][j - 1];
                trace[i][j] = 1;  // 对角线移入
            } else {
                if (res[i - 1][j] > res[i][j - 1]) {
                    res[i][j] = res[i - 1][j];
                    trace[i][j] = 2;  // 向上移入
                } else {
                    res[i][j] = res[i][j - 1];
                    trace[i][j] = 3;  // 向左移入
                }
            }
        }
    }
    
    // 回溯输出公共子序列
    Print(trace, m, n, a);
    return res[m][n];
}

/**
 * @brief 主函数
 */
int main() {
    string a, b;
    // 读入两个待比对字符串
    cin >> a >> b;
    cout << lcs(a, b);
    return 0;
}
