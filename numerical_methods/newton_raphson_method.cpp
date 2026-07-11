/**
 * @file
 * @brief Solve the equation $f(x)=0$ using [Newton-Raphson method](https://en.wikipedia.org/wiki/Newton%27s_method) (使用牛顿-拉夫森迭代法求根)
 *
 * @details
 * 牛顿-拉夫森方法（Newton-Raphson Method）是一种在实数域和复数域上近似求解方程 $f(x) = 0$ 的非线性迭代方法。
 * 它的几何意义是在当前点 $x_i$ 处切线与 $x$ 轴的交点作为下一步的迭代值：
 *
 * $x_{i+1} = x_i - \frac{f(x_i)}{f'(x_i)}$
 *
 * 该方法在根附近具有二次收敛速度（局部二次收敛）。
 *
 * 时间复杂度: $O(\text{迭代次数})$，在根附近通常少于 10 次迭代。
 * 空间复杂度: $O(1)$
 *
 * @note
 * 【导数除零异常与不确定性测试 Bug 审计与修复】：
 * 1. **切线斜率零值导致除以零崩溃 Bug**：
 *    当迭代值 $x_i$ 的导数 $f'(x_i) \approx 0$（切线几乎平行于 $x$ 轴）时，
 *    计算 $\frac{f(x_i)}{f'(x_i)}$ 会导致除以零崩溃，或者迭代点被抛射至无穷远处，导致计算发散。
 *    原程序中未做任何除零防卫。
 *    **修复**：在迭代中增加对导数绝对值的校验，若低于阈值 `1e-15`，则抛出异常或停止迭代。
 * 2. **随机初始化导致单元测试不稳定（Flakiness）Bug**：
 *    原实现使用 `std::rand() % 100` 作为初始值，在流水线自动化测试中具有非确定性，有可能生成不良初值导致发散。
 *    **修复**：设计标准确定性的初始值（如 `initial_guess = 5.0`），并增加断言。
 *
 * @see bisection_method.cpp, false_position.cpp
 * @author [Krishna Vedala](https://github.com/kvedala)
 */

#include <cassert>
#include <cmath>
#include <cstdint>
#include <functional>
#include <iostream>
#include <limits>
#include <stdexcept>

namespace numerical_methods {
namespace newton_raphson {

/** 系统收敛精度上限 */
constexpr double EPSILON = 1e-10;
/** 最大迭代次数 */
constexpr int32_t MAX_ITERATIONS = 1000;

/**
 * @brief 牛顿-拉夫森法求根核心函数
 * @param initial_guess 迭代初始猜测点
 * @param f 目标函数
 * @param df 目标函数的导函数
 * @return 求解出的根近似值
 */
double solve(double initial_guess, 
             const std::function<double(double)> &f, 
             const std::function<double(double)> &df) {
    double x = initial_guess;
    
    for (int32_t i = 0; i < MAX_ITERATIONS; ++i) {
        double fx = f(x);
        double dfx = df(x);

        // 核心修复：防止导数极度接近 0 导致的除零崩溃或发散异常
        if (std::abs(dfx) < 1e-7) {
            throw std::runtime_error("Derivative is zero or extremely close to zero, cannot continue iteration!");
        }

        double next_x = x - (fx / dfx);

        // 如果变化量或函数值已足够接近 0，则达到精度退出
        if (std::abs(fx) < EPSILON || std::abs(next_x - x) < EPSILON) {
            return next_x;
        }
        x = next_x;
    }
    return x;
}

/** 待求根的目标方程: f(x) = x^3 - 4x - 9 */
double eq(double x) {
    return std::pow(x, 3) - (4.0 * x) - 9.0;
}

/** 目标方程的导数: f'(x) = 3x^2 - 4 */
double eq_der(double x) {
    return (3.0 * std::pow(x, 2)) - 4.0;
}

}  // namespace newton_raphson
}  // namespace numerical_methods

/**
 * @brief 单元自测用例
 */
static void test() {
    using namespace numerical_methods::newton_raphson;

    // 测试方程 x^3 - 4x - 9 = 0 的根，设定确定性初值 5.0
    double root = solve(5.0, eq, eq_der);
    std::cout << "Newton-Raphson calculated root: " << root << "\n";

    // 验证根的正确性: f(root) 应该在 EPSILON 精度内为 0
    assert(std::abs(eq(root)) < EPSILON);

    // 校验异常处理：若在导数为 0 处开始（例如 f'(x) = 3x^2 - 4，令 x = 2/sqrt(3)）
    try {
        double zero_deriv_x = 2.0 / std::sqrt(3.0);
        solve(zero_deriv_x, eq, eq_der);
        assert(false && "Expected exception due to zero derivative, but it didn't throw!");
    } catch (const std::runtime_error &e) {
        std::cout << "Caught expected zero derivative exception: " << e.what() << "\n";
    }

    std::cout << "All Newton-Raphson tests passed successfully!" << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    test(); // 运行单元自测
    return 0;
}
