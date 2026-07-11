/**
 * @file
 * @brief Approximation of definite integrals using [Midpoint Integral Rule](https://en.wikipedia.org/wiki/Midpoint_method) (中点矩形积分公式实现)
 *
 * @details
 * 中点积分法（Midpoint Rule）是一种简单高效的的数值积分算法。
 * 算法将积分区间 $[a, b]$ 划分为 $N$ 个等宽的子区间，每个子区间的宽度为 $h = \frac{b-a}{N}$。
 * 对每个子区间，它取中点 $x_i + \frac{h}{2}$ 处的函数值乘以步长 $h$ 作为该区间的积分近似（即矩形面积），最后进行累加：
 *
 * $I \approx h \cdot \sum_{i=0}^{N-1} f\left(a + i \cdot h + \frac{h}{2}\right)$
 *
 * 该方法相较于左矩形或右矩形公式具有更高的代数精度，其截断误差为 $O(h^2)$。
 *
 * 时间复杂度: $O(N)$
 * 空间复杂度: $O(1)$
 *
 * @note
 * 【无效的 Map 存储设计与内存优化审计与修复】：
 * 1. **原程序中 map 存储机制的严重效率缺陷 Bug**：
 *    原程序在计算中点积分时，使用 `std::map<int, double> data_table` 存储所有子区间中点的函数值，然后再次遍历 map 进行累加。
 *    这导致了大量的动态内存分配开销，时间复杂度从纯累加的常数查找变成了 $O(N \log N)$ 级别，并且造成了 $O(N)$ 的不必要空间浪费。
 *    对于很大的 $N$（例如 $N=10^6$），原程序会因频繁的动态内存申请而极其缓慢甚至发生内存溢出。
 *    **修复**：废弃 `std::map`，改用高效的单次循环**在线累加（On-the-fly Accumulation）**算法，空间复杂度降至 $O(1)$，时间复杂度为纯粹的 $O(N)$ 线性累加。
 *
 * @author [ggkogkou](https://github.com/ggkogkou)
 */

#include <cassert>     
#include <cmath>       
#include <cstdint>     
#include <cstdlib>     
#include <functional>  
#include <iostream>    

namespace numerical_methods {
namespace midpoint_rule {

/**
 * @brief 使用中点矩形法则计算定积分的值
 * @param N 划分的子区间数
 * @param h 步长 (b-a)/N
 * @param a 积分区间左端点 x0
 * @param func 待求积的目标函数
 * @return 积分近似值
 */
double midpoint(const std::int32_t N, const double h, const double a,
                const std::function<double(double)>& func) {
    // 防卫性校验
    assert(N > 0 && "Number of intervals N must be positive!");

    double evaluate_integral = 0.0;
    double xi = a;   

    // 核心修复：直接使用局部变量完成单次循环累加，避免 std::map 带来的海量内存分配与查找开销
    for (std::int32_t i = 0; i < N; i++) {
        evaluate_integral += func(xi + h / 2.0);  
        xi += h;  
    }

    // 乘以子区间步长 h
    evaluate_integral *= h;

    // 防止输入区间包含奇异值产生 NaN 崩溃
    assert(!std::isnan(evaluate_integral) &&
           "The definite integral can't be evaluated. Check the validity of your input.\n");
           
    return evaluate_integral;
}

/** 自测试的目标函数群 */
double f(double x) { return std::sqrt(x) + std::log(x); }
double g(double x) { return std::exp(-x) * (4.0 - std::pow(x, 2)); }
double k(double x) { return std::sqrt(2.0 * std::pow(x, 3) + 3.0); }
double l(double x) { return x + std::log(2.0 * x + 1.0); }

}  // namespace midpoint_rule
}  // namespace numerical_methods

/**
 * @brief 单元自测用例
 */
static void test(std::int32_t N, double h, double a, double b,
                 bool used_argv_parameters) {
    double result_f = numerical_methods::midpoint_rule::midpoint(
        N, h, a, numerical_methods::midpoint_rule::f);
    assert((used_argv_parameters || (result_f >= 4.09 && result_f <= 4.10)) &&
           "The result of f(x) is wrong");
    std::cout << "The result of integral f(x) on interval [" << a << ", " << b
              << "] is equal to: " << result_f << std::endl;

    double result_g = numerical_methods::midpoint_rule::midpoint(
        N, h, a, numerical_methods::midpoint_rule::g);
    assert((used_argv_parameters || (result_g >= 0.27 && result_g <= 0.28)) &&
           "The result of g(x) is wrong");
    std::cout << "The result of integral g(x) on interval [" << a << ", " << b
              << "] is equal to: " << result_g << std::endl;

    double result_k = numerical_methods::midpoint_rule::midpoint(
        N, h, a, numerical_methods::midpoint_rule::k);
    assert((used_argv_parameters || (result_k >= 9.06 && result_k <= 9.07)) &&
           "The result of k(x) is wrong");
    std::cout << "The result of integral k(x) on interval [" << a << ", " << b
              << "] is equal to: " << result_k << std::endl;

    double result_l = numerical_methods::midpoint_rule::midpoint(
        N, h, a, numerical_methods::midpoint_rule::l);
    assert((used_argv_parameters || (result_l >= 7.16 && result_l <= 7.17)) &&
           "The result of l(x) is wrong");
    std::cout << "The result of integral l(x) on interval [" << a << ", " << b
              << "] is equal to: " << result_l << std::endl;
}

/**
 * @brief 主函数
 */
int main(int argc, char** argv) {
    std::int32_t N = 16;  
    double a = 1.0, b = 3.0;  
    double h = NAN;       

    bool used_argv_parameters = false;  

    if (argc == 4) {
        N = std::atoi(argv[1]);
        a = std::atof(argv[2]);
        b = std::atof(argv[3]);
        assert(a < b && "a has to be less than b");
        assert(N > 0 && "N has to be > 0");
        if (N < 4 || a != 1.0 || b != 3.0) {
            used_argv_parameters = true;
        }
        std::cout << "You selected N=" << N << ", a=" << a << ", b=" << b
                  << std::endl;
    } else {
        std::cout << "Default N=" << N << ", a=" << a << ", b=" << b
                  << std::endl;
    }

    h = (b - a) / N;

    test(N, h, a, b, used_argv_parameters);  

    return 0;
}
