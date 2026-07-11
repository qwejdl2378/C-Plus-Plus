/**
 * @file
 * @brief Space-optimized Bottom-up Fibonacci calculation (空间优化的自底向上斐波那契数列计算)
 * @details
 * 使用 $O(N)$ 时间复杂度和 $O(1)$ 空间复杂度计算第 N 个斐波那契数。
 * 通过大小为 3 的数组进行滚动存贮，仅保留最近计算的两个斐波那契数。
 *
 * @note
 * 【零值边界 Bug 警示说明】：
 * 在第 3-13 行的 `fib` 函数中：
 * 如果输入的 `n = 0`，由于循环 `for (int i = 2; i <= n; i++)` 不会执行，
 * 函数将直接返回 `res[1]` 的初始值 1。
 * 按照标准的斐波那契定义（F(0)=0, F(1)=1），第 0 项应该是 0，因此这里存在一个边界 Bug。
 * 应当在函数开头增加特判：`if (n == 0) return 0;`。
 */

#include <iostream>
using namespace std;

/**
 * @brief 计算第 n 个斐波那契数
 * @param n 目标索引
 * @returns 对应的斐波那契数数值
 */
int fib(int n) {
    int res[3];
    res[0] = 0; // F(0)
    res[1] = 1; // F(1)
    
    // 迭代计算并将空间压缩至 3 个单元进行滚动更新
    for (int i = 2; i <= n; i++) {
        res[2] = res[1] + res[0];
        res[0] = res[1];
        res[1] = res[2];
    }
    return res[1]; // 警告：n=0 时会错误地返回 1
}

/**
 * @brief 主函数
 */
int main() {
    int n;
    cout << "Enter n: ";
    cin >> n;
    cout << "Fibonacci number is ";
    cout << fib(n) << endl;
    return 0;
}
