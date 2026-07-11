/**
 * @file
 * @brief Find extrema of a univariate real function in a given interval using [Golden Section Search Algorithm](https://en.wikipedia.org/wiki/Golden-section_search) (黄金分割搜索算法求单变量函数极值)
 *
 * @details
 * 黄金分割搜索法是一种在已知单峰函数极值所在区间的条件下，通过不断收缩区间来寻找极值点的数值算法。
 * 该方法类似于二分法，但它利用黄金分割比（约 0.618）来确定内部两个试探点 $c$ 和 $d$：
 * - $c = b - \frac{b - a}{\phi}$
 * - $d = a + \frac{b - a}{\phi}$
 * 其中 $\phi = \frac{1 + \sqrt{5}}{2}$ 为黄金比例。
 * 每次迭代评估 $f(c)$ 和 $f(d)$，将不包含极值的区间舍弃，直到区间长度小于 $\epsilon$。
 *
 * 时间复杂度: $O(\log(\frac{b-a}{\epsilon}))$
 * 空间复杂度: $O(1)$
 *
 * @note
 * 【NaN 崩溃、过时均值精度损失与极值注释错误 Bug 审计与修复】：
 * 1. **定义域未定义 NaN 崩溃 Bug**：
 *    在 `test2` 中，目标函数为 $f(x) = -x^{1/x}$。该函数在实数域仅在 $x > 0$ 时有定义。
 *    原程序将搜索区间设为 $[-2, 10]$，在负数域迭代计算会直接产生 `NaN`，导致所有的比较判定失效。
 *    **修复**：将测试区间收缩至正数域 $[1.0, 10.0]$。
 * 2. **返回过时均值的精度损失 Bug**：
 *    原程序在退出循环时返回了 `prev_mean`。这是**前一次迭代的区间均值**，其误差明显大于当前已经收敛的最新区间均值。
 *    **修复**：直接返回最新收敛区间的中点 `(lim_a + lim_b) / 2.0`。
 * 3. **注释与概念不一致**：
 *    `test3` 中注释写着“find maxima for cos(x)”，然而实际寻找的是极小值（Minima）$\pi$。
 *    **修复**：修正注释文字描述。
 *
 * @see brent_method_extrema.cpp
 * @author [Krishna Vedala](https://github.com/kvedala)
 */

#define _USE_MATH_DEFINES  
#include <cassert>
#include <cmath>
#include <cstdint>
#include <functional>
#include <iostream>
#include <limits>

/** 收敛精度上限 */
constexpr double EPSILON = 1e-7;

namespace numerical_methods {
namespace golden_search {

/**
 * @brief 使用黄金分割法在区间 [lim_a, lim_b] 内寻找函数的极小值点
 * @param f 目标单峰函数
 * @param lim_a 搜索区间下界
 * @param lim_b 搜索区间上界
 * @return 极小值点对应的自变量 x 坐标
 */
double get_minima(const std::function<double(double)> &f, double lim_a,
                  double lim_b) {
    uint32_t iters = 0;
    double c = 0.0, d = 0.0;

    // 黄金比例常量
    const double M_GOLDEN_RATIO = (1.0 + std::sqrt(5.0)) / 2.0;

    // 保证区间下界小于上界
    if (lim_a > lim_b) {
        std::swap(lim_a, lim_b);
    } else if (std::abs(lim_a - lim_b) <= EPSILON) {
        std::cerr << "Search range must be greater than " << EPSILON << "\n";
        return lim_a;
    }

    while (std::abs(lim_a - lim_b) > EPSILON) {
        // 计算分割比例宽度
        double ratio = (lim_b - lim_a) / M_GOLDEN_RATIO;
        c = lim_b - ratio;  
        d = lim_a + ratio;  

        if (f(c) < f(d)) {
            // 极值在左侧子区间内，收缩右端点
            lim_b = d;
        } else {
            // 极值在右侧子区间内，收缩左端点
            lim_a = c;
        }
        iters++;
    }

    std::cout << " (iters: " << iters << ") ";
    // 核心修复：直接返回最新且最高精度的中点估计值，而非旧的 prev_mean
    return (lim_a + lim_b) / 2.0;
}

}  // namespace golden_search
}  // namespace numerical_methods

/**
 * @brief 测试用例 1: 寻找 f(x) = (x-2)^2 的极小值点，预期为 2
 */
void test1() {
    std::function<double(double)> f1 = [](double x) {
        return (x - 2.0) * (x - 2.0);
    };

    std::cout << "Test 1.... ";
    double minima = numerical_methods::golden_search::get_minima(f1, 1.0, 5.0);
    std::cout << minima << "...";

    assert(std::abs(minima - 2.0) < EPSILON);
    std::cout << "passed\n";
}

/**
 * @brief 测试用例 2: 寻找 f(x) = -x^(1/x) 的极小值点，预期为 e
 * @note 核心修复：将区间设为 [1.0, 10.0]，防止负数幂运算导致 NaN
 */
void test2() {
    std::function<double(double)> func = [](double x) {
        assert(x > 0.0 && "Domain error: x must be strictly positive!");
        return -std::pow(x, 1.0 / x);
    };

    std::cout << "Test 2.... ";
    double minima = numerical_methods::golden_search::get_minima(func, 1.0, 10.0);
    std::cout << minima << " (" << M_E << ")...";

    assert(std::abs(minima - M_E) < EPSILON);
    std::cout << "passed\n";
}

/**
 * @brief 测试用例 3: 寻找 f(x) = cos(x) 的局部极小值点，预期为 pi
 */
void test3() {
    std::function<double(double)> func = [](double x) { return std::cos(x); };

    std::cout << "Test 3.... ";
    double minima = numerical_methods::golden_search::get_minima(func, -4.0, 12.0);
    std::cout << minima << " (" << M_PI << ")...";

    assert(std::abs(minima - M_PI) < EPSILON);
    std::cout << "passed\n";
}

/**
 * @brief 主函数
 */
int main() {
    std::cout.precision(9);
    std::cout << "Computations performed with machine epsilon: " << EPSILON << "\n";

    test1();
    test2();
    test3();

    return 0;
}
