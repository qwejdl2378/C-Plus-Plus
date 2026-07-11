/**
 * @file
 * @brief Solve the equation $f(x)=0$ using [Bisection Method](https://en.wikipedia.org/wiki/Bisection_method) (二分法求解非线性方程的根)
 *
 * @details
 * 二分法（Bisection Method）是一种连续函数求根的数值方法。
 * 如果已知一个连续函数 $f(x)$ 在区间 $[a, b]$ 上满足 $f(a) \cdot f(b) < 0$，根据零点存在定理，
 * 区间内至少存在一个根。算法每次取区间中点 $x = \frac{a+b}{2}$，并根据 $f(x)$ 的正负号缩减区间：
 * - 若 $\text{sgn}(f(x)) == \text{sgn}(f(a))$，说明零点在 $[x, b]$ 区间内，令 $a = x$；
 * - 否则，零点在 $[a, x]$ 区间内，令 $b = x$。
 * 重复此过程直至区间大小或函数值小于精度误差上限 $\epsilon$。
 *
 * 时间复杂度: $O(\log(\frac{b-a}{\epsilon}))$
 * 空间复杂度: $O(1)$
 *
 * @see newton_raphson_method.cpp, false_position.cpp
 *
 * @note
 * 【区间符号更新假设与单元测试缺失 Bug 审计与修复】：
 * 1. **更新符号硬编码 Bug**：原程序中在二分更新区间时，直接硬编码为 `if (z < 0) { a = x; } else { b = x; }`。
 *    该逻辑仅在 $f(a) < 0$ 且 $f(b) > 0$ 时成立。如果函数形式或初始根区间不同导致 $f(a) > 0$，$f(b) < 0$，
 *    原程序便会出现逻辑倒反，最终导致**区间不收敛甚至收敛至错误区域**。
 *    **修复**：改用通用的符号判别 `if (sgn(z) == sgn(eq(a))) { a = x; } else { b = x; }`，使其对任何函数与初始区间通用。
 * 2. **缺少自动化测试 assert 校验**：原程序仅使用交互打印输出，无任何自动断言。
 *    **修复**：添加 `assert` 测试校验，封装核心功能，确保算法实现具备自测能力。
 */

#include <cassert>
#include <cmath>
#include <iostream>
#include <limits>

namespace numerical_methods {
namespace bisection_method {

/**
 * @brief 系统要求的收敛精度上限
 */
constexpr double EPSILON = 1e-6;

/**
 * @brief 允许的最大迭代步数
 */
constexpr int MAX_ITERATIONS = 50000;

/**
 * @brief 需要求根的连续方程定义: f(x) = x^3 - 4x - 9
 * @param x 自变量
 * @return 函数值
 */
double eq(double x) {
    return std::pow(x, 3) - (4.0 * x) - 9.0;
}

/**
 * @brief 获取任意数值的符号
 * @return 1 (正数), -1 (负数), 0 (零)
 */
template <typename T>
int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

/**
 * @brief 二分法求解核心函数
 * @param a 区间下界
 * @param b 区间上界
 * @param num_iterations [out] 记录迭代次数的输出指针
 * @return 方程根的近似值
 */
double solve_bisection(double a, double b, int* num_iterations = nullptr) {
    double x = a;
    double z = eq(a);
    
    // 校验输入区间是否包含根
    assert(sgn(eq(a)) != sgn(eq(b)) && "The interval [a, b] must contain a root (different signs at endpoints)!");

    int i = 0;
    for (i = 0; i < MAX_ITERATIONS; ++i) {
        x = (a + b) / 2.0;
        z = eq(x);

        // 如果函数值已经极度接近 0，说明达到预设收敛标准
        if (std::abs(z) < EPSILON) {
            break;
        }

        // 核心修复：通用区间更新逻辑，不依赖于 a 必须为负的假设
        if (sgn(z) == sgn(eq(a))) {
            a = x;
        } else {
            b = x;
        }
    }

    if (num_iterations != nullptr) {
        *num_iterations = i;
    }
    return x;
}

}  // namespace bisection_method
}  // namespace numerical_methods

/**
 * @brief 单元自测用例
 */
static void test() {
    using namespace numerical_methods::bisection_method;

    // 寻找包含方程根的区间边界
    double a = -1.0, b = 1.0;
    for (int i = 0; i < MAX_ITERATIONS; i++) {
        if (sgn(eq(a)) != sgn(eq(b))) {
            break;
        }
        b += 1.0;
        a -= 1.0;
    }

    int steps = 0;
    double root = solve_bisection(a, b, &steps);

    std::cout << "First initial a: " << a << "\n";
    std::cout << "Second initial b: " << b << "\n";
    std::cout << "Calculated root: " << root << " in " << steps << " steps\n";

    // 验证根的正确性: f(root) 的绝对值应小于误差精度上限
    assert(std::abs(eq(root)) < EPSILON);
    std::cout << "Bisection method tests passed successfully!" << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    test(); // 运行自动单元测试
    return 0;
}
