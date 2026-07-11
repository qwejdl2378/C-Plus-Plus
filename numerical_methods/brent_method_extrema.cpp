/**
 * @file
 * @brief Find real extrema of a univariate real function in a given interval using [Brent's method](https://en.wikipedia.org/wiki/Brent%27s_method) (布伦特法求解单变量实函数的极值)
 *
 * @details
 * 布伦特方法（Brent's Method）是一种不需要导数的极值求解算法。
 * 它结合了黄金分割搜索法（Golden Section Search）的鲁棒性和二次插值法（Parabolic Interpolation）的高效收敛性。
 * 当二次插值步骤有效且落在当前区间内时使用二次插值，否则回退到黄金分割搜索。
 *
 * 时间复杂度: 介于 $O(\log N)$ 到 $O(N)$ 之间，在多数光滑函数下收敛极快。
 * 空间复杂度: $O(1)$
 *
 * @note
 * 【实数定义域越界与极值概念注释错误 Bug 审计与修复】：
 * 1. **数学定义域未定义导致 NaN 崩溃 Bug**：
 *    在 `test2` 中，目标函数为 $f(x) = -x^{1/x}$。该函数在实数范围内仅在 $x > 0$ 时有定义。
 *    然而，原测试中却将搜索区间设为了 $[-2, 5]$，包含了负数区间。一旦迭代点 $u \le 0$，
 *    `pow(x, 1/x)` 会产生 `NaN`，破坏所有的不等式比较，导致迭代器彻底失效。
 *    **修复**：将 `test2` 的搜索区间修改为正数域 $[1.0, 5.0]$，确保不会计算负数分式幂。
 * 2. **注释概念错误**：
 *    `test3` 中注释写着“find maxima for cos(x)”，然而其返回极小值（Minima）$\pi$，
 *    因为 $\cos(\pi) = -1$ 是局部极小值。
 *    **修复**：修正注释，明确其为寻找极小值（Minima）。
 *
 * @see golden_search_extrema.cpp
 * @author [Krishna Vedala](https://github.com/kvedala)
 */

#define _USE_MATH_DEFINES  
#include <cassert>
#include <cmath>
#include <cstdint>
#include <functional>
#include <iostream>
#include <limits>

/** 系统收敛精度上限 */
#define EPSILON \
    std::sqrt(  \
        std::numeric_limits<double>::epsilon())  

namespace numerical_methods {
namespace brent_extrema {

/**
 * @brief 在区间内使用布伦特方法寻找函数的极小值点
 * @param f 目标实函数
 * @param lim_a 搜索区间的下界
 * @param lim_b 搜索区间的上界
 * @return 极小值对应的自变量 x 坐标
 */
double get_minima(const std::function<double(double)> &f, double lim_a,
                  double lim_b) {
    uint32_t iters = 0;

    if (lim_a > lim_b) {
        std::swap(lim_a, lim_b);
    } else if (std::abs(lim_a - lim_b) <= EPSILON) {
        std::cerr << "Search range must be greater than " << EPSILON << "\n";
        return lim_a;
    }

    // 黄金分割系数数值 (精确双精度)
    const double M_GOLDEN_RATIO = (3.0 - std::sqrt(5.0)) / 2.0;

    double v = lim_a + M_GOLDEN_RATIO * (lim_b - lim_a);
    double u = 0.0, w = v, x = v;
    double fu = 0.0, fv = f(v);
    double fw = fv, fx = fv;

    double mid_point = (lim_a + lim_b) / 2.0;
    double p = 0.0, q = 0.0, r = 0.0;
    double d = 0.0, e = 0.0;
    double tolerance = 0.0, tolerance2 = 0.0;

    do {
        mid_point = (lim_a + lim_b) / 2.0;
        tolerance = EPSILON * std::abs(x);
        tolerance2 = 2.0 * tolerance;

        if (std::abs(e) > tolerance2) {
            // 尝试抛物线二次插值拟合
            r = (x - w) * (fx - fv);
            q = (x - v) * (fx - fw);
            p = (x - v) * q - (x - w) * r;
            q = 2.0 * (q - r);
            if (q > 0.0) {
                p = -p;
            } else {
                q = -q;
            }
            r = e;
            e = d;
        }

        if (std::abs(p) < std::abs(0.5 * q * r) && p < q * (lim_b - x)) {
            // 采纳二次插值步骤
            d = p / q;
            u = x + d;
            if (u - lim_a < tolerance2 || lim_b - u < tolerance2) {
                d = x < mid_point ? tolerance : -tolerance;
            }
        } else {
            // 回退到黄金分割步骤
            e = (x < mid_point ? lim_b : lim_a) - x;
            d = M_GOLDEN_RATIO * e;
        }

        // 避免过于靠近当前最优估算点 x
        if (std::abs(d) >= tolerance) {
            u = d;
        } else if (d > 0.0) {
            u = tolerance;
        } else {
            u = -tolerance;
        }
        u += x;
        fu = f(u);

        // 更新搜索边界以及局部极小值估计变量
        if (fu <= fx) {
            if (u < x) {
                lim_b = x;
            } else {
                lim_a = x;
            }
            v = w;
            fv = fw;
            w = x;
            fw = fx;
            x = u;
            fx = fu;
        } else {
            if (u < x) {
                lim_a = u;
            } else {
                lim_b = u;
            }
            if (fu <= fw || x == w) {
                v = w;
                fv = fw;
                w = u;
                fw = fu;
            } else if (fu <= fv || v == x || v == w) {
                v = u;
                fv = fu;
            }
        }

        iters++;
    } while (std::abs(x - mid_point) > (tolerance2 - (lim_b - lim_a) / 2.0));

    std::cout << " (iters: " << iters << ") ";

    return x;
}

}  // namespace brent_extrema
}  // namespace numerical_methods

/**
 * @brief 测试用例 1: 求 f(x) = (x-2)^2 的极小值点，预期为 2
 */
void test1() {
    std::function<double(double)> f1 = [](double x) {
        return (x - 2.0) * (x - 2.0);
    };

    std::cout << "Test 1.... ";
    double minima = numerical_methods::brent_extrema::get_minima(f1, -1.0, 5.0);
    std::cout << minima << "...";

    assert(std::abs(minima - 2.0) < 1e-7);
    std::cout << "passed\n";
}

/**
 * @brief 测试用例 2: 求 f(x) = -x^(1/x) 的极小值点，预期为自然常数 e
 * @note 核心修复：修改区间为 [1.0, 5.0] 避免对负数进行幂运算产生 NaN
 */
void test2() {
    std::function<double(double)> func = [](double x) {
        assert(x > 0.0 && "Domain error: x must be strictly positive!");
        return -std::pow(x, 1.0 / x);
    };

    std::cout << "Test 2.... ";
    double minima = numerical_methods::brent_extrema::get_minima(func, 1.0, 5.0);
    std::cout << minima << " (" << M_E << ")...";

    assert(std::abs(minima - M_E) < 1e-7);
    std::cout << "passed\n";
}

/**
 * @brief 测试用例 3: 寻找 f(x) = cos(x) 的局部极小值点，预期为 pi
 */
void test3() {
    std::function<double(double)> func = [](double x) { return std::cos(x); };

    std::cout << "Test 3.... ";
    double minima = numerical_methods::brent_extrema::get_minima(func, -4.0, 12.0);
    std::cout << minima << " (" << M_PI << ")...";

    assert(std::abs(minima - M_PI) < 1e-7);
    std::cout << "passed\n";
}

/**
 * @brief 主函数
 */
int main() {
    std::cout.precision(18);
    std::cout << "Computations performed with machine epsilon: " << EPSILON << "\n";

    test1();
    test2();
    test3();

    return 0;
}
