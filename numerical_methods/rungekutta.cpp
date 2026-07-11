/**
 * @file
 * @brief Solve a first-order ordinary differential equation (ODE) using [Fourth-Order Runge-Kutta method](https://en.wikipedia.org/wiki/Runge%E2%80%93Kutta_methods) (四阶龙格-库塔法求解一阶常微分方程)
 *
 * @details
 * 四阶龙格-库塔法（Runge-Kutta 4th Order, RK4）是一种经典的显式高精度数值积分方法，用于求解一阶初值问题 $y' = f(x, y), y(x_0) = y_0$。
 * RK4 在一个步长内通过计算四个位置的斜率并做加权平均，使单步局部截断误差达到 $O(h^5)$，累积全局误差达到 $O(h^4)$：
 * - $k_1 = h \cdot f(x_n, y_n)$
 * - $k_2 = h \cdot f(x_n + \frac{h}{2}, y_n + \frac{k_1}{2})$
 * - $k_3 = h \cdot f(x_n + \frac{h}{2}, y_n + \frac{k_2}{2})$
 * - $k_4 = h \cdot f(x_n + h, y_n + k_3)$
 * - $y_{n+1} = y_n + \frac{1}{6}(k_1 + 2k_2 + 2k_3 + k_4)$
 *
 * 时间复杂度: $O(\frac{x_{max} - x_0}{h})$
 * 空间复杂度: $O(1)$
 *
 * @note
 * 【浮点数精确相等断言与通用性缺失 Bug 审计与修复】：
 * 1. **浮点数硬编码精确相等比较 Bug**：
 *    原单元测试中，直接使用双精度浮点数的精确相等判定 `assert(valfirst == 3.10363932323749570)`。
 *    由于不同 CPU 架构（如 x86-64 的 x87 FPU 与 ARM64 的 NEON 寄存器）以及不同的编译器优化级别（如 `-ffast-math`）
 *    对浮点舍入的处理可能存在微小差异，**精确相等判定在跨平台或不同编译配置下极易导致断言失败**。
 *    **修复**：修改断言方式，采用绝对误差限 `std::abs(val - expected) < 1e-9` 进行容差校验。
 * 2. **微分方程硬编码通用性差**：
 *    原实现中将方程 $y' = \frac{x-y}{2}$ 硬编码在静态函数 `change` 内，无法作为一个通用的求解库重用。
 *    **修复**：将目标微分函数参数化为 `std::function<double(double, double)>`，提升算法的通用性。
 *
 * @author [Rudra Prasad Das](http://github.com/rudra697)
 */

#include <cassert>   
#include <cmath>
#include <cstdint>
#include <functional>
#include <iostream>  
#include <vector>    

namespace numerical_methods {
namespace runge_kutta {

/**
 * @brief 使用四阶龙格-库塔法 (RK4) 计算常微分方程的近似解
 * @param init_x 独立变量的初始值 x0
 * @param init_y 依赖变量的初始值 y0 = y(x0)
 * @param x 待求解的目标 x 坐标值
 * @param h 积分迭代步长
 * @param f 目标微分方程函数 y' = f(x, y)
 * @return 对应目标自变量 x 处的 y 值估计
 */
double rungeKutta(double init_x, double init_y, double x, double h,
                  const std::function<double(double, double)> &f) {
    if (h <= 0.0) {
        throw std::invalid_argument("Step size h must be strictly positive!");
    }
    if (x < init_x) {
        throw std::invalid_argument("Target x must be greater than or equal to initial x!");
    }

    auto n = static_cast<uint64_t>((x - init_x) / h);
    double y = init_y;

    for (uint64_t i = 1; i <= n; ++i) {
        // RK4 斜率估算公式
        double k1 = h * f(init_x, y);
        double k2 = h * f(init_x + 0.5 * h, y + 0.5 * k1);
        double k3 = h * f(init_x + 0.5 * h, y + 0.5 * k2);
        double k4 = h * f(init_x + h, y + k3);

        // 加权累加更新
        y += (1.0 / 6.0) * (k1 + 2.0 * k2 + 2.0 * k3 + k4);
        init_x += h;
    }

    return y;
}

}  // namespace runge_kutta
}  // namespace numerical_methods

/** 示例微分方程: dy/dx = (x - y) / 2 */
static double example_ode(double x, double y) {
    return (x - y) / 2.0;
}

/**
 * @brief 单元自测用例
 */
static void test() {
    using namespace numerical_methods::runge_kutta;
    constexpr double epsilon = 1e-9;

    std::cout << "Starting Runge-Kutta 4th order tests...\n";

    // 测试 1
    double valfirst = rungeKutta(2.0, 3.0, 4.0, 0.2, example_ode);
    std::cout << "Test 1 calculated: " << valfirst << "\n";
    assert(std::abs(valfirst - 3.10363932323749570) < epsilon);
    std::cout << "Passed Test 1\n";

    // 测试 2
    double valsec = rungeKutta(1.0, 2.0, 5.0, 0.1, example_ode);
    std::cout << "Test 2 calculated: " << valsec << "\n";
    assert(std::abs(valsec - 3.40600589380261409) < epsilon);
    std::cout << "Passed Test 2\n";

    // 测试 3
    double valthird = rungeKutta(-1.0, 3.0, 4.0, 0.1, example_ode);
    std::cout << "Test 3 calculated: " << valthird << "\n";
    assert(std::abs(valthird - 2.49251005860244268) < epsilon);
    std::cout << "Passed Test 3\n";

    std::cout << "All Runge-Kutta tests passed successfully!\n";
}

/**
 * @brief 主函数
 */
int main() {
    test();  
    return 0;
}
