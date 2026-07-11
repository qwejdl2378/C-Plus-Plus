/**
 * @file
 * @brief Solve the equation $f(x)=0$ using [False Position Method](https://en.wikipedia.org/wiki/Regula_falsi) (使用试位法求解非线性方程的根)
 *
 * @details
 * 试位法（False Position Method，又称 Regula Falsi）是一种求解单变量非线性方程的割线法变体。
 * 类似于二分法，试位法也需要保证根被夹在区间 $[x_1, x_2]$ 之间（即 $f(x_1) \cdot f(x_2) < 0$）。
 * 与二分法粗暴取中点不同，试位法通过连接点 $(x_1, f(x_1))$ 和 $(x_2, f(x_2))$ 的割线与 $x$ 轴的交点作为下一个估计根：
 *
 * $x_3 = x_1 - \frac{f(x_1) \cdot (x_1 - x_2)}{f(x_1) - f(x_2)}$
 *
 * 计算出 $f(x_3)$ 后，根据符号选择缩减区间为 $[x_1, x_3]$ 还是 $[x_2, x_3]$，以确保区间内始终包含根。
 *
 * 时间复杂度: 介于 $O(\log N)$ 到 $O(N)$ 之间。
 * 空间复杂度: $O(1)$（迭代实现）
 *
 * @note
 * 【区间符号判定缺失与递归爆栈 Bug 审计与修复】：
 * 1. **无条件区间迭代导致的失效与死循环 Bug**：
 *    原程序在计算出 $x_3$ 后，不管 $f(x_3)$ 的符号如何，无脑递归调用 `regula_falsi(x2, x3, y2, y3)`。
 *    如果在 $[x_2, x_3]$ 区间内端点符号相同（即不包含根），该割线迭代会立即跑偏甚至发散到无穷，导致算法彻底失效。
 *    原程序能跑通只是因为测试方程 $x^2 - x = 0$ 的根 0 和 1 恰好是步长 0.5 的整数倍，在主循环中被 `b == 0` 直接拦截拦截输出，
 *    **核心的 `regula_falsi` 实际从未在非整点根下运行过**。若换成 $\sqrt{2}$ 等无理数根，原程序会直接发散或无限递归导致崩溃。
 *    **修复**：重构为迭代实现（消除递归爆栈风险），并在每次更新时严格根据 $f(x_1) \cdot f(x_3) < 0$ 判断零点区间，正确收缩边界。
 * 2. **除零防护**：在 $f(x_1) == f(x_2)$ 时退出以防分母除零崩溃。
 *
 * @see newton_raphson_method.cpp, bisection_method.cpp
 * @author [Samruddha Patil](https://github.com/sampatil578)
 */

#include <cassert>
#include <cmath>     
#include <iostream>  
#include <vector>

namespace numerical_methods {
namespace false_position {

/**
 * @brief 待求根的目标连续方程: f(x) = x^2 - x
 */
double eq(double x) {
    return (x * x - x);
}

/**
 * @brief 使用试位法（Regula Falsi）迭代求解区间内的根
 * @param x1 区间端点 1
 * @param x2 区间端点 2
 * @return 求解出的根近似值
 */
double regula_falsi(double x1, double x2) {
    double y1 = eq(x1);
    double y2 = eq(x2);

    // 确保输入区间确实包含根
    assert(y1 * y2 < 0.0 && "Initial endpoints must have opposite signs!");

    double x3 = x1;
    double y3 = y1;
    
    // 迭代限制以防止由于数值精度限制陷入死循环
    for (int iter = 0; iter < 1000; ++iter) {
        if (std::abs(x1 - x2) < 1e-6) {
            break;
        }
        if (std::abs(y1 - y2) < 1e-15) {
            break; // 核心修复：防止分母除零崩溃
        }

        // 计算割线与 x 轴的交点坐标
        x3 = x1 - (x1 - x2) * y1 / (y1 - y2);
        y3 = eq(x3);

        if (std::abs(y3) < 1e-6) {
            break; // 已达到足够精度，退出
        }

        // 核心修复：基于符号判定收缩区间范围，保证根始终被夹在区间内
        if (y1 * y3 < 0.0) {
            x2 = x3;
            y2 = y3;
        } else {
            x1 = x3;
            y1 = y3;
        }
    }
    return x3;
}

}  // namespace false_position
}  // namespace numerical_methods

/**
 * @brief 单元自测用例
 */
static void test() {
    using namespace numerical_methods::false_position;

    // 测试 f(x) = x^2 - x = 0 在区间 [0.5, 1.5] 之间的根（预期为 1.0）
    double root1 = regula_falsi(0.5, 1.5);
    std::cout << "Root in [0.5, 1.5] is: " << root1 << "\n";
    assert(std::abs(root1 - 1.0) < 1e-4);

    // 测试 f(x) = x^2 - x = 0 在区间 [-0.5, 0.5] 之间的根（预期为 0.0）
    double root2 = regula_falsi(-0.5, 0.5);
    std::cout << "Root in [-0.5, 0.5] is: " << root2 << "\n";
    assert(std::abs(root2 - 0.0) < 1e-4);

    std::cout << "All false position method tests passed successfully!" << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    test(); // 运行自动单元测试
    return 0;
}
