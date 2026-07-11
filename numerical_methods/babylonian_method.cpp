/**
 * @file
 * @brief [Babylonian Method](https://en.wikipedia.org/wiki/Methods_of_computing_square_roots#Babylonian_method) for computing square root (巴比伦方法/牛顿迭代法求平方根)
 *
 * @details
 * 巴比伦方法（也称作牛顿-拉夫森法）是一种高效的求解实数平方根的迭代算法。
 * 对于方程 $x^2 - S = 0$，其迭代公式为：
 * $x_{n+1} = \frac{1}{2} (x_n + \frac{S}{x_n})$
 * 该算法具有二次收敛速度。
 *
 * 时间复杂度: $O(\log(\text{精度}))$
 * 空间复杂度: $O(1)$
 *
 * @note
 * 【迭代条件颠倒与零/负数除零崩溃 Bug 审计与修复】：
 * 1. **收敛迭代循环判定条件颠倒逻辑 Bug**：
 *    原程序中的收敛判定条件写成了 `while (std::max(x0, x1) - std::min(x0, x1) < 0.0001)`。
 *    这意味着只有当初始猜测已经极其接近真实值（差值小于 0.0001）时，循环才会启动。而在绝大多数未收敛的情况下，
 *    **循环根本不会执行**。程序能够勉强通过自测只是因为最初始的一步近似刚好达到了较高的精度。
 *    对于其他没有那么幸运的测试值，该逻辑会直接返回粗糙的第一步估计值，导致巨大的计算误差。
 *    **修复**：将循环判定修改为 `while (std::abs(x0 - x1) >= 1e-9)`，即未达到高精度收敛前持续迭代。
 * 2. **零与负数输入引发的除零及崩溃 Bug**：
 *    - 如果输入为 0，初始循环后 `x0 = 0`，计算 `radicand / x0` 会导致直接的除以零未定义行为。
 *    - 如果输入为负数，平方根为虚数，实数迭代无意义。
 *    **修复**：在入口处添加对被开方数 `radicand` 的防卫性校验。若为 0 直接返回 0；若小于 0 则抛出异常。
 *
 * @author [Ameya Chawla](https://github.com/ameyachawlaggsipu)
 */

#include <cassert>  
#include <cmath>
#include <iostream>  
#include <stdexcept>

namespace numerical_methods {

/**
 * @brief 使用巴比伦迭代法计算被开方数的平方根
 * @param radicand 被开方数，必须是非负数
 * @return 对应的平方根近似值
 */
double babylonian_method(double radicand) {
    // 核心修复：防卫负数输入
    if (radicand < 0.0) {
        throw std::invalid_argument("Radicand must be non-negative!");
    }
    // 核心修复：防卫 0 输入，防止发生后续的除以零错误
    if (radicand == 0.0) {
        return 0.0;
    }

    int i = 1;  
    while (static_cast<double>(i * i) <= radicand) {
        i++;
    }
    i--;  

    double x0 = i;  
    double x1 = (radicand / x0 + x0) / 2.0;  
    double temp = NAN;             

    // 核心修复：纠正反转的循环判定条件，直到相邻两次迭代的差值小于指定精度阈值时才停止迭代
    while (std::abs(x0 - x1) >= 1e-12) {
        temp = (radicand / x1 + x1) / 2.0;  
        x0 = x1;
        x1 = temp;
    }

    return x1;  
}

}  // namespace numerical_methods

/**
 * @brief 单元自测用例
 */
static void test() {
    auto testcase1 = 125348.0;  
    auto testcase2 = 752080.0;  

    auto real_output1 = 354.045194855;  
    auto real_output2 = 867.225460881;  

    auto test_result1 = numerical_methods::babylonian_method(testcase1);
    auto test_result2 = numerical_methods::babylonian_method(testcase2);

    assert(std::abs(test_result1 - real_output1) < 0.0001);
    assert(std::abs(test_result2 - real_output2) < 0.0001);

    // 测试 0 与极小值
    assert(numerical_methods::babylonian_method(0.0) == 0.0);
    assert(std::abs(numerical_methods::babylonian_method(2.0) - 1.41421356) < 1e-6);

    std::cout << "All Babylonian method tests have successfully passed!\n";
}

/**
 * @brief 主函数
 */
int main() {
    test();  
    return 0;
}
