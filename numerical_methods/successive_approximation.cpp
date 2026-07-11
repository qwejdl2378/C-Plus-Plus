/**
 * @file
 * @brief Solve nonlinear equations using [Fixed-Point Iteration Method](https://en.wikipedia.org/wiki/Fixed-point_iteration) (不动点迭代法/连续逼近法求解非线性方程的根)
 *
 * @details
 * 不动点迭代法（连续逼近法）用于求解方程 $f(x) = 0$。
 * 首先将方程重写为等价的不动点形式：$x = g(x)$。
 * 然后选择一个初始近似值 $x_0$，执行以下递推迭代：
 *
 * $x_{n+1} = g(x_n)$
 *
 * 如果对于区间内的所有 $x$ 满足 $|g'(x)| < 1$，则该迭代序列收敛于唯一的根（不动点）。
 * 本实现中，目标方程为：
 * $f(y) = 3y - \cos(y) - 2 = 0$
 * 重写为不动点形式为：
 * $y = g(y) = \frac{\cos(y) + 2}{3}$
 *
 * 时间复杂度: $O(\text{迭代次数})$
 * 空间复杂度: $O(1)$
 *
 * @note
 * 【未初始化局部变量、公式不匹配与交互挂起 Bug 审计与修复】：
 * 1. **未初始化局部变量导致的未定义行为 (UB) Bug**：
 *    原程序中声明 `float y;` 后，在未赋予任何初始值的情况下直接在循环中调用 `sum = eq(y);` 和 `y++;`。
 *    读取未初始化的局部变量在 C++ 中属于典型的未定义行为（Undefined Behavior），会导致在不同编译器下输出随机垃圾值甚至直接崩溃。
 *    **修复**：在声明时将 `y` 显式初始化为 `0.0f`。
 * 2. **数学公式不匹配/公式推导错误 Bug**：
 *    原程序的目标方程定义为 $3y - \cos(y) - 2 = 0$，但迭代函数 `eqd` 却写成了 `0.5 * (cos(y) + 2)`（除以 2）。
 *    这会导致迭代收敛到错误的方程 $2y - \cos(y) - 2 = 0$ 的根，使得 `eq` 的评估结果与根的本意彻底脱节。
 *    **修复**：修正 `eqd` 为 `(std::cos(y) + 2.0f) / 3.0f`。
 * 3. **交互挂起与测试缺失 Bug**：
 *    **修复**：默认以确定性初始值运行迭代自测，并加 `assert` 验证逼近结果是否收敛至方程根，杜绝终端交互阻塞。
 */

#include <cassert>
#include <cmath>
#include <iostream>

namespace numerical_methods {
namespace successive_approximation {

/**
 * @brief 目标方程定义: f(y) = 3y - cos(y) - 2
 */
double eq(double y) { 
    return (3.0 * y) - std::cos(y) - 2.0; 
}

/**
 * @brief 不动点迭代递推函数: g(y) = (cos(y) + 2) / 3
 * @note 核心修复：修正除数系数为 3.0 以匹配目标方程，使其收敛到同一个根。
 */
double eqd(double y) { 
    return (std::cos(y) + 2.0) / 3.0; 
}

/**
 * @brief 不动点连续迭代求解器
 * @param initial_x 初始迭代起点值
 * @param iterations 迭代次数
 * @return 逼近收敛的根
 */
double solve(double initial_x, int iterations) {
    double x = initial_x;
    for (int i = 0; i < iterations; ++i) {
        x = eqd(x);
    }
    return x;
}

}  // namespace successive_approximation
}  // namespace numerical_methods

/**
 * @brief 单元自测用例
 */
static void test() {
    using namespace numerical_methods::successive_approximation;

    // 核心修复：初始化变量 y = 0.0，杜绝未定义行为 (UB)
    double y = 0.0;
    std::cout << "Values of the equation during evaluation step:\n";
    for (int i = 0; i < 10; i++) {
        double sum = eq(y);
        std::cout << "value of equation at y=" << y << ": " << sum << "\n";
        y += 1.0;
    }

    // 运行迭代自测，初始猜测值为 0.0，迭代 50 次
    double root = solve(0.0, 50);
    std::cout << "Calculated root after 50 iterations: " << root << "\n";
    std::cout << "Equation value at calculated root f(root): " << eq(root) << "\n";

    // 校验解的正确性：f(root) 应极其接近 0
    assert(std::abs(eq(root)) < 1e-6);
    std::cout << "Successive approximation test passed successfully!\n";
}

/**
 * @brief 主函数
 */
int main() {
    test(); // 运行自动化自测
    return 0;
}
